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


/* To update:
 curl https://raw.githubusercontent.com/kgabis/cutils/master/collections.h > collections.h
 curl https://raw.githubusercontent.com/kgabis/cutils/master/collections.c > collections.c
 */

#ifndef collections_h
#define collections_h

#include <stdbool.h>
#include <stddef.h>

//-----------------------------------------------------------------------------
// Dictionary
//-----------------------------------------------------------------------------

typedef struct dict_ dict_t_;

#define dict(TYPE) dict_t_

dict_t_*     dict_make(void);
void         dict_destroy(dict_t_ *dict);
bool         dict_set(dict_t_ *dict, const char *key, void *value);
void *       dict_get(const dict_t_ *dict, const char *key);
void *       dict_get_value_at(const dict_t_ *dict, unsigned int ix);
const char * dict_get_key_at(const dict_t_ *dict, unsigned int ix);
unsigned int dict_count(const dict_t_ *dict);
bool         dict_remove(dict_t_ *dict, const char *key);
void         dict_clear(dict_t_ *dict);

//-----------------------------------------------------------------------------
// Pointer dictionary
//-----------------------------------------------------------------------------

typedef struct ptrdict_ ptrdict_t_;

#define ptrdict(KEY_TYPE, VALUE_TYPE) ptrdict_t_

ptrdict_t_*  ptrdict_make(void);
void         ptrdict_destroy(ptrdict_t_ *dict);
bool         ptrdict_set(ptrdict_t_ *dict, void *key, void *value);
void *       ptrdict_get(const ptrdict_t_ *dict, void *key);
void *       ptrdict_get_value_at(const ptrdict_t_ *dict, unsigned int ix);
void *       ptrdict_get_key_at(const ptrdict_t_ *dict, unsigned int ix);
unsigned int ptrdict_count(const ptrdict_t_ *dict);
bool         ptrdict_remove(ptrdict_t_ *dict, void *key);
void         ptrdict_clear(ptrdict_t_ *dict);

//-----------------------------------------------------------------------------
// Array
//-----------------------------------------------------------------------------

typedef struct array_ array_t_;

#define array(TYPE) array_t_

#define array_make(type) array_make_(sizeof(type))
array_t_*    array_make_(size_t element_size);
array_t_*    array_make_with_capacity(unsigned int capacity, size_t element_size);
void         array_destroy(array_t_ *arr);
bool         array_add(array_t_ *arr, const void *value);
bool         array_addn(array_t_ *arr, const void *values, unsigned int n);
bool         array_add_array(array_t_ *dest, const array_t_ *source);
bool         array_push(array_t_ *arr, const void *value);
bool         array_pop(array_t_ *arr, void *out_value);
bool         array_set(array_t_ *arr, unsigned int ix, void *value);
bool         array_setn(array_t_ *arr, unsigned int ix, void *values, unsigned int n);
void *       array_get(const array_t_ *arr, unsigned int ix);
void *       array_get_last(const array_t_ *arr);
unsigned int array_count(const array_t_ *arr);
bool         array_remove(array_t_ *arr, unsigned int ix);
void         array_clear(array_t_ *arr);
void         array_lock_capacity(array_t_ *arr);
int          array_get_index(const array_t_ *arr, void *ptr);
void*        array_data(array_t_ *arr);
const void*  array_const_data(const array_t_ *arr);
bool         array_orphan_data(array_t_ *arr);

//-----------------------------------------------------------------------------
// Pointer Array
//-----------------------------------------------------------------------------

typedef void (*ptrarray_item_destroy_fn)(void* item);

typedef struct ptrarray_ ptrarray_t_;

#define ptrarray(TYPE) ptrarray_t_
#define ptrarray_destroy_with_items(arr, fn) ptrarray_destroy_with_items_(arr, (ptrarray_item_destroy_fn)(fn))

ptrarray_t_* ptrarray_make(void);
ptrarray_t_* ptrarray_make_with_capacity(unsigned int capacity);
void         ptrarray_destroy(ptrarray_t_ *arr);
void         ptrarray_destroy_with_items_(ptrarray_t_ *arr, ptrarray_item_destroy_fn destroy_fn);
bool         ptrarray_add(ptrarray_t_ *arr, void *ptr);
bool         ptrarray_set(ptrarray_t_ *arr, unsigned int ix, void *ptr);
bool         ptrarray_add_array(ptrarray_t_ *dest, const ptrarray_t_ *source);
void *       ptrarray_get(const ptrarray_t_ *arr, unsigned int ix);
bool         ptrarray_push(ptrarray_t_ *arr, void *ptr);
void *       ptrarray_pop(ptrarray_t_ *arr);
void *       ptrarray_top(ptrarray_t_ *arr);
unsigned int ptrarray_count(const ptrarray_t_ *arr);
bool         ptrarray_remove(ptrarray_t_ *arr, unsigned int ix);
bool         ptrarray_remove_item(ptrarray_t_ *arr, void *item);
void         ptrarray_clear(ptrarray_t_ *arr);
void         ptrarray_lock_capacity(ptrarray_t_ *arr);
int          ptrarray_get_index(const ptrarray_t_ *arr, void *ptr);
void *       ptrarray_get_addr(ptrarray_t_ *arr, unsigned int ix);
void*        ptrarray_data(ptrarray_t_ *arr);
void         ptrarray_reverse(ptrarray_t_ *arr);

//-----------------------------------------------------------------------------
// String buffer
//-----------------------------------------------------------------------------

typedef struct strbuf strbuf_t;

strbuf_t* strbuf_make(void);
strbuf_t* strbuf_make_with_capacity(unsigned int capacity);
void strbuf_destroy(strbuf_t *buf);
void strbuf_clear(strbuf_t *buf);
bool strbuf_append(strbuf_t *buf, const char *str);
bool strbuf_appendf(strbuf_t *buf, const char *fmt, ...)  __attribute__((format(printf, 2, 3)));;
const char * strbuf_get_string(strbuf_t *buf);
const char * strbuf_get_string_and_destroy(strbuf_t *buf);

#endif /* collections_h */
