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
struct egg
{
    uint8_t magic_number;
    uint8_t egglet_format;
    char *permissions;
    uint16_t pathname_length;
    char *pathname;
    uint64_t content_length;
    char *content;
    uint8_t hash;
};
typedef struct egg e;
typedef e *ChickenEgg;

// ADD YOUR FUNCTION PROTOTYPES HERE

// Functions to initialize the struct ChickenEgg:
ChickenEgg produce_egg(FILE *fptr);
void eat_egg(ChickenEgg egg);
void get_magic_number(ChickenEgg egg, FILE *fptr);
void get_egglet_format(ChickenEgg egg, FILE *fptr);
void get_permissions(ChickenEgg egg, FILE *fptr);
void get_pathname_length(ChickenEgg egg, FILE *fptr);
void get_pathname(ChickenEgg egg, FILE *fptr);
void get_content_length(ChickenEgg egg, FILE *fptr);
void get_content(ChickenEgg egg, FILE *fptr);
void get_hash(ChickenEgg egg, FILE *fptr);

// Helper functions:
void error_warning(char *words);

// Subset 0 functions:
void list_contents_detail(char *egg_pathname);
void list_contents(char *egg_pathname);

uint8_t check_hash(ChickenEgg egg);
// print the files & directories stored in egg_pathname (subset 0)
//
// if long_listing is non-zero then file/directory permissions, formats & sizes are also printed (subset 0)

void list_egg(char *egg_pathname, int long_listing)
{
    // Command L
    if (long_listing)
        list_contents_detail(egg_pathname);
    // Command l
    else
        list_contents(egg_pathname);
}

// check the files & directories stored in egg_pathname (subset 1)
//
// prints the files & directories stored in egg_pathname with a message
// either, indicating the hash byte is correct, or
// indicating the hash byte is incorrect, what the incorrect value is and the correct value would be

void check_egg(char *egg_pathname)
{
    // Call egglet_hash to calculate hash values.
    FILE *fptr = fopen(egg_pathname, "r");
    if (fptr == NULL)
    {
        error_warning("Cannot find current files.");
    }
    int ch;
    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch != EGGLET_MAGIC)
        {
            printf("error: incorrect first egglet byte: 0x%x should be 0x63\n", ch);
            exit(1);
        }
        // Produce a fresh egg
        ChickenEgg egg = produce_egg(fptr);
        uint8_t computed_hash = check_hash(egg);
        if (egg->hash == computed_hash)
        {
            // 256.bin - correct hash
            printf("%s - correct hash, computed 0x%x egghash 0x%x\n", egg->pathname, computed_hash, egg->hash);
        }
        else
        {
            // hello.sql - incorrect hash 0x19 should be 0x43
            printf("%s - incorrect hash 0x%x should be 0x%x\n", egg->pathname, computed_hash, egg->hash);
        }
        // Eat that egg
        eat_egg(egg);
    }
    fclose(fptr);
}

// extract the files/directories stored in egg_pathname (subset 2 & 3)

void extract_egg(char *egg_pathname)
{

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
                int n_pathnames, char *pathnames[n_pathnames])
{

    // REPLACE THIS CODE PRINTFS WITH YOUR CODE

    printf("create_egg called to create egg: '%s'\n", egg_pathname);
    printf("format = %x\n", format);
    printf("append = %d\n", append);
    printf("These %d pathnames specified:\n", n_pathnames);
    for (int p = 0; p < n_pathnames; p++)
    {
        printf("%s\n", pathnames[p]);
    }
}

// ADD YOUR EXTRA FUNCTIONS HERE

// Print when error occurs
void error_warning(char *words)
{
    perror(words);
    exit(1);
}

// Produce a new egg struct
ChickenEgg produce_egg(FILE *fptr)
{
    ChickenEgg egg = malloc(sizeof(e));
    get_magic_number(egg, fptr);
    get_egglet_format(egg, fptr);
    get_permissions(egg, fptr);
    get_pathname_length(egg, fptr);
    get_pathname(egg, fptr);
    get_content_length(egg, fptr);
    get_content(egg, fptr);
    get_hash(egg, fptr);

    return egg;
}

// Free the memory assigned
void eat_egg(ChickenEgg egg)
{
    free(egg->content);
    egg->content = NULL;
    free(egg->pathname);
    egg->pathname = NULL;

    free(egg);
}

// Get the magic number
void get_magic_number(ChickenEgg egg, FILE *fptr)
{
    egg->magic_number = EGGLET_MAGIC;
    // printf("magic number is: %hhu\n", egg->magic_number);
}

// Get the egglet format.
void get_egglet_format(ChickenEgg egg, FILE *fptr)
{
    egg->egglet_format = fgetc(fptr);
    // printf("egglet_format is: %c\n", egg->egglet_format);
}

// Get permissions of egg
void get_permissions(ChickenEgg egg, FILE *fptr)
{
    egg->permissions = calloc(sizeof(char), EGG_LENGTH_MODE + 1);
    for (long i = 0; i < EGG_LENGTH_MODE; i++)
    {
        egg->permissions[i] = fgetc(fptr);
    }
    // printf("permissions: %s\n", egg->permissions);
}

// Get pathname_length of egg
void get_pathname_length(ChickenEgg egg, FILE *fptr)
{
    egg->pathname_length = 0;
    int lil_endian = fgetc(fptr);
    int big_endian = fgetc(fptr);
    egg->pathname_length = lil_endian | (big_endian << 8);
    // printf("pathname_length is: %hu\n", egg->pathname_length);
}

// Get the pathname
void get_pathname(ChickenEgg egg, FILE *fptr)
{
    egg->pathname = calloc(sizeof(char), egg->pathname_length + 1);
    for (int i = 0; i < egg->pathname_length; i++)
    {
        egg->pathname[i] = fgetc(fptr);
    }
    // printf("pathname is %s\n", egg->pathname);
}

// Get the content length
void get_content_length(ChickenEgg egg, FILE *fptr)
{
    egg->content_length = 0;
    for (int i = 0; i < EGG_LENGTH_CONTLEN; i++)
    {
        long endian = fgetc(fptr);
        endian <<= (8 * i);
        egg->content_length |= endian;
    }
    // printf("content_length: %llu\n", egg->content_length);
}

// Get the content
void get_content(ChickenEgg egg, FILE *fptr)
{
    egg->content = calloc(sizeof(char), egg->content_length + 1);
    for (long i = 0; i < egg->content_length; i++)
    {
        egg->content[i] = fgetc(fptr);
    }
    //    printf("content is: %d\n", egg->content);
}

// Get the hash
void get_hash(ChickenEgg egg, FILE *fptr)
{
    egg->hash = fgetc(fptr);
}

void list_contents_detail(char *egg_pathname)
{
    FILE *fptr = fopen(egg_pathname, "r");
    if (fptr == NULL)
    {
        error_warning("Cannot find current files.");
    }
    char ch;
    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch != EGGLET_MAGIC)
        {
            error_warning("Incorrect egglet magic.");
        }
        // Produce a fresh egg
        ChickenEgg egg = produce_egg(fptr);

        printf("%s", egg->permissions);
        printf("  %c", egg->egglet_format);
        printf("%7llu", egg->content_length);
        printf("  %s\n", egg->pathname);

        // Eat that egg
        eat_egg(egg);
    }
    fclose(fptr);
}

void list_contents(char *egg_pathname)
{
    FILE *fptr = fopen(egg_pathname, "r");
    if (fptr == NULL)
    {
        error_warning("Cannot find current files.");
    }
    char ch;
    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch != EGGLET_MAGIC)
        {
            error_warning("Incorrect egglet magic.");
        }
        // Produce a fresh egg
        ChickenEgg egg = produce_egg(fptr);

        // Command l
        printf("%s\n", egg->pathname);

        // Eat that egg
        eat_egg(egg);
    }
    fclose(fptr);
}

uint8_t check_hash(ChickenEgg egg)
{
    uint8_t hash = 0;

    hash = egglet_hash(hash, egg->magic_number);
    hash = egglet_hash(hash, egg->egglet_format);

    for (int i = 0; i < EGG_LENGTH_MODE; i++)
    {
        hash = egglet_hash(hash, egg->permissions[i]);
    }

    hash = egglet_hash(hash, egg->pathname_length & 0xFF);
    hash = egglet_hash(hash, egg->pathname_length >> 8 & 0xFF);

    for (int i = 0; i < egg->pathname_length; i++)
    {
        hash = egglet_hash(hash, egg->pathname[i]);
    }

    for (int i = 0; i < EGG_LENGTH_CONTLEN; i++)
    {
        hash = egglet_hash(hash, egg->content_length >> 8 * i & 0xFF);
    }

    for (int i = 0; i < egg->content_length; i++)
    {
        hash = egglet_hash(hash, egg->content[i]);
    }

    return hash;
}