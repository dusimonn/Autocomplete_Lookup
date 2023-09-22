#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary_search.h"
#include "radix.h"

// function definitions
/* creates an empty radix tree */
radix_node_t *
create_radix() {
    radix_node_t *radix_records =
        (radix_node_t *) malloc(sizeof(*radix_records));
    assert(radix_records != NULL);
    radix_records->branch_a = NULL;
    radix_records->branch_b = NULL;
    radix_records->common_bits = 0;
    radix_records->prefix = NULL;
    radix_records->prefix_len = 0;
    radix_records->num_records = 0;
    radix_records->size = INITIAL_SIZE;
    return radix_records;
}

/* inserts a record into the radix tree */
radix_node_t *
insert_radix(radix_node_t *radix_records, record_t *record) {

    /* first insertion into tree */
    if (radix_records == NULL) {
        radix_records = create_radix();
        int len = strlen(record->trading_name) + 1;
        radix_records = set_node_properties(
            radix_records, record, len * BITS_CHAR, NULL, len, NULL, NULL);
        return radix_records;
    }

    /* represent trading name as array of bits */
    radix_node_t *curr = radix_records;
    radix_node_t *prev = radix_records;
    radix_node_t *other = NULL;
    char *key = record->trading_name;
    int num_chars = strlen(key) + 1;
    int *key_bit_array = store_bit_array(num_chars, key);
    int comm_bits;
    int next_bit;

    /* traverse tree to find loc to insert */
    while (curr != NULL) {
        /* find matching prefix between prefix at curr node and record */
        int prefix_len = curr->prefix_len;

        int *com_bit_array =
            find_matching_prefix(curr->prefix, prefix_len, key_bit_array,
                                 num_chars * BITS_CHAR, &comm_bits);

        /* we have matching prefix, now determine what the next bit is*/

        /* if no next bit, then just store the substring */
        if (comm_bits == num_chars * BITS_CHAR) {
            free(com_bit_array);
            break;
        } else {
            next_bit = key_bit_array[comm_bits];
        }

        /* if next bit is 1, go branch b*/
        if (next_bit == 1) {
            prev = curr;
            curr = curr->branch_b;
        }
        /* if next bit is 0, go branch a*/
        else if (next_bit == 0) {
            prev = curr;
            curr = curr->branch_a;
        }
        free(com_bit_array);
    }

    /* found location, now update the radix tree */

    /* special case: storing substring */
    if (comm_bits == num_chars * BITS_CHAR) {
        prev->records[prev->num_records] = *record;
        prev->num_records += 1;
    }
    /* normal case */
    else {
        /* update the child node */
        int prefix_len = num_chars * BITS_CHAR - comm_bits;
        int *curr_prefix =
            store_prefix(comm_bits, key_bit_array, num_chars * BITS_CHAR);
        curr = set_node_properties(create_radix(), record, prefix_len,
                                   curr_prefix, prefix_len, NULL, NULL);

        /* update the other branch */
        int *other_prefix =
            store_prefix(comm_bits, prev->prefix, prev->prefix_len);
        record_t *parent_record = &prev->records[prev->num_records - 1];
        other = set_node_properties(create_radix(), parent_record,
                                    prev->prefix_len - comm_bits, other_prefix,
                                    prev->prefix_len - comm_bits, NULL, NULL);

        /* update the parent node 'prev'*/
        if (next_bit == 0) {
            prev = set_node_properties(prev, NULL, comm_bits, NULL, comm_bits,
                                       curr, other);
        } else {
            prev = set_node_properties(prev, NULL, comm_bits, NULL, comm_bits,
                                       other, curr);
        }
    }
    free(key_bit_array);

    return radix_records;
}

/* updates the properties/attributes of a radix_node */
radix_node_t *
set_node_properties(radix_node_t *node, record_t *record, int common_bits,
                    int *prefix, int prefix_len, radix_node_t *branch_a,
                    radix_node_t *branch_b) {

    /* inserting the first node */
    if (branch_a == NULL && branch_b == NULL && prefix == NULL) {
        node->common_bits = common_bits;
        node->prefix = store_bit_array(prefix_len, record->trading_name);
        node->prefix_len = prefix_len * BITS_CHAR;
        node->records[node->num_records] = *record;
        node->num_records += 1;
        return node;
    }
    /* inserting a leaf node */
    else if (branch_b == NULL && branch_a == NULL && prefix != NULL) {
        node->common_bits = common_bits;
        node->prefix = prefix;
        node->prefix_len = prefix_len;
        node->branch_a = NULL;
        node->branch_b = NULL;
        node->records[node->num_records] = *record;
        node->num_records += 1;
        return node;
    }
    /* updating a parent node */
    else if (record == NULL && branch_a != NULL && branch_b != NULL) {
        node->common_bits = common_bits;
        /* no need to explicitly update prefix, just shorten length */
        node->prefix_len = prefix_len;
        node->branch_a = branch_a;
        node->branch_b = branch_b;
        node->num_records = 0;
        return node;
    }

    return node;
}

/* stores a string as its corresponding bit array */
int *
store_bit_array(int num_chars, char *key) {

    int *key_bit_array = (int *) malloc(sizeof(int) * num_chars * BITS_CHAR);
    assert(key_bit_array != NULL);

    int index = 0;
    for (int i = 0; i < num_chars; i++) {
        char c = key[i];
        for (int j = BITS_CHAR - 1; j >= 0; j--) {
            if (c == '\0') {
                key_bit_array[index++] = 0;
            } else {
                key_bit_array[index++] = get_bit(c, j);
            }
        }
    }

    return key_bit_array;
}

/* finds the matching prefix between 2 bit arrays and returns num of com bits */
int *
find_matching_prefix(int *prefix_bit_array, int prefix_bit_len,
                     int *key_bit_array, int key_bit_len, int *common_bits) {

    int comm_bits = 0;
    int shorter_len;

    if (prefix_bit_len <= key_bit_len) {
        shorter_len = prefix_bit_len;
    } else {
        shorter_len = key_bit_len;
    }

    for (int i = 0; i < shorter_len; i++) {
        if (prefix_bit_array[i] == key_bit_array[i]) {
            comm_bits++;
        } else {
            break;
        }
    }

    int *com_bit_array = (int *) malloc(sizeof(int) * comm_bits);
    assert(com_bit_array != NULL);
    for (int i = 0; i < comm_bits; i++) {
        (com_bit_array)[i] = prefix_bit_array[i];
    }

    *common_bits = comm_bits;

    return com_bit_array;
}

/* captues everything but the parent common prefix as a bit array */
int *
store_prefix(int comm_bits, int *full_bit_array, int num_char) {
    int *key_prefix = (int *) malloc(sizeof(int) * (num_char - comm_bits));
    assert(key_prefix != NULL);
    int index = comm_bits;
    for (int i = 0; i < num_char - comm_bits; i++) {
        key_prefix[i] = full_bit_array[index];
        index++;
    }
    return key_prefix;
}

/* converts a bit array into corresponding string representation */
char *
bit_to_str(int *bit_array, int bit_len) {

    int str_len = bit_len / 8;

    char *str = (char *) malloc(sizeof(char) * (str_len + 1));
    assert(str != NULL);

    for (int i = 0; i < bit_len; i += 8) {
        int val = 0;
        for (int j = 0; j < BITS_CHAR; j++) {
            val = (val << 1) | bit_array[i + j];
        }
        str[i / 8] = (char) val;
    }

    return str;
}

/* Returns the nth bit of a char, 0th bit being the least sig bit */
int
get_bit(char key, int n) {
    int bit = (key >> n) & 1;
    return bit;
}

/* traverses radix tree to find matching bit representation of tree */
void
radix_find_and_traverse(int *trading_bit, int len, radix_node_t *radix_records,
                        FILE *out_file, int *b, int *c, int *s) {
    if (radix_records != NULL) {
        radix_find_and_traverse(trading_bit, len, radix_records->branch_a,
                                out_file, b, c, s);

        /* check if there are any records to compare to */
        if (radix_records->num_records != 0) {
            char *key = bit_to_str(trading_bit, len * BITS_CHAR);
            int outcome = strcmp(key, radix_records->records[0].trading_name);
            if (outcome == 0) {
                *s += 1;
                for (int i = 0; i < radix_records->num_records; i++) {
                    /* ignore null byte */
                    *b += radix_records->common_bits - BITS_CHAR;
                    *c += radix_records->prefix_len / BITS_CHAR - 1;
                    print_to_outfile(out_file, radix_records->records[i]);
                }
            }
            free(key);
            return;
        }

        radix_find_and_traverse(trading_bit, len, radix_records->branch_b,
                                out_file, b, c, s);
    }
}

/* alternative traversal utilising more bit comparison UNABLE TO GET WORKING
 */
void
radix_find_and_traverse_2(int *trading_bit, int len,
                          radix_node_t *radix_records, FILE *out_file, int *b,
                          int *c, int *s) {

    radix_node_t *current = radix_records;
    int match = 0;
    int bit_len = len * BITS_CHAR;
    int *bit_traversal = (int *) malloc(sizeof(int) * bit_len);
    assert(bit_traversal != NULL);
    int total_length = 0;
    int bit_index = 0;

    while (current != NULL) {
        *s += 1;
        int prefix_len = current->prefix_len;
        /* for every node we visit, paste the prefix into bit_traversal */
        paste_prefix(bit_traversal, bit_len, current->prefix, prefix_len,
                     &bit_index, &total_length);

        /* if trading_bit is subset of the bit traversal, there is a match
         */
        match = is_match(trading_bit, bit_len, bit_traversal, total_length, b);

        if (match == MATCH || match == NO_MATCH) {
            break;
        }

        /* there is a potential match, so go to next branch depending on bit
         */
        int next_bit = trading_bit[total_length];
        if (next_bit == 0) {
            current = current->branch_a;
        } else if (next_bit == 1) {
            current = current->branch_b;
        }
    }

    printf("bit traversal is: \n");
    for (int i = 0; i < len * BITS_CHAR; i++) {
        printf("%d", bit_traversal[i]);
    }
    printf("\n");
    printf("trading bit is: \n");
    for (int i = 0; i < len * BITS_CHAR; i++) {
        printf("%d", trading_bit[i]);
    }
    printf("\n");

    /*output the corresponding prefix to the outfile */
    char *key = bit_to_str(trading_bit, bit_len);
    fprintf(out_file, "%s\n", key);
    free(key);

    /* if there is a match, print all records associated */
    if (match == MATCH) {
        for (int i = 0; i < current->num_records; i++) {
            print_to_outfile(out_file, current->records[i]);
        }
    }

    /* ignore \0 */
    *c = bit_len / 8 - 1;
    free(bit_traversal);
}

/* for every node we visit, paste the prefix into an array for comparison */
void
paste_prefix(int *bit_traversal, int bit_len, int *prefix, int prefix_len,
             int *index, int *total_len) {
    for (int i = 0; i < prefix_len && *total_len < bit_len; i++) {
        bit_traversal[*index] = prefix[i];
        *index += 1;
        *total_len += 1;
    }
}

/* determines whether there is a match between 2 bit arrays */
int
is_match(int *trading_bit, int trading_len, int *bit_traversal,
         int traversal_len, int *b) {
    /* only compare bits that have been pasted into bit traversal */
    /* 1. reached end of comparing, so match could be in another branch */
    if (traversal_len < trading_len) {
        for (int i = 0; i < traversal_len; i++) {
            *b += 1;
            if (trading_bit[i] != bit_traversal[i]) {
                return NO_MATCH;
            }
        }
    }
    /* 2. match there is a match within current branch */
    else if (traversal_len >= trading_len) {
        for (int i = 0; i < trading_len - BITS_CHAR; i++) {
            *b += 1;
            if (trading_bit[i] != bit_traversal[i]) {
                return NO_MATCH;
            }
        }
        return MATCH;
    }
    return MAYBE_MATCH;
}

/* frees any memory allocated to radix tree */
void
free_radix(radix_node_t *radix_records) {
    if (!radix_records) {
        return;
    }
    if (radix_records->branch_a) {
        free_radix(radix_records->branch_a);
    }
    if (radix_records->branch_b) {
        free_radix(radix_records->branch_b);
    }
    free(radix_records->prefix);
    free(radix_records);
}

/* prints all trading names in a radix tree */
void
print_all_leaf(radix_node_t *radix_records) {

    if (radix_records != NULL) {
        print_all_leaf(radix_records->branch_a);
        printf("numrecords = %d\n", radix_records->num_records);
        for (int i = 0; i < radix_records->num_records; i++) {
            printf("%dth record = %s\n", i,
                   radix_records->records[i].trading_name);
        }
        if (radix_records->num_records != 0) {
            for (int i = 0; i < radix_records->prefix_len; i++) {
                printf("%d", radix_records->prefix[i]);
            }
            printf("\n");
        }
        print_all_leaf(radix_records->branch_b);
    }
}

/* encapsulate regix autocomplete implementation */
void
regix_autocomplete(FILE *data_file, FILE *out_file) {
    /* create regix tree to store records */
    radix_node_t *radix_records = NULL;

    /* processing dataset */
    while (!feof(data_file)) {
        /* read each record */
        record_t *record = read_record(data_file);
        /* insert the record into the array */
        radix_records = insert_radix(radix_records, record);
        free(record);
    }

    /* processing partial trading_names from stdin */
    int b_ave = 0;
    int c_ave = 0;
    int s_ave = 0;
    double num_keys = 0;
    while (!feof(stdin)) {
        int b = 0, c = 0, s = 0;
        char trading_name[MAX_FIELD];
        fscanf(stdin, "%[^\n]\n", trading_name);

        /* convert the trading name to a bit array */
        int trading_bit_len = strlen(trading_name) + 1;
        int *trading_bit = store_bit_array(trading_bit_len, trading_name);

        /*output the corresponding prefix to the outfile */
        fprintf(out_file, "%s\n", trading_name);
        /* find matching records in sorted array and output to outfile */
        radix_find_and_traverse(trading_bit, trading_bit_len, radix_records,
                                out_file, &b, &c, &s);

        /* output number of comparisons to stdout */
        // fprintf(stdout, "%s --> b%d c%d s%d\n", trading_name, b, c, s);
        b_ave += b;
        c_ave += c;
        s_ave += s;
        num_keys++;
        free(trading_bit);
    }
    printf("%.2lf %.2lf %.2lf\n", b_ave / num_keys, c_ave / num_keys,
           s_ave / num_keys);

    free_radix(radix_records);
}