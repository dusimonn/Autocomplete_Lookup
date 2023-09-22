#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "dataset.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_SIZE 20
#define BITS_CHAR    8

// struct declarations
typedef struct {
    record_t *records;
    int length;
    int size;
} array_t;

// function prototypes

/* encapsulate array autocomplete implementation */
void array_autocomplete(FILE *data_file, FILE *out_file);

/* allocates space and returns an array struct */
array_t *make_array();

/* inserts a record pointer into the array struct */
array_t *array_insert(array_t *record_array, record_t *record);

/* sorts the array using insertion sort */
array_t *array_sort(array_t *record_array);

/* finds the prefix in the array struct using binary search */
int array_find_and_traverse(char *prefix, array_t *record_array, FILE *out_file,
                            int *b, int *c, int *s);

/* frees all allocated memory for the array struct */
void free_array(array_t *record_array);

#endif