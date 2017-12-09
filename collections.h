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

typedef struct dict dict_t;

dict_t*      dict_make(void);
void         dict_destroy(dict_t *dict);
bool         dict_set(dict_t *dict, const char *key, void *value);
void *       dict_get(const dict_t *dict, const char *key);
void *       dict_get_value_at(const dict_t *dict, unsigned int ix);
const char * dict_get_key_at(const dict_t *dict, unsigned int ix);
unsigned int dict_count(const dict_t *dict);
bool         dict_remove(dict_t *dict, const char *key);
void         dict_clear(dict_t *dict);

//-----------------------------------------------------------------------------
// Array
//-----------------------------------------------------------------------------

typedef struct array array_t;

array_t*     array_make(size_t element_size);
array_t*     array_make_with_capacity(unsigned int capacity, size_t element_size);
void         array_destroy(array_t *arr);
bool         array_add(array_t *arr, void *value);
void *       array_get(const array_t *arr, unsigned int ix);
unsigned int array_count(const array_t *arr);
bool         array_remove(array_t *arr, unsigned int ix);
void         array_clear(array_t *arr);

//-----------------------------------------------------------------------------
// Pointer Array
//-----------------------------------------------------------------------------

typedef struct ptrarray ptrarray_t;

ptrarray_t*     ptrarray_make(void);
ptrarray_t*     ptrarray_make_with_capacity(unsigned int capacity);
void            ptrarray_destroy(ptrarray_t *arr);
bool            ptrarray_add(ptrarray_t *arr, void *ptr);
void *          ptrarray_get(const ptrarray_t *arr, unsigned int ix);
unsigned int    ptrarray_count(const ptrarray_t *arr);
bool            ptrarray_remove(ptrarray_t *arr, unsigned int ix);
void            ptrarray_clear(ptrarray_t *arr);

#endif /* collections_h */
