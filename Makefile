# Adapted from W2.3 generous_Makefile

# Define C compiler & flags
CC = gcc
CFLAGS = -Wall -g
# Define libraries to be linked (for example -lm)
LDLIBS = 

# Define sets of source files and object files
SRC_DICT2 = main.c dataset.c array.c binary_search.c radix.c
SRC_DICT3 = main.c dataset.c array.c binary_search.c radix.c
# OBJ is the same as SRC, just replace .c with .o
OBJ_DICT2 = $(SRC_DICT2:.c=.o)
OBJ_DICT3 = $(SRC_DICT3:.c=.o)
 
# Define the executable name(s)
EXE_DICT2 = dict2
EXE_DICT3 = dict3

# The first target:
$(EXE_DICT2): $(OBJ_DICT2) 
	$(CC) $(CFLAGS) -o $(EXE_DICT2) $(OBJ_DICT2) $(LDLIBS)

# The second target:
$(EXE_DICT3): $(OBJ_DICT3) 
	$(CC) $(CFLAGS) -o $(EXE_DICT3) $(OBJ_DICT3) $(LDLIBS)

clean:
	rm -f $(OBJ_DICT2) $(OBJ_DICT3) $(EXE_DICT2) $(EXE_DICT3)
