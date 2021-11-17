# Chicken file archiver
A file archiver for the egg format.

## What is egg format
The egg format is made up of one or more egglets; where an egglet records one file system object.

## The chicken file archiver can do these
- list the path names of each object in an egg

- list the permissions of each object in an egg

- list the size (number of bytes) of files in an egg

- check the egglet magic number

- extract files from an egg

- check an egg for integrity, by checking egglet hashes;

- set the file permissions of files extracted from an egg

- create an egg from a list of files

- list, extract, and create eggs that include directories

- extract, and create eggs in 7-bit and 6-bit formats

