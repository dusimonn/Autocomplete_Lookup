#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "binary_search.h"

// function definitions
/* creates an empty array to store dataset records */
array_t *
make_array() {
    array_t *record_array = (array_t *) malloc(sizeof(*record_array));
    assert(record_array != NULL);
    record_array->length = 0;
    record_array->size = INITIAL_SIZE;
    /* initiate the array to hold 20 records */
    record_array->records =
        (record_t *) malloc(record_array->size * sizeof(record_t));
    return record_array;
}

/* inserts a record from dataset into the array */
array_t *
array_insert(array_t *record_array, record_t *record) {
    assert(record_array != NULL);
    /* the array is currently empty, this is the first isnertion */
    if (record_array->length == 0) {
        record_array->records[0] = *record;
        record_array->length++;
    } else {
        /* check if there is enough space in the array, realloc otherwise */
        int curr_size = record_array->size;
        if (record_array->length >= curr_size - 1) {
            record_array->size *= 2;
            record_array->records = (record_t *) realloc(
                record_array->records, sizeof(record_t) * record_array->size);
            assert(record_array->records != NULL);

            /* initialise the new space */
            record_t new_record = (record_t){0};
            for (int i = record_array->length; i < record_array->size; i++) {
                record_array->records[i] = new_record;
            }
        }
        /* now insert the record */
        record_array->records[record_array->length] = *record;
        record_array->length++;
    }

    return record_array;
}

/* sorts the array on trading name attribute using insertion sort */
array_t *
array_sort(array_t *record_array) {
    for (int i = 0; i < record_array->length; i++) {
        for (int j = i - 1;
             j >= 0 && strcmp(record_array->records[j].trading_name,
                              record_array->records[j + 1].trading_name) > 0;
             j--) {
            record_t temp = record_array->records[j];
            record_array->records[j] = record_array->records[j + 1];
            record_array->records[j + 1] = temp;
        }
    }
    return record_array;
}

/* finds the prefix in the array and keeps counts of any comparisons */
int
array_find_and_traverse(char *prefix, array_t *record_array, FILE *out_file,
                        int *b, int *c, int *s) {
    int locn;
    int result = binary_search(record_array, 0, record_array->length, prefix,
                               &locn, s, c);
    *c = strlen(prefix) + 1;

    if (result == FOUND) {
        if (locn > 0) {
            /* traverse backwards to first instance of match */
            while (locn >= 0 &&
                   cmp(prefix, record_array->records[locn].trading_name) == 0) {
                *s += 1;
                locn--;
            }
            locn++;
            /* traverse forwards and output all matches */
            while (locn < record_array->length &&
                   cmp(prefix, record_array->records[locn].trading_name) == 0) {
                *s += 1;
                print_to_outfile(out_file, record_array->records[locn]);
                locn++;
            }
        } else {
            print_to_outfile(out_file, record_array->records[locn]);
        }
    }

    *b = *c * BITS_CHAR;
    return result;
}

/* frees any memory allocated to the array */
void
free_array(array_t *record_array) {
    free(record_array->records);
    free(record_array);
}

/* encapsulate array autocomplete implementation */
void
array_autocomplete(FILE *data_file, FILE *out_file) {
    /* create sorted array to store records */
    array_t *record_array = make_array();

    /* processing dataset */
    while (!feof(data_file)) {
        /* read each record */
        record_t *record = read_record(data_file);
        /* insert the record into the array */
        record_array = array_insert(record_array, record);
        free(record);
    }

    /* sort the array */
    record_array = array_sort(record_array);
    int b_ave = 0;
    int c_ave = 0;
    int s_ave = 0;
    double num_keys = 0;
    /* processing partial trading_names from stdin */
    while (!feof(stdin)) {
        int b = 0, c = 0, s = 0;
        char trading_name[MAX_FIELD];
        fscanf(stdin, "%[^\n]\n", trading_name);

        /* output the corresponding prefix to the outfile */
        fprintf(out_file, "%s\n", trading_name);
        /* find matching records in sorted array and output to outfile */
        array_find_and_traverse(trading_name, record_array, out_file, &b, &c,
                                &s);

        /* output number of comparisons to stdout */
        // fprintf(stdout, "%s --> b%d c%d s%d\n", trading_name, b, c, s);
        b_ave += b;
        c_ave += c;
        s_ave += s;
        num_keys++;
    }
    printf("%.2lf %.2lf %.2lf\n", b_ave / num_keys, c_ave / num_keys,
           s_ave / num_keys);

    free_array(record_array);
}