#ifndef _DATASET_H_
#define _DATASET_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELD   128
#define MAX_DATASET 512

// struct declarations
/* each line in the dataset will be stored in a record_t struct */
typedef struct {
    int census_year;
    int block_id;
    int property_id;
    int base_property_id;
    char building_address[MAX_FIELD];
    char clue_small_area[MAX_FIELD];
    char business_address[MAX_FIELD];
    char trading_name[MAX_FIELD];
    int industry_code;
    char industry_description[MAX_FIELD];
    char seating_type[MAX_FIELD];
    int number_of_seats;
    double longitude;
    double latitude;
} record_t;

// function prototypes
/*scans in a record from input dataset */
record_t *read_record(FILE *datafile);
/* process the next csv string field to remove " " if necessary and store*/
void remove_quotes(FILE *datafile, char *field);
/* outputs the matching record to the outfile */
void print_to_outfile(FILE *outfile, record_t record);

#endif