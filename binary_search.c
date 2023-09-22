#include "binary_search.h"
#include "array.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* seaches for matching prefix between sorted_array[lo] and sorted_array[hi] */
int
binary_search(array_t *record_array, int lo, int hi, char *prefix, int *locn,
              int *s, int *c) {
    // empty search range, so return not found
    if (lo >= hi) {
        return NOT_FOUND;
    }

    int mid, outcome;
    mid = (lo + hi) / 2;
    outcome = cmp(prefix, record_array->records[mid].trading_name);

    if (outcome < 0) {
        *s += 1;
        // key is smaller than mid
        return binary_search(record_array, lo, mid, prefix, locn, s, c);
    } else if (outcome > 0) {
        *s += 1;
        // key larger than mid
        return binary_search(record_array, mid + 1, hi, prefix, locn, s, c);
    } else {
        *s += 1;
        // key == mid so found location
        *locn = mid;
        return FOUND;
    }
}

/* compare the first n characters between the key and record, n = len(key) */
int
cmp(char *s1, char *s2) {
    return strcmp(s1, s2);
}
