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

typedef struct _dict _dict_t;

#define dict(TYPE) _dict_t

_dict_t*     dict_make(void);
void         dict_destroy(_dict_t *dict);
bool         dict_set(_dict_t *dict, const char *key, void *value);
void *       dict_get(const _dict_t *dict, const char *key);
void *       dict_get_value_at(const _dict_t *dict, unsigned int ix);
const char * dict_get_key_at(const _dict_t *dict, unsigned int ix);
unsigned int dict_count(const _dict_t *dict);
bool         dict_remove(_dict_t *dict, const char *key);
void         dict_clear(_dict_t *dict);

//-----------------------------------------------------------------------------
// Pointer dictionary
//-----------------------------------------------------------------------------

typedef struct _ptrdict _ptrdict_t;

#define ptrdict(KEY_TYPE, VALUE_TYPE) _ptrdict_t

_ptrdict_t*  ptrdict_make(void);
void         ptrdict_destroy(_ptrdict_t *dict);
bool         ptrdict_set(_ptrdict_t *dict, void *key, void *value);
void *       ptrdict_get(const _ptrdict_t *dict, void *key);
void *       ptrdict_get_value_at(const _ptrdict_t *dict, unsigned int ix);
void *       ptrdict_get_key_at(const _ptrdict_t *dict, unsigned int ix);
unsigned int ptrdict_count(const _ptrdict_t *dict);
bool         ptrdict_remove(_ptrdict_t *dict, void *key);
void         ptrdict_clear(_ptrdict_t *dict);

//-----------------------------------------------------------------------------
// Array
//-----------------------------------------------------------------------------

typedef struct _array _array_t;

#define array(TYPE) _array_t

_array_t*    array_make(size_t element_size);
_array_t*    array_make_with_capacity(unsigned int capacity, size_t element_size);
void         array_destroy(_array_t *arr);
bool         array_add(_array_t *arr, void *value);
void *       array_get(const _array_t *arr, unsigned int ix);
unsigned int array_count(const _array_t *arr);
bool         array_remove(_array_t *arr, unsigned int ix);
void         array_clear(_array_t *arr);
void         array_lock_capacity(_array_t *arr);
int          array_get_index(const _array_t *arr, void *ptr);

//-----------------------------------------------------------------------------
// Pointer Array
//-----------------------------------------------------------------------------

typedef struct _ptrarray _ptrarray_t;

#define ptrarray(TYPE) _ptrarray_t

_ptrarray_t* ptrarray_make(void);
_ptrarray_t* ptrarray_make_with_capacity(unsigned int capacity);
void         ptrarray_destroy(_ptrarray_t *arr);
bool         ptrarray_add(_ptrarray_t *arr, void *ptr);
void *       ptrarray_get(const _ptrarray_t *arr, unsigned int ix);
unsigned int ptrarray_count(const _ptrarray_t *arr);
bool         ptrarray_remove(_ptrarray_t *arr, unsigned int ix);
bool         ptrarray_remove_item(_ptrarray_t *arr, void *item);
void         ptrarray_clear(_ptrarray_t *arr);
void         ptrarray_lock_capacity(_ptrarray_t *arr);
int          ptrarray_get_index(const _ptrarray_t *arr, void *ptr);
void *       ptrarray_get_addr(_ptrarray_t *arr, unsigned int ix);

#endif /* collections_h */
