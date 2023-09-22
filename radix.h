#ifndef _RADIX_H_
#define _RADIX_H_

#include "dataset.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_SIZE 20
#define BITS_CHAR    8
#define NO_MATCH     0
#define MAYBE_MATCH  -1
#define MATCH        1

// struct declarations
typedef struct radix_node radix_node_t;
struct radix_node {
    int common_bits;
    int *prefix;      // capture prefix as a bit array
    int prefix_len;   // bit array length
    radix_node_t *branch_a;
    radix_node_t *branch_b;
    record_t records[INITIAL_SIZE];
    int num_records;
    int size;
};

// function prototypes

/* encapsulate regix autocomplete implementation */
void regix_autocomplete(FILE *data_file, FILE *out_file);

/* allocates memory and initialises a radix_node */
radix_node_t *create_radix(void);

/* inserts a record into the radix tree */
radix_node_t *insert_radix(radix_node_t *radix_records, record_t *record);

/* updates the properties/attributes of a radix_node */
radix_node_t *set_node_properties(radix_node_t *node, record_t *record,
                                  int common_bits, int *prefix, int prefix_len,
                                  radix_node_t *branch_a,
                                  radix_node_t *branch_b);

/* stores a string as its corresponding bit array */
int *store_bit_array(int num_chars, char *key);

/* finds the matching prefix between 2 bit arrays and returns num of com bits */
int *find_matching_prefix(int *prefix_bit_array, int prefix_bit_len,
                          int *key_bit_array, int key_bit_len,
                          int *common_bits);

/* captues everything but the parent common prefix as a bit array */
int *store_prefix(int comm_bits, int *full_bit_array, int len);

/* converts a bit array into corresponding string representation */
char *bit_to_str(int *bit_array, int bit_len);

/*Returns the nth bit of a char, 0th bit being the least sig bit */
int get_bit(char key, int n);

/* traverses radix tree to find matching bit representation of tree */
void radix_find_and_traverse(int *trading_bit, int len,
                             radix_node_t *radix_records, FILE *out_file,
                             int *b, int *c, int *s);

/* alternative traversal utilising more bit comparison UNABLE TO GET WORKING */
void radix_find_and_traverse_2(int *trading_bit, int len,
                               radix_node_t *radix_records, FILE *out_file,
                               int *b, int *c, int *s);

/* for every node we visit, paste the prefix into an array for comparison */
void paste_prefix(int *bit_traversal, int bit_len, int *prefix, int prefix_len,
                  int *index, int *total_len);

/* determines whether there is a match between 2 bit arrays */
int is_match(int *trading_bit, int trading_len, int *bit_traversal,
             int traversal_len, int *b);

/* frees any memory allocated to radix tree */
void free_radix(radix_node_t *radix_records);

/* prints all trading names in a radix tree */
void print_all_leaf(radix_node_t *radix_records);

#endif