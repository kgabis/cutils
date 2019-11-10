/*
 Copyright (c) 2019 Krzysztof Gabis
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "collections.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Dictionary
//-----------------------------------------------------------------------------

#define DICT_INVALID_IX UINT_MAX

typedef struct dict_ {
    unsigned int *cells;
    unsigned long *hashes;
    char **keys;
    void **values;
    unsigned int *cell_ixs;
    unsigned int count;
    unsigned int item_capacity;
    unsigned int cell_capacity;
} dict_t_;

// Private declarations
static bool dict_init(dict_t_ *hd, unsigned int initial_capacity);
static void dict_deinit(dict_t_ *hd, bool free_keys);
static unsigned int dict_get_cell_ix(const dict_t_ *hd,
                                     const char *key,
                                     unsigned long hash,
                                     bool *out_found);
static unsigned long hash_string(const char *str);
static bool dict_grow_and_rehash(dict_t_ *hd);
static bool dict_set_internal(dict_t_ *hd, const char *ckey, char *mkey, void *value);

// Public
dict_t_* dict_make(void) {
    dict_t_ *dict = malloc(sizeof(dict_t_));
    if (dict == NULL) {
        return NULL;
    }
    bool succeeded = dict_init(dict, 16);
    if (succeeded == false) {
        free(dict);
        return NULL;
    }
    return dict;
}

void dict_destroy(dict_t_ *dict) {
    if (dict == NULL) {
        return;
    }
    dict_deinit(dict, true);
    free(dict);
}

bool dict_set(dict_t_ *dict, const char *key, void *value) {
    return dict_set_internal(dict, key, NULL, value);
}

void *dict_get(const dict_t_ *dict, const char *key) {
    unsigned long hash = hash_string(key);
    bool found = false;
    unsigned long cell_ix = dict_get_cell_ix(dict, key, hash, &found);
    if (found == false) {
        return NULL;
    }
    unsigned int item_ix = dict->cells[cell_ix];
    return dict->values[item_ix];
}

void *dict_get_value_at(const dict_t_ *dict, unsigned int ix) {
    if (ix >= dict->count) {
        return NULL;
    }
    return dict->values[ix];
}

const char *dict_get_key_at(const dict_t_ *dict, unsigned int ix) {
    if (ix >= dict->count) {
        return NULL;
    }
    return dict->keys[ix];
}

unsigned int dict_count(const dict_t_ *dict) {
    if (!dict) {
        return 0;
    }
    return dict->count;
}

bool dict_remove(dict_t_ *dict, const char *key) {
    unsigned long hash = hash_string(key);
    bool found = false;
    unsigned int cell = dict_get_cell_ix(dict, key, hash, &found);
    if (!found) {
        return false;
    }

    unsigned int item_ix = dict->cells[cell];
    free(dict->keys[item_ix]);
    unsigned int last_item_ix = dict->count - 1;
    if (item_ix < last_item_ix) {
        dict->keys[item_ix] = dict->keys[last_item_ix];
        dict->values[item_ix] = dict->values[last_item_ix];
        dict->cell_ixs[item_ix] = dict->cell_ixs[last_item_ix];
        dict->hashes[item_ix] = dict->hashes[last_item_ix];
        dict->cells[dict->cell_ixs[item_ix]] = item_ix;
    }
    dict->count--;

    unsigned int i = cell;
    unsigned int j = i;
    for (unsigned int x = 0; x < (dict->cell_capacity - 1); x++) {
        j = (j + 1) & (dict->cell_capacity - 1);
        if (dict->cells[j] == DICT_INVALID_IX) {
            break;
        }
        unsigned int k = dict->hashes[dict->cells[j]] & (dict->cell_capacity - 1);
        if ((j > i && (k <= i || k > j))
            || (j < i && (k <= i && k > j))) {
            dict->cell_ixs[dict->cells[j]] = i;
            dict->cells[i] = dict->cells[j];
            i = j;
        }
    }
    dict->cells[i] = DICT_INVALID_IX;
    return true;
}

void dict_clear(dict_t_ *dict) {
    for (unsigned int i = 0; i < dict->count; i++) {
        free(dict->keys[i]);
    }
    dict->count = 0;
    for (unsigned int i = 0; i < dict->cell_capacity; i++) {
        dict->cells[i] = DICT_INVALID_IX;
    }
}

// Private definitions
static bool dict_init(dict_t_ *dict, unsigned int initial_capacity) {
    // todo: check initial capacity is a power of 2
    dict->cells = NULL;
    dict->keys = NULL;
    dict->values = NULL;
    dict->cell_ixs = NULL;
    dict->hashes = NULL;

    dict->count = 0;
    dict->cell_capacity = initial_capacity;
    dict->item_capacity = (unsigned int)(initial_capacity * 0.7f);

    dict->cells = malloc(dict->cell_capacity * sizeof(*dict->cells));
    dict->keys = malloc(dict->item_capacity * sizeof(*dict->keys));
    dict->values = malloc(dict->item_capacity * sizeof(*dict->values));
    dict->cell_ixs = malloc(dict->item_capacity * sizeof(*dict->cell_ixs));
    dict->hashes = malloc(dict->item_capacity * sizeof(*dict->hashes));
    if (dict->cells == NULL
        || dict->keys == NULL
        || dict->values == NULL
        || dict->cell_ixs == NULL
        || dict->hashes == NULL) {
        goto error;
    }
    for (unsigned int i = 0; i < dict->cell_capacity; i++) {
        dict->cells[i] = DICT_INVALID_IX;
    }
    return true;
error:
    free(dict->cells);
    free(dict->keys);
    free(dict->values);
    free(dict->cell_ixs);
    free(dict->hashes);
    return false;
}

static void dict_deinit(dict_t_ *dict, bool free_keys) {
    if (free_keys) {
        for (unsigned int i = 0; i < dict->count; i++) {
            free(dict->keys[i]);
        }
    }
    dict->count = 0;
    dict->item_capacity = 0;
    dict->cell_capacity = 0;

    free(dict->cells);
    free(dict->keys);
    free(dict->values);
    free(dict->cell_ixs);
    free(dict->hashes);

    dict->cells = NULL;
    dict->keys = NULL;
    dict->values = NULL;
    dict->cell_ixs = NULL;
    dict->hashes = NULL;
}

static unsigned int dict_get_cell_ix(const dict_t_ *dict,
                                     const char *key,
                                     unsigned long hash,
                                     bool *out_found)
{
    *out_found = false;
    unsigned int cell_ix = hash & (dict->cell_capacity - 1);
    for (unsigned int i = 0; i < dict->cell_capacity; i++) {
        unsigned int ix = (cell_ix + i) & (dict->cell_capacity - 1);
        unsigned int cell = dict->cells[ix];
        if (cell == DICT_INVALID_IX) {
            return ix;
        }
        unsigned long hash_to_check = dict->hashes[cell];
        if (hash != hash_to_check) {
            continue;
        }
        const char *key_to_check = dict->keys[cell];
        if (strcmp(key, key_to_check) == 0) {
            *out_found = true;
            return ix;
        }
    }
    return DICT_INVALID_IX;
}

static unsigned long hash_string(const char *str) { /* djb2 */
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    }
    return hash;
}

static bool dict_grow_and_rehash(dict_t_ *dict) {
    dict_t_ new_hd;
    bool succeeded = dict_init(&new_hd, dict->cell_capacity * 2);
    if (succeeded == false) {
        return false;
    }
    for (unsigned int i = 0; i < dict->count; i++) {
        char *key = dict->keys[i];
        void *value = dict->values[i];
        succeeded = dict_set_internal(&new_hd, key, key, value);
        if (succeeded == false) {
            dict_deinit(&new_hd, false);
            return false;
        }
    }
    dict_deinit(dict, false);
    *dict = new_hd;
    return true;
}

static bool dict_set_internal(dict_t_ *dict, const char *ckey, char *mkey, void *value) {
    unsigned long hash = hash_string(ckey);
    bool found = false;
    unsigned int cell_ix = dict_get_cell_ix(dict, ckey, hash, &found);
    if (found) {
        unsigned int item_ix = dict->cells[cell_ix];
        dict->values[item_ix] = value;
        return true;
    }
    if (dict->count >= dict->item_capacity) {
        bool succeeded = dict_grow_and_rehash(dict);
        if (succeeded == false) {
            return false;
        }
        cell_ix = dict_get_cell_ix(dict, ckey, hash, &found);
    }
    dict->cells[cell_ix] = dict->count;
    dict->keys[dict->count] = mkey != NULL ? mkey : strdup(ckey);
    dict->values[dict->count] = value;
    dict->cell_ixs[dict->count] = cell_ix;
    dict->hashes[dict->count] = hash;
    dict->count++;
    return true;
}

//-----------------------------------------------------------------------------
// Pointer dictionary
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

#define PTRDICT_INVALID_IX UINT_MAX

typedef struct ptrdict_ {
    unsigned int *cells;
    void **keys;
    void **values;
    unsigned int *cell_ixs;
    unsigned int count;
    unsigned int item_capacity;
    unsigned int cell_capacity;
} ptrdict_t_;

// Private declarations
static bool ptrdict_init(ptrdict_t_ *pd, unsigned int initial_capacity);
static void ptrdict_deinit(ptrdict_t_ *pd);
static unsigned int ptrdict_get_cell_ix(const ptrdict_t_ *pd, void *key, bool *out_found);
static bool ptrdict_grow_and_rehash(ptrdict_t_ *pd);
static bool ptrdict_set_internal(ptrdict_t_ *pd, void *key, void *value);

// Public
ptrdict_t_* ptrdict_make(void) {
    ptrdict_t_ *dict = malloc(sizeof(ptrdict_t_));
    if (dict == NULL) {
        return NULL;
    }
    bool succeeded = ptrdict_init(dict, 16);
    if (succeeded == false) {
        free(dict);
        return NULL;
    }
    return dict;
}

void ptrdict_destroy(ptrdict_t_ *dict) {
    if (dict == NULL) {
        return;
    }
    ptrdict_deinit(dict);
    free(dict);
}

bool ptrdict_set(ptrdict_t_ *dict, void *key, void *value) {
    return ptrdict_set_internal(dict, key, value);
}

void *ptrdict_get(const ptrdict_t_ *dict, void *key) {
    bool found = false;
    unsigned long cell_ix = ptrdict_get_cell_ix(dict, key, &found);
    if (found == false) {
        return NULL;
    }
    unsigned int item_ix = dict->cells[cell_ix];
    return dict->values[item_ix];
}

void *ptrdict_get_value_at(const ptrdict_t_ *dict, unsigned int ix) {
    if (ix >= dict->count) {
        return NULL;
    }
    return dict->values[ix];
}

void *ptrdict_get_key_at(const ptrdict_t_ *dict, unsigned int ix) {
    if (ix >= dict->count) {
        return NULL;
    }
    return dict->keys[ix];
}

unsigned int ptrdict_count(const ptrdict_t_ *dict) {
    if (!dict) {
        return 0;
    }
    return dict->count;
}

bool ptrdict_remove(ptrdict_t_ *dict, void *key) {
    bool found = false;
    unsigned int cell = ptrdict_get_cell_ix(dict, key, &found);
    if (!found) {
        return false;
    }

    unsigned int item_ix = dict->cells[cell];
    free(dict->keys[item_ix]);
    unsigned int last_item_ix = dict->count - 1;
    if (item_ix < last_item_ix) {
        dict->keys[item_ix] = dict->keys[last_item_ix];
        dict->values[item_ix] = dict->values[last_item_ix];
        dict->cell_ixs[item_ix] = dict->cell_ixs[last_item_ix];
        dict->cells[dict->cell_ixs[item_ix]] = item_ix;
    }
    dict->count--;

    unsigned int i = cell;
    unsigned int j = i;
    for (unsigned int x = 0; x < (dict->cell_capacity - 1); x++) {
        j = (j + 1) & (dict->cell_capacity - 1);
        if (dict->cells[j] == PTRDICT_INVALID_IX) {
            break;
        }
        unsigned int k = (uintptr_t)(dict->keys[dict->cells[j]]) & (dict->cell_capacity - 1);
        if ((j > i && (k <= i || k > j))
            || (j < i && (k <= i && k > j))) {
            dict->cell_ixs[dict->cells[j]] = i;
            dict->cells[i] = dict->cells[j];
            i = j;
        }
    }
    dict->cells[i] = PTRDICT_INVALID_IX;
    return true;
}

void ptrdict_clear(ptrdict_t_ *dict) {
    for (unsigned int i = 0; i < dict->count; i++) {
        free(dict->keys[i]);
    }
    dict->count = 0;
    for (unsigned int i = 0; i < dict->cell_capacity; i++) {
        dict->cells[i] = PTRDICT_INVALID_IX;
    }
}

// Private definitions
static bool ptrdict_init(ptrdict_t_ *dict, unsigned int initial_capacity) {
    // todo: check initial capacity is a power of 2
    dict->cells = NULL;
    dict->keys = NULL;
    dict->values = NULL;
    dict->cell_ixs = NULL;

    dict->count = 0;
    dict->cell_capacity = initial_capacity;
    dict->item_capacity = (unsigned int)(initial_capacity * 0.7f);

    dict->cells = malloc(dict->cell_capacity * sizeof(*dict->cells));
    dict->keys = malloc(dict->item_capacity * sizeof(*dict->keys));
    dict->values = malloc(dict->item_capacity * sizeof(*dict->values));
    dict->cell_ixs = malloc(dict->item_capacity * sizeof(*dict->cell_ixs));
    if (dict->cells == NULL
        || dict->keys == NULL
        || dict->values == NULL
        || dict->cell_ixs == NULL) {
        goto error;
    }
    for (unsigned int i = 0; i < dict->cell_capacity; i++) {
        dict->cells[i] = PTRDICT_INVALID_IX;
    }
    return true;
error:
    free(dict->cells);
    free(dict->keys);
    free(dict->values);
    free(dict->cell_ixs);
    return false;
}

static void ptrdict_deinit(ptrdict_t_ *dict) {
    dict->count = 0;
    dict->item_capacity = 0;
    dict->cell_capacity = 0;

    free(dict->cells);
    free(dict->keys);
    free(dict->values);
    free(dict->cell_ixs);

    dict->cells = NULL;
    dict->keys = NULL;
    dict->values = NULL;
    dict->cell_ixs = NULL;
}

static unsigned int ptrdict_get_cell_ix(const ptrdict_t_ *dict, void *key, bool *out_found) {
    *out_found = false;
    unsigned int cell_ix = (uintptr_t)key & (dict->cell_capacity - 1);
    for (unsigned int i = 0; i < dict->cell_capacity; i++) {
        unsigned int ix = (cell_ix + i) & (dict->cell_capacity - 1);
        unsigned int cell = dict->cells[ix];
        if (cell == PTRDICT_INVALID_IX) {
            return ix;
        }
        void *key_to_check = dict->keys[cell];
        if (key == key_to_check) {
            *out_found = true;
            return ix;
        }
    }
    return PTRDICT_INVALID_IX;
}

static bool ptrdict_grow_and_rehash(ptrdict_t_ *dict) {
    ptrdict_t_ new_hd;
    bool succeeded = ptrdict_init(&new_hd, dict->cell_capacity * 2);
    if (succeeded == false) {
        return false;
    }
    for (unsigned int i = 0; i < dict->count; i++) {
        void *key = dict->keys[i];
        void *value = dict->values[i];
        succeeded = ptrdict_set_internal(&new_hd, key, value);
        if (succeeded == false) {
            ptrdict_deinit(&new_hd);
            return false;
        }
    }
    ptrdict_deinit(dict);
    *dict = new_hd;
    return true;
}

static bool ptrdict_set_internal(ptrdict_t_ *dict, void *key, void *value) {
    bool found = false;
    unsigned int cell_ix = ptrdict_get_cell_ix(dict, key, &found);
    if (found) {
        unsigned int item_ix = dict->cells[cell_ix];
        dict->values[item_ix] = value;
        return true;
    }
    if (dict->count >= dict->item_capacity) {
        bool succeeded = ptrdict_grow_and_rehash(dict);
        if (succeeded == false) {
            return false;
        }
        cell_ix = ptrdict_get_cell_ix(dict, key, &found);
    }
    dict->cells[cell_ix] = dict->count;
    dict->keys[dict->count] = key;
    dict->values[dict->count] = value;
    dict->cell_ixs[dict->count] = cell_ix;
    dict->count++;
    return true;
}

//-----------------------------------------------------------------------------
// Array
//-----------------------------------------------------------------------------

typedef struct array_ {
    unsigned char *data;
    unsigned int count;
    unsigned int capacity;
    size_t element_size;
    bool lock_capacity;
} array_t_;

static bool array_init_with_capacity(array_t_ *arr, unsigned int capacity, size_t element_size);
static void array_deinit(array_t_ *arr);

array_t_* array_make_(size_t element_size) {
    return array_make_with_capacity(0, element_size);
}

array_t_* array_make_with_capacity(unsigned int capacity, size_t element_size) {
    array_t_ *arr = malloc(sizeof(array_t_));
    if (arr == NULL) {
        return NULL;
    }
    bool succeeded = array_init_with_capacity(arr, capacity, element_size);
    if (succeeded == false) {
        free(arr);
        return NULL;
    }
    return arr;
}

void array_destroy(array_t_ *arr) {
    if (arr == NULL) {
        return;
    }
    array_deinit(arr);
    free(arr);
}

bool array_add(array_t_ *arr, const void *value) {
    if (arr->count >= arr->capacity) {
        assert(!arr->lock_capacity);
        if (arr->lock_capacity) {
            return false;
        }
        unsigned int new_capacity = arr->capacity > 0 ? arr->capacity * 2 : 1;
        unsigned char *new_data = malloc(new_capacity * arr->element_size);
        if (new_data == NULL) {
            return false;
        }
        memcpy(new_data, arr->data, arr->count * arr->element_size);
        free(arr->data);
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    if (value) {
        memcpy(arr->data + (arr->count * arr->element_size), value, arr->element_size);
    }
    arr->count++;
    return true;
}

bool array_addn(array_t_ *arr, const void *values, unsigned int n) {
    for (int i = 0; i < n; i++) {
        unsigned char *value = NULL;
        if (values) {
            value = (unsigned char*)values + (i * arr->element_size);
        }
        bool ok = array_add(arr, value);
        if (!ok) {
            return false;
        }
    }
    return true;
}

bool array_add_array(array_t_ *dest, const array_t_ *source) {
    assert(dest->element_size == source->element_size);
    if (dest->element_size != source->element_size) {
        return false;
    }
    for (int i = 0; i < array_count(source); i++) {
        void *item = array_get(source, i);
        bool ok = array_add(dest, item);
        if (!ok) {
            return false;
        }
    }
    return true;
}

bool array_push(array_t_ *arr, const void *value) {
    return array_add(arr, value);
}

bool array_pop(array_t_ *arr, void *out_value) {
    if (arr->count <= 0) {
        return false;
    }
    if (out_value) {
        void *res = array_get(arr, arr->count - 1);
        memcpy(out_value, res, arr->element_size);
    }
    array_remove(arr, arr->count - 1);
    return true;
}

bool array_set(array_t_ *arr, unsigned int ix, void *value) {
    if (ix >= arr->count) {
        assert(false);
        return false;
    }
    size_t offset = ix * arr->element_size;
    memmove(arr->data + offset, value, arr->element_size);
    return true;
}

bool array_setn(array_t_ *arr, unsigned int ix, void *values, unsigned int n) {
    for (int i = 0; i < n; i++) {
        unsigned int dest_ix = ix + i;
        unsigned char *value = (unsigned char*)values + (i * arr->element_size);
        if (dest_ix < array_count(arr)) {
            bool ok = array_set(arr, dest_ix, value);
            if (!ok) {
                return false;
            }
        } else {
            bool ok = array_add(arr, value);
            if (!ok) {
                return false;
            }
        }
    }
    return true;
}

void * array_get(const array_t_ *arr, unsigned int ix) {
    if (ix >= arr->count) {
        assert(false);
        return NULL;
    }
    size_t offset = ix * arr->element_size;
    return arr->data + offset;
}

void * array_get_last(const array_t_ *arr) {
    if (arr->count <= 0) {
        return NULL;
    }
    return array_get(arr, arr->count - 1);
}

unsigned int array_count(const array_t_ *arr) {
    if (!arr) {
        return 0;
    }
    return arr->count;
}

bool array_remove(array_t_ *arr, unsigned int ix) {
    if (ix >= arr->count) {
        return false;
    }
    if (ix == (arr->count - 1)) {
        arr->count--;
        return true;
    }
    size_t to_move_bytes = (arr->count - 1 - ix) * arr->element_size;
    void *dest = arr->data + (ix * arr->element_size);
    void *src = arr->data + ((ix + 1) * arr->element_size);
    memmove(dest, src, to_move_bytes);
    arr->count--;
    return true;
}

void array_clear(array_t_ *arr) {
    arr->count = 0;
}

void array_lock_capacity(array_t_ *arr) {
    arr->lock_capacity = true;
}

int array_get_index(const array_t_ *arr, void *ptr) {
    for (int i = 0; i < array_count(arr); i++) {
        if (array_get(arr, i) == ptr) {
            return i;
        }
    }
    return -1;
}

void* array_data(array_t_ *arr) {
    return arr->data;
}

const void*  array_const_data(const array_t_ *arr) {
    return arr->data;
}

bool array_orphan_data(array_t_ *arr) {
    return array_init_with_capacity(arr, 0, arr->element_size);
}

static bool array_init_with_capacity(array_t_ *arr, unsigned int capacity, size_t element_size) {
    arr->data = malloc(capacity * element_size);
    if (arr->data == NULL) {
        return false;
    }
    arr->capacity = capacity;
    arr->count = 0;
    arr->element_size = element_size;
    arr->lock_capacity = false;
    return true;
}

static void array_deinit(array_t_ *arr) {
    free(arr->data);
}

//-----------------------------------------------------------------------------
// Pointer Array
//-----------------------------------------------------------------------------

typedef struct ptrarray_ {
    array_t_ arr;
} ptrarray_t_;

ptrarray_t_* ptrarray_make(void) {
    return ptrarray_make_with_capacity(0);
}

ptrarray_t_* ptrarray_make_with_capacity(unsigned int capacity) {
    ptrarray_t_ *ptrarr = malloc(sizeof(ptrarray_t_));
    if (ptrarr == NULL) {
        return NULL;
    }
    bool succeeded = array_init_with_capacity(&ptrarr->arr, capacity, sizeof(void*));
    if (succeeded == false) {
        free(ptrarr);
        return NULL;
    }
    return ptrarr;
}

void ptrarray_destroy(ptrarray_t_ *arr) {
    if (arr == NULL) {
        return;
    }
    array_deinit(&arr->arr);
    free(arr);
}

void ptrarray_destroy_with_items_(ptrarray_t_ *arr, ptrarray_item_destroy_fn destroy_fn){
    if (arr == NULL) {
        return;
    }

    if (destroy_fn) {
        for (int i = 0; i < ptrarray_count(arr); i++) {
            void *item = ptrarray_get(arr, i);
            destroy_fn(item);
        }
    }
    
    ptrarray_destroy(arr);
}

bool ptrarray_add(ptrarray_t_ *arr, void *ptr) {
    return array_add(&arr->arr, &ptr);
}

bool ptrarray_set(ptrarray_t_ *arr, unsigned int ix, void *ptr) {
    return array_set(&arr->arr, ix, &ptr);
}

bool ptrarray_add_array(ptrarray_t_ *dest, const ptrarray_t_ *source) {
    return array_add_array(&dest->arr, &source->arr);
}

void * ptrarray_get(const ptrarray_t_ *arr, unsigned int ix) {
    void* res = array_get(&arr->arr, ix);
    if (res == NULL) {
        return NULL;
    }
    return *(void**)res;
}

bool ptrarray_push(ptrarray_t_ *arr, void *ptr) {
    return ptrarray_add(arr, ptr);
}

void *ptrarray_pop(ptrarray_t_ *arr) {
    int ix = ptrarray_count(arr) - 1;
    if (ix < 0) {
        return NULL;
    }
    void *res = ptrarray_get(arr, ix);
    ptrarray_remove(arr, ix);
    return res;
}

void *ptrarray_top(ptrarray_t_ *arr) {
    int count = ptrarray_count(arr);
    if (count == 0) {
        return NULL;
    }
    return ptrarray_get(arr, count - 1);
}

unsigned int ptrarray_count(const ptrarray_t_ *arr) {
    if (!arr) {
        return 0;
    }
    return array_count(&arr->arr);
}

bool ptrarray_remove(ptrarray_t_ *arr, unsigned int ix) {
    return array_remove(&arr->arr, ix);
}

bool ptrarray_remove_item(ptrarray_t_ *arr, void *item) {
    for (int i = 0; i < ptrarray_count(arr); i++) {
        if (item == ptrarray_get(arr, i)) {
            ptrarray_remove(arr, i);
            return true;
        }
    }
    assert(false);
    return false;
}

void ptrarray_clear(ptrarray_t_ *arr) {
    array_clear(&arr->arr);
}

void ptrarray_lock_capacity(ptrarray_t_ *arr) {
    array_lock_capacity(&arr->arr);
}

int ptrarray_get_index(const ptrarray_t_ *arr, void *ptr) {
    for (int i = 0; i < ptrarray_count(arr); i++) {
        if (ptrarray_get(arr, i) == ptr) {
            return i;
        }
    }
    return -1;
}

void * ptrarray_get_addr(ptrarray_t_ *arr, unsigned int ix) {
    void* res = array_get(&arr->arr, ix);
    if (res == NULL) {
        return NULL;
    }
    return res;
}

void* ptrarray_data(ptrarray_t_ *arr) {
    return array_data(&arr->arr);
}

void ptrarray_reverse(ptrarray_t_ *arr) {
    int count = ptrarray_count(arr);
    if (count < 2) {
        return;
    }

    for (int a_ix = 0; a_ix < (count / 2); a_ix++) {
        int b_ix = count - a_ix - 1;
        void *a = ptrarray_get(arr, a_ix);
        void *b = ptrarray_get(arr, b_ix);
        ptrarray_set(arr, a_ix, b);
        ptrarray_set(arr, b_ix, a);
    }
}

//-----------------------------------------------------------------------------
// String buffer
//-----------------------------------------------------------------------------

typedef struct strbuf {
    array_t_ arr;
} strbuf_t;

strbuf_t* strbuf_make(void) {
    strbuf_t *res = strbuf_make_with_capacity(1);
    return res;
}

strbuf_t* strbuf_make_with_capacity(unsigned int capacity) {
    strbuf_t *buf = malloc(sizeof(strbuf_t));
    if (buf == NULL) {
        return NULL;
    }
    bool succeeded = array_init_with_capacity(&buf->arr, capacity, sizeof(char));
    if (succeeded == false) {
        free(buf);
        return NULL;
    }
    char nul = '\0';
    array_add(&buf->arr, &nul);
    return buf;
}

void strbuf_destroy(strbuf_t *buf) {
    if (buf == NULL) {
        return;
    }
    array_deinit(&buf->arr);
    free(buf);
}

void strbuf_clear(strbuf_t *buf) {
    array_clear(&buf->arr);
    strbuf_append(buf, "");
}

bool strbuf_append(strbuf_t *buf, const char *str) {
    size_t len = array_count(&buf->arr);
    int str_len = 0;
    char c = str[str_len];
    while (c != '\0') {
        if (str_len == 0 && len > 0) {
            array_set(&buf->arr, (unsigned int)len - 1, &c);
        } else {
            array_add(&buf->arr, &c);
        }

        str_len++;
        c = str[str_len];
    }
    if (str_len > 0) {
        char nul = '\0';
        array_add(&buf->arr, &nul);
    }
    return true;
}

bool strbuf_appendf(strbuf_t *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int to_write = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    va_start(args, fmt);
    char *res = (char*)malloc(to_write + 1);
    int written = vsprintf(res, fmt, args);
    assert(written == to_write);
    va_end(args);
    bool ok = strbuf_append(buf, res);
    free(res);
    return ok;
}

const char * strbuf_get_string(strbuf_t *buf) {
    return array_data(&buf->arr);
}

const char * strbuf_get_string_and_destroy(strbuf_t *buf) {
    const char *res = array_data(&buf->arr);
    array_orphan_data(&buf->arr);
    strbuf_destroy(buf);
    return res;
}
