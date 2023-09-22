#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "dataset.h"
#include "radix.h"

#define STAGE_1  "1"
#define STAGE_2  "2"
#define STAGE_3  "3"
#define NOTFOUND 0

int
main(int argc, char **argv) {

    /* read in command line arguments */
    char *stage_num = argv[1];
    FILE *data_file = fopen(argv[2], "r");
    assert(data_file != NULL);
    FILE *out_file = fopen(argv[3], "w");
    assert(out_file != NULL);

    /* read and disregard the header */
    char header[MAX_DATASET];
    fscanf(data_file, "%[^\n]\n", header);

    /* implement autocomplete depending on program configuration */
    if (strcmp(stage_num, STAGE_2) == 0) {
        array_autocomplete(data_file, out_file);
    } else if (strcmp(stage_num, STAGE_3) == 0) {
        regix_autocomplete(data_file, out_file);
    }

    /* clean up */
    fclose(data_file);
    fclose(out_file);
    return 0;
}
