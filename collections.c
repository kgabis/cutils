/*
    Copyright (c) 2017 Krzysztof Gabis
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

//-----------------------------------------------------------------------------
// Dictionary
//-----------------------------------------------------------------------------

#define DICT_INVALID_IX UINT_MAX

typedef struct dict {
    unsigned int *cells;
    unsigned long *hashes;
    char **keys;
    void **values;
    unsigned int *cell_ixs;
    unsigned int count;
    unsigned int item_capacity;
    unsigned int cell_capacity;
} dict_t;

// Private declarations
static bool dict_init(dict_t *hd, unsigned int initial_capacity);
static void dict_deinit(dict_t *hd, bool free_keys);
static unsigned int dict_get_cell_ix(const dict_t *hd,
                                         const char *key,
                                         unsigned long hash,
                                         bool *out_found);
static unsigned long hash_string(const char *str);
static bool dict_grow_and_rehash(dict_t *hd);
static bool dict_set_internal(dict_t *hd, const char *ckey, char *mkey, void *value);

// Public
dict_t* dict_make(void) {
    dict_t *dict = malloc(sizeof(dict_t));
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

void dict_destroy(dict_t *ht) {
    dict_deinit(ht, true);
    free(ht);
}

bool dict_set(dict_t *hd, const char *key, void *value) {
    return dict_set_internal(hd, key, NULL, value);
}

void *dict_get(const dict_t *hd, const char *key) {
    unsigned long hash = hash_string(key);
    bool found = false;
    unsigned long cell_ix = dict_get_cell_ix(hd, key, hash, &found);
    if (found == false) {
        return NULL;
    }
    unsigned int item_ix = hd->cells[cell_ix];
    return hd->values[item_ix];
}

void *dict_get_value_at(const dict_t *hd, unsigned int ix) {
    if (ix >= hd->count) {
        return NULL;
    }
    return hd->values[ix];
}

const char *dict_get_key_at(const dict_t *hd, unsigned int ix) {
    if (ix >= hd->count) {
        return NULL;
    }
    return hd->keys[ix];
}

unsigned int dict_count(const dict_t *hd) {
    return hd->count;
}

bool dict_remove(dict_t *hd, const char *key) {
    unsigned long hash = hash_string(key);
    bool found = false;
    unsigned int cell = dict_get_cell_ix(hd, key, hash, &found);
    if (!found) {
        return false;
    }

    unsigned int item_ix = hd->cells[cell];
    free(hd->keys[item_ix]);
    unsigned int last_item_ix = hd->count - 1;
    if (item_ix < last_item_ix) {
        hd->keys[item_ix] = hd->keys[last_item_ix];
        hd->values[item_ix] = hd->values[last_item_ix];
        hd->cell_ixs[item_ix] = hd->cell_ixs[last_item_ix];
        hd->hashes[item_ix] = hd->hashes[last_item_ix];
        hd->cells[hd->cell_ixs[item_ix]] = item_ix;
    }
    hd->count--;

    unsigned int i = cell;
    unsigned int j = i;
    for (unsigned int x = 0; x < (hd->cell_capacity - 1); x++) {
        j = (j + 1) & (hd->cell_capacity - 1);
        if (hd->cells[j] == DICT_INVALID_IX) {
            break;
        }
        unsigned int k = hd->hashes[hd->cells[j]] & (hd->cell_capacity - 1);
        if ((j > i && (k <= i || k > j))
            || (j < i && (k <= i && k > j))) {
            hd->cell_ixs[hd->cells[j]] = i;
            hd->cells[i] = hd->cells[j];
            i = j;
        }
    }
    hd->cells[i] = DICT_INVALID_IX;
    return true;
}

void dict_clear(dict_t *hd) {
    for (unsigned int i = 0; i < hd->count; i++) {
        free(hd->keys[i]);
    }
    hd->count = 0;
    for (unsigned int i = 0; i < hd->cell_capacity; i++) {
        hd->cells[i] = DICT_INVALID_IX;
    }
}

// Private definitions
static bool dict_init(dict_t *hd, unsigned int initial_capacity) {
    // todo: check initial capacity is a power of 2
    hd->cells = NULL;
    hd->keys = NULL;
    hd->values = NULL;
    hd->cell_ixs = NULL;
    hd->hashes = NULL;

    hd->count = 0;
    hd->cell_capacity = initial_capacity;
    hd->item_capacity = (unsigned int)(initial_capacity * 0.7f);

    hd->cells = malloc(hd->cell_capacity * sizeof(*hd->cells));
    hd->keys = malloc(hd->item_capacity * sizeof(*hd->keys));
    hd->values = malloc(hd->item_capacity * sizeof(*hd->values));
    hd->cell_ixs = malloc(hd->item_capacity * sizeof(*hd->cell_ixs));
    hd->hashes = malloc(hd->item_capacity * sizeof(*hd->hashes));
    if (hd->cells == NULL
        || hd->keys == NULL
        || hd->values == NULL
        || hd->cell_ixs == NULL
        || hd->hashes == NULL) {
        goto error;
    }
    for (unsigned int i = 0; i < hd->cell_capacity; i++) {
        hd->cells[i] = DICT_INVALID_IX;
    }
    return true;
error:
    free(hd->cells);
    free(hd->keys);
    free(hd->values);
    free(hd->cell_ixs);
    free(hd->hashes);
    return false;
}

static void dict_deinit(dict_t *hd, bool free_keys) {
    if (free_keys) {
        for (unsigned int i = 0; i < hd->count; i++) {
            free(hd->keys[i]);
        }
    }
    hd->count = 0;
    hd->item_capacity = 0;
    hd->cell_capacity = 0;

    free(hd->cells);
    free(hd->keys);
    free(hd->values);
    free(hd->cell_ixs);
    free(hd->hashes);

    hd->cells = NULL;
    hd->keys = NULL;
    hd->values = NULL;
    hd->cell_ixs = NULL;
    hd->hashes = NULL;
}

static unsigned int dict_get_cell_ix(const dict_t *hd,
                                         const char *key,
                                         unsigned long hash,
                                         bool *out_found)
{
    *out_found = false;
    unsigned int cell_ix = hash & (hd->cell_capacity - 1);
    for (unsigned int i = 0; i < hd->cell_capacity; i++) {
        unsigned int ix = (cell_ix + i) & (hd->cell_capacity - 1);
        unsigned int cell = hd->cells[ix];
        if (cell == DICT_INVALID_IX) {
            return ix;
        }
        unsigned long hash_to_check = hd->hashes[cell];
        if (hash != hash_to_check) {
            continue;
        }
        const char *key_to_check = hd->keys[cell];
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

static bool dict_grow_and_rehash(dict_t *hd) {
    dict_t new_hd;
    bool succeeded = dict_init(&new_hd, hd->cell_capacity * 2);
    if (succeeded == false) {
        return false;
    }
    for (unsigned int i = 0; i < hd->count; i++) {
        char *key = hd->keys[i];
        void *value = hd->values[i];
        succeeded = dict_set_internal(&new_hd, key, key, value);
        if (succeeded == false) {
            dict_deinit(&new_hd, false);
            return false;
        }
    }
    dict_deinit(hd, false);
    *hd = new_hd;
    return true;
}

static bool dict_set_internal(dict_t *hd, const char *ckey, char *mkey, void *value) {
    unsigned long hash = hash_string(ckey);
    bool found = false;
    unsigned int cell_ix = dict_get_cell_ix(hd, ckey, hash, &found);
    if (found) {
        unsigned int item_ix = hd->cells[cell_ix];
        hd->values[item_ix] = value;
        return true;
    }
    if (hd->count >= hd->item_capacity) {
        bool succeeded = dict_grow_and_rehash(hd);
        if (succeeded == false) {
            return false;
        }
        cell_ix = dict_get_cell_ix(hd, ckey, hash, &found);
    }
    hd->cells[cell_ix] = hd->count;
    hd->keys[hd->count] = mkey != NULL ? mkey : strdup(ckey);
    hd->values[hd->count] = value;
    hd->cell_ixs[hd->count] = cell_ix;
    hd->hashes[hd->count] = hash;
    hd->count++;
    return true;
}

//-----------------------------------------------------------------------------
// Array
//-----------------------------------------------------------------------------

typedef struct array {
    unsigned char *data;
    unsigned int count;
    unsigned int capacity;
    size_t element_size;
} array_t;

static bool array_init_with_capacity(array_t *arr, unsigned int capacity, size_t element_size);
static void array_deinit(array_t *arr);

array_t* array_make(size_t element_size) {
    return array_make_with_capacity(0, element_size);
}

array_t* array_make_with_capacity(unsigned int capacity, size_t element_size) {
    array_t *arr = malloc(sizeof(array_t));
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

void array_destroy(array_t *arr) {
    array_deinit(arr);
    free(arr);
}

bool array_add(array_t *arr, void *value) {
    if ((arr->count + 1) >= arr->capacity) {
        unsigned int new_capacity = arr->capacity > 0 ? arr->capacity * 2 : 1;
        unsigned char *new_data = malloc(new_capacity * arr->element_size);
        if (new_data == NULL) {
            return NULL;
        }
        memcpy(new_data, arr->data, arr->count * arr->element_size);
        free(arr->data);
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    memcpy(arr->data + (arr->count * arr->element_size), value, arr->element_size);
    arr->count++;
    return true;
}

void * array_get(const array_t *arr, unsigned int ix) {
    if (ix >= arr->count) {
        return NULL;
    }
    size_t offset = ix * arr->element_size;
    return arr->data + offset;
}

unsigned int array_count(const array_t *arr) {
    return arr->count;
}

bool array_remove(array_t *arr, unsigned int ix) {
    if (ix >= arr->count) {
        return false;
    }
    size_t to_move_bytes = (arr->count - 1 - ix) * arr->element_size;
    memmove(arr->data + ix, arr->data + ix + 1, to_move_bytes);
    arr->count--;
    return true;
}

void array_clear(array_t *arr) {
    arr->count = 0;
}

static bool array_init_with_capacity(array_t *arr, unsigned int capacity, size_t element_size) {
    arr->data = malloc(capacity * element_size);
    if (arr->data == NULL) {
        return false;
    }
    arr->capacity = capacity;
    arr->count = 0;
    arr->element_size = element_size;
    return true;
}

static void array_deinit(array_t *arr) {
    free(arr->data);
}

//-----------------------------------------------------------------------------
// Pointer Array
//-----------------------------------------------------------------------------

typedef struct ptrarray {
    array_t arr;
} ptrarray_t;

ptrarray_t* ptrarray_make(void) {
    return ptrarray_make_with_capacity(0);
}

ptrarray_t* ptrarray_make_with_capacity(unsigned int capacity) {
    ptrarray_t *ptrarr = malloc(sizeof(ptrarray_t));
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

void ptrarray_destroy(ptrarray_t *arr) {
    array_deinit(&arr->arr);
}

bool ptrarray_add(ptrarray_t *arr, void *ptr) {
    return array_add(&arr->arr, &ptr);
}

void * ptrarray_get(const ptrarray_t *arr, unsigned int ix) {
    void* res = array_get(&arr->arr, ix);
    if (res == NULL) {
        return NULL;
    }
    return *(void**)res;
}

unsigned int ptrarray_count(const ptrarray_t *arr) {
    return array_count(&arr->arr);
}

bool ptrarray_remove(ptrarray_t *arr, unsigned int ix) {
    return array_remove(&arr->arr, ix);
}

void ptrarray_clear(ptrarray_t *arr) {
    array_clear(&arr->arr);
}
