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

#include "tests_collections.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../collections.h"

static void dict_tests(void);
static void ptrdict_tests(void);
static void array_tests(void);
static void ptrarray_tests(void);

void collections_tests() {
    dict_tests();
    ptrdict_tests();
    array_tests();
    ptrarray_tests();
}

static void dict_tests() {
    puts("Running dict tests:");
    bool succeeded = false;
    dict(char) *dict = dict_make();
    int count = 128;
    for (int i = 0; i < count; i++) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%d", i);
        char *buf_cpy = strdup(buf);
        succeeded = dict_set(dict, buf, buf_cpy);
        assert(succeeded);
    }
    for (int i = 0; i < count; i++) {
        const char *key = dict_get_key_at(dict, (unsigned int)i);
        char *val = dict_get(dict, key);
        assert(strcmp(key, val) == 0);
    }
    puts("dict tests: ok");
}

static void ptrdict_tests(void) {
    puts("Running ptrdict tests:");
    bool succeeded = false;
    ptrdict(int, char) *dict = ptrdict_make();
    int count = 128;
    for (int i = 0; i < count; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        char buf[128];
        snprintf(buf, sizeof(buf), "%d", i);
        char *buf_cpy = strdup(buf);
        succeeded = ptrdict_set(dict, val, buf_cpy);
        assert(succeeded);
    }
    for (int i = 0; i < count; i++) {
        int *key = ptrdict_get_key_at(dict, i);
        char *val = ptrdict_get(dict, key);
        int val_int = atoi(val);
        assert(*key == val_int);
    }
    puts("ptrdict tests: ok");
}

static void array_tests() {
    puts("Running array tests:");
    int c = 1024;
    array(int) *int_arr = array_make(sizeof(int));
    for (int i = 0; i < c; i++) {
        array_add(int_arr, &i);
    }
    for (int i = 0; i < c; i++) {
        int *x = array_get(int_arr, (unsigned int)i);
        assert(*x == i);
    }
    puts("array tests: ok");

}

static void ptrarray_tests() {
    puts("Running ptrarray tests:");
    int c = 1024;
    ptrarray(int) *int_arr = ptrarray_make();
    for (int i = 0; i < c; i++) {
        int *el = malloc(sizeof(int));
        *el = i;
        ptrarray_add(int_arr, el);
    }
    for (int i = 0; i < c; i++) {
        int *x = ptrarray_get(int_arr, (unsigned int)i);
        assert(*x == i);
    }
    puts("ptrarray tests: ok");
}
