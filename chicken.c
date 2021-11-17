////////////////////////////////////////////////////////////////////////
// COMP1521 21T3 --- Assignment 2: `chicken', a simple file archiver
// <https://www.cse.unsw.edu.au/~cs1521/21T3/assignments/ass2/index.html>
//
// Written by Wang Liao (z5306312) on 17-Nov-2021
//
// 2021-11-08   v1.1    Team COMP1521 <cs1521 at cse.unsw.edu.au>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "chicken.h"


// ADD ANY extra #defines HERE
struct egg {
    uint8_t magic_number;
    uint8_t egglet_format;
    char *mode;
    uint16_t pathname_length;
    char *pathname;
    uint64_t content_length;
    int *contents;
    uint8_t hash;
};
typedef struct egg *ChickenEgg;

// ADD YOUR FUNCTION PROTOTYPES HERE
void error_warning(char *words);
ChickenEgg produceEgg(FILE *fptr);
void getMagicNum(ChickenEgg egg);
void getMode(ChickenEgg egg, FILE *fptr);
void getEggletFormat(ChickenEgg egg, FILE *fptr);
void getContentLength(ChickenEgg egg, FILE *fptr);
void getPathName(ChickenEgg egg, FILE *fptr);
void getPathNameLength(ChickenEgg egg, FILE *fptr);
void getContents(ChickenEgg egg, FILE *fptr);

void eatEgg(ChickenEgg egg);
// print the files & directories stored in egg_pathname (subset 0)
//
// if long_listing is non-zero then file/directory permissions, formats & sizes are also printed (subset 0)

void list_egg(char *egg_pathname, int long_listing) {
    if (!long_listing) {
        FILE *fptr = fopen(egg_pathname, "rb");
        if (fptr == NULL){
            error_warning("Cannot find current files.");
        }
        char ch;
        while ((ch = fgetc(fptr)) != EOF) {
            if (ch != EGGLET_MAGIC) {
                error_warning("Incorrect egglet.");
            }
            ChickenEgg egg = produceEgg(fptr);
            printf("%s\n", egg->pathname);
            eatEgg(egg);
        }
    }
}


// check the files & directories stored in egg_pathname (subset 1)
//
// prints the files & directories stored in egg_pathname with a message
// either, indicating the hash byte is correct, or
// indicating the hash byte is incorrect, what the incorrect value is and the correct value would be

void check_egg(char *egg_pathname) {

    // REPLACE THIS PRINTF WITH YOUR CODE

    printf("check_egg called to check egg: '%s'\n", egg_pathname);
}


// extract the files/directories stored in egg_pathname (subset 2 & 3)

void extract_egg(char *egg_pathname) {

    // REPLACE THIS PRINTF WITH YOUR CODE

    printf("extract_egg called to extract egg: '%s'\n", egg_pathname);
}


// create egg_pathname containing the files or directories specified in pathnames (subset 3)
//
// if append is zero egg_pathname should be over-written if it exists
// if append is non-zero egglets should be instead appended to egg_pathname if it exists
//
// format specifies the egglet format to use, it must be one EGGLET_FMT_6,EGGLET_FMT_7 or EGGLET_FMT_8

void create_egg(char *egg_pathname, int append, int format,
                int n_pathnames, char *pathnames[n_pathnames]) {

    // REPLACE THIS CODE PRINTFS WITH YOUR CODE

    printf("create_egg called to create egg: '%s'\n", egg_pathname);
    printf("format = %x\n", format);
    printf("append = %d\n", append);
    printf("These %d pathnames specified:\n", n_pathnames);
    for (int p = 0; p < n_pathnames; p++) {
        printf("%s\n", pathnames[p]);
    }
}


// ADD YOUR EXTRA FUNCTIONS HERE

// Print when error occurs
void error_warning(char *words) {
    perror(words);
    exit(1);
}

// Produce a new egg struct
ChickenEgg produceEgg(FILE *fptr) {
    ChickenEgg egg = malloc(sizeof(ChickenEgg));
    getMagicNum(egg);
    printf("yes\n");
    getMode(egg, fptr);
    printf("yes\n");

    getEggletFormat(egg, fptr);
    printf("yes\n");

    getPathNameLength(egg, fptr);
    printf("yes\n");

    getContentLength(egg, fptr);
    printf("yes\n");

    getPathName(egg, fptr);
    printf("yes\n");

    // getContents(egg, fptr);
    printf("yes\n");

    return egg;
}

// Free the memory assigned 
void eatEgg(ChickenEgg egg) {
    free(egg->pathname);
    egg->pathname = NULL;
    free(egg);
}

// Get the magic number
void getMagicNum(ChickenEgg egg) {
    egg->magic_number = EGGLET_MAGIC;
}

// Get mode of egg
void getMode(ChickenEgg egg, FILE *fptr) {
    egg->mode = calloc(sizeof(char), EGG_LENGTH_MODE + 1); // 1 is the space for null
    for (long i = 0; i < EGG_LENGTH_MODE; i++) {
        egg->mode[i] = fgetc(fptr);
    }
}

void getEggletFormat(ChickenEgg egg, FILE *fptr) {
    egg->egglet_format = EGGLET_FMT_8;
}

void getContentLength(ChickenEgg egg, FILE *fptr) {
    egg->content_length = 0;
    for (long offset = EGG_LENGTH_CONTLEN - 1; offset >= 0; offset--) {
        egg->content_length |= ((long)(fgetc(fptr) << (offset * 8)));
    }
}

void getPathName(ChickenEgg egg, FILE *fptr) {
    egg->pathname = calloc(sizeof(char), egg->pathname_length + 1);
    for (int i = 0; i < egg->pathname_length; i++) {
        egg->pathname[i] = fgetc(fptr);
    }
}

void getPathNameLength(ChickenEgg egg, FILE *fptr) {
    egg->pathname_length = 0;
    for (int offset = EGG_LENGTH_PATHNLEN - 1; offset >= 0; offset--) {
        egg->pathname_length |= (fgetc(fptr) << (offset * 8));
    }
}
void getContents(ChickenEgg egg, FILE *fptr) {
    egg->contents = calloc(sizeof(char), egg->content_length + 1);
    for (long i = 0; i < egg->content_length; i++) {
        egg->contents[i] = fgetc(fptr);
    }
}