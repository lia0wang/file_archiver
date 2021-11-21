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
#include <string.h>
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
void list_L(char *egg_pathname);
void list_l(char *egg_pathname);

// Subset 1 functions:
uint8_t check_hash(ChickenEgg egg);
void check_C(char *egg_pathname);
void extract_x(char *egg_pathname);
char *get_pathname_array(ChickenEgg egg);
int get_octal_digit(ChickenEgg egg);

// Subset 2 functions:
FILE *check_create_egg_condition(int append, char *egg_pathname);
ChickenEgg produce_small_egg(int format, char *permissions, uint64_t content_length, char *pathnames, FILE *fptr);
ChickenEgg *catch_eggs(FILE *fptr, int n_pathnames, char *pathnames[n_pathnames], int format);
void eat_eggs(int length, ChickenEgg *egg);
void write_to_egg_file(int length, ChickenEgg *egg, FILE *fptr_eggs);
void create_C(char *egg_pathname, int append, int format,
              int n_pathnames, char *pathnames[n_pathnames]);

// print the files & directories stored in egg_pathname (subset 0)
//
// if long_listing is non-zero then file/directory permissions, formats & sizes are also printed (subset 0)

void list_egg(char *egg_pathname, int long_listing)
{
    // Command L
    if (long_listing)
        list_L(egg_pathname);
    // Command l
    else
        list_l(egg_pathname);
}

// check the files & directories stored in egg_pathname (subset 1)
//
// prints the files & directories stored in egg_pathname with a message
// either, indicating the hash byte is correct, or
// indicating the hash byte is incorrect, what the incorrect value is and the correct value would be

void check_egg(char *egg_pathname)
{
    // Command C
    check_C(egg_pathname);
}

// extract the files/directories stored in egg_pathname (subset 2 & 3)

void extract_egg(char *egg_pathname)
{
    // Command x
    extract_x(egg_pathname);
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
    // Command c
    create_C(egg_pathname, append, format, n_pathnames, pathnames);
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

void list_L(char *egg_pathname)
{
    FILE *fptr = fopen(egg_pathname, "r");
    if (fptr == NULL)
        error_warning("Cannot find current files.");

    int ch;
    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch != EGGLET_MAGIC)
            error_warning("Incorrect egglet magic.");

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

void list_l(char *egg_pathname)
{
    FILE *fptr = fopen(egg_pathname, "r");
    if (fptr == NULL)
        error_warning("Cannot find current files.");

    int ch;
    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch != EGGLET_MAGIC)
            error_warning("Incorrect egglet magic.");

        // Produce a fresh egg
        ChickenEgg egg = produce_egg(fptr);

        // Command l
        printf("%s\n", egg->pathname);

        // Eat that egg
        eat_egg(egg);
    }
    fclose(fptr);
}

// Check the hash value from hash = 0 for every byte
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

void check_C(char *egg_pathname)
{
    // Call egglet_hash to calculate hash values.
    FILE *fptr = fopen(egg_pathname, "r");
    if (fptr == NULL)
        error_warning("Cannot find current files.");
    int ch;
    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch != EGGLET_MAGIC)
        {
            fprintf(stderr, "error: incorrect first egglet byte: 0x%x should be 0x%x\n", ch, EGGLET_MAGIC);
            exit(1);
        }

        // Produce a fresh egg
        ChickenEgg egg = produce_egg(fptr);

        // Computed the hash value
        uint8_t computed_hash = check_hash(egg);
        if (egg->hash == computed_hash)
            printf("%s - correct hash\n", egg->pathname);
        else
            printf("%s - incorrect hash 0x%x should be 0x%x\n", egg->pathname, computed_hash, egg->hash);

        // Eat that egg
        eat_egg(egg);
    }
    fclose(fptr);
}

void extract_x(char *egg_pathname)
{
    // Call egglet_hash to calculate hash values.
    FILE *fptr = fopen(egg_pathname, "r");
    if (fptr == NULL)
        error_warning("Cannot find current files.");
    int ch;
    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch != EGGLET_MAGIC)
        {
            fprintf(stderr, "error: incorrect first egglet byte: 0x%x should be 0x%x\n", ch, EGGLET_MAGIC);
            exit(1);
        }
        // Produce a fresh egg
        ChickenEgg egg = produce_egg(fptr);

        // get the path name array
        char *pathname_array = get_pathname_array(egg);

        // find the octal_digit
        int octal_digit = get_octal_digit(egg);

        // give permissions to the files
        chmod(pathname_array, octal_digit);
        free(pathname_array);

        eat_egg(egg);
    }
    fclose(fptr);
}

char *get_pathname_array(ChickenEgg egg)
{
    // Copt the path names to a temp array.
    char *array = calloc(sizeof(char), egg->pathname_length + 1);
    strncpy(array, egg->pathname, egg->pathname_length);

    FILE *f_out_ptr = fopen(array, "w");

    // Extracting: hello.rs
    printf("Extracting: %s\n", array);

    // write the egg content to the output stream.
    for (long i = 0; i < egg->content_length; i++)
    {
        fputc(egg->content[i], f_out_ptr);
        // printf("%c\n", egg->content[i]);
    }
    fclose(f_out_ptr);

    return array;
}

int get_octal_digit(ChickenEgg egg)
{
    int *array = calloc(sizeof(int), EGG_LENGTH_MODE + 1);
    for (int i = 1; i < EGG_LENGTH_MODE; i++)
    {
        if (egg->permissions[i] == '-')
            array[i] = 0;
        else if (egg->permissions[i] == 'r')
            array[i] = 4;
        else if (egg->permissions[i] == 'w')
            array[i] = 2;
        else if (egg->permissions[i] == 'x')
            array[i] = 1;
    }

    int digit_one = 0;
    int digit_two = 0;
    int digit_three = 0;
    for (int i = 0; i < EGG_LENGTH_MODE; i++)
    {
        // -xxx 0123
        if (i < 4)
            digit_one += array[i];
        // xxx 456
        if (i > 3 && i <= 6)
            digit_two += array[i];
        // xxx 789, EGG_LENGTH_MODE = 10
        if (i > 6 && i < EGG_LENGTH_MODE)
            digit_three += array[i];
    }
    free(array);

    return digit_one * 64 + digit_two * 8 + digit_three;
}

FILE *check_create_egg_condition(int append, char *egg_pathname)
{
    FILE *fptr;
    // Overwrite if it is not appending
    if (!append)
        fptr = fopen(egg_pathname, "w");
    // Append the file if the append is 1
    else
        fptr = fopen(egg_pathname, "a+");

    // Perror if the file is null
    if (fptr == NULL)
        error_warning(egg_pathname);

    return fptr;
}

ChickenEgg produce_small_egg(int format, char *permissions, uint64_t content_length, char *pathnames, FILE *fptr)
{
    ChickenEgg small_egg = malloc(sizeof(e));
    // Initialize the assigned values.
    int pathnames_len = strlen(pathnames);
    char *pathname = calloc((sizeof(char)), pathnames_len + 1);
    strncpy(pathname, pathnames, pathnames_len);

    // Assigned the values to each attribute by sequence.
    small_egg->magic_number = EGGLET_MAGIC;
    small_egg->egglet_format = format;
    small_egg->permissions = permissions;
    small_egg->pathname_length = pathnames_len;
    small_egg->pathname = pathname;
    small_egg->content_length = content_length;
    get_content(small_egg, fptr);
    small_egg->hash = fgetc(fptr);

    return small_egg;
}

ChickenEgg *catch_eggs(FILE *fptr, int n_pathnames, char *pathnames[n_pathnames], int format)
{
    ChickenEgg *egg = malloc((sizeof(ChickenEgg)) * n_pathnames);

    for (int p = 0; p < n_pathnames; p++)
    {
        printf("Adding: %s\n", pathnames[p]);

        FILE *fptr = fopen(pathnames[p], "r");
        struct stat s;
        if (stat(pathnames[p], &s) != 0)
            error_warning(pathnames[p]);
        // Get the permissions
        char *permissions = calloc(sizeof(char), EGG_LENGTH_MODE + 1);
        permissions[0] = *(S_ISDIR(s.st_mode) ? "d" : "-");
        permissions[1] = *((s.st_mode & S_IRUSR) ? "r" : "-");
        permissions[2] = *((s.st_mode & S_IWUSR) ? "w" : "-");
        permissions[3] = *((s.st_mode & S_IXUSR) ? "x" : "-");
        permissions[4] = *((s.st_mode & S_IRGRP) ? "r" : "-");
        permissions[5] = *((s.st_mode & S_IWGRP) ? "w" : "-");
        permissions[6] = *((s.st_mode & S_IXGRP) ? "x" : "-");
        permissions[7] = *((s.st_mode & S_IROTH) ? "r" : "-");
        permissions[8] = *((s.st_mode & S_IWOTH) ? "w" : "-");
        permissions[9] = *((s.st_mode & S_IXOTH) ? "x" : "-");

        // Get the content length
        uint64_t content_length = s.st_size;
        // printf("content length is %llu\n", cont_len);

        // for each file in .egg, produce a small egg.
        egg[p] = produce_small_egg(format, permissions, content_length, pathnames[p], fptr);

        fclose(fptr);
    }

    return egg;
}

void eat_eggs(int length, ChickenEgg *egg)
{
    for (int p = 0; p < length; p++)
    {
        free(egg[p]);
    }
    free(egg);
}

void write_to_egg_file(int length, ChickenEgg *egg, FILE *fptr_eggs)
{
    for (int p = 0; p < length; p++)
    {
        fputc(egg[p]->magic_number, fptr_eggs);
        fputc(egg[p]->egglet_format, fptr_eggs);
        for (int i = 0; i < EGG_LENGTH_MODE; i++)
        {
            fputc(egg[p]->permissions[i], fptr_eggs);
        }
        char c = 0;
        for (int i = 0; i < EGG_LENGTH_PATHNLEN; i++)
        {
            c = (egg[p]->pathname_length >> (8 * i)) & 0xff;
            fputc(c, fptr_eggs);
        }
        for (int i = 0; i < egg[p]->pathname_length; i++)
        {
            fputc(egg[p]->pathname[i], fptr_eggs);
        }
        c = 0;
        for (int i = 0; i < EGG_LENGTH_CONTLEN; i++)
        {
            c = (egg[p]->content_length >> (8 * i)) & 0xff;
            fputc(c, fptr_eggs);
        }
        for (int i = 0; i < egg[p]->content_length; i++)
        {
            fputc(egg[p]->content[i], fptr_eggs);
        }
        fputc(check_hash(egg[p]), fptr_eggs);
    }
}

// A function packages 4 functions
void create_C(char *egg_pathname, int append, int format,
              int n_pathnames, char *pathnames[n_pathnames])
{
    FILE *fptr_eggs = check_create_egg_condition(append, egg_pathname);
    ChickenEgg *egg = catch_eggs(fptr_eggs, n_pathnames, pathnames, format);
    write_to_egg_file(n_pathnames, egg, fptr_eggs);
    eat_eggs(n_pathnames, egg);
}