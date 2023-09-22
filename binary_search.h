#ifndef _BINARY_SEARCH_H_
#define _BINARY_SEARCH_H_

#include "array.h"
#include "dataset.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOT_FOUND 0
#define FOUND     1

/* binary search the array for a matching prefix between index lo and hi + 1 */
int binary_search(array_t *record_array, int lo, int hi, char *prefix,
                  int *locn, int *s, int *c);

/* string comparison for binary search */
int cmp(char *s1, char *s2);

#endif