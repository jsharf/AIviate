#ifndef DATABASE_H
#define DATABASE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mbed.h"

#define REC_SIZE 16

typedef int recordType;

// Record struct
typedef struct
{
    recordType val;
    char name[REC_SIZE-sizeof(recordType)];
} record;

// Database struct
typedef struct
{
    int size, capacity;
    record* records;
} db;

// creates and returns a database to manage a block of memory
// the database is a map between record names (c-strings), and
// 32-bit integers
db* init_database(char *mem, int size);

// cleans up database from memory
void release_database(db* datab);

// prints database to USB serial port (for debugging)
void print_db(db* datab, bool one_line = true);

// inserts a record into database "data"
// or updates it if it already exists
// returns -1 on failure, 0 on success
// insertion keeps the database in-order
int set(db *datab, char *name, int val);

// removes record from db
// returns -1 on failure, 0 on success
int del(db *datab, char *name);

// searches for record with tag "val" in database
// uses binary search (database is kept in order) for O(logn) search.
// returns index of record, or -1 if no such record exists
int search_db(db *datab, const char *val);


#endif