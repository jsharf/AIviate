#include "database.h"

extern Serial pc;

db* init_database(char *mem, int size)
{
    memset(mem, 0, size);
    db *new_db = (db *)malloc(sizeof(db));
    new_db->records = (record *)mem;
    new_db->capacity = size/REC_SIZE;
    new_db->size = 0;
    return new_db;
}

void print_db(db* datab, bool one_line)
{
    int len = datab->size;
    pc.printf("Database: ");
    if(!one_line)
    {
        pc.printf("\r\n");
    }
    for (int i=0; i<len; i++)
    {
        pc.printf("%s %+8i%s", datab->records[i].name, datab->records[i].val, one_line?(" | "):("\r\n"));
    }
    if(one_line)
    {
        pc.printf("\r\n");
    }
    return;
}

void release_database(db *datab)
{
    free(datab);
}

int set(db *datab, char *name, int val)
{
    // if there's no space to insert
    // a new record, return failure
    if (datab->size >= datab->capacity)
        return -1;
    // check if the record is already in the database
    // note search_db takes O(logn)
    int check_idx;
    if ((check_idx = search_db(datab, name)) != -1)
    {
        // The record is already in the database; set its new value
        datab->records[check_idx].val = val;
        // Return success
        return 0;
    }
    for (int i=datab->size; i > 0; i--)
    {
        if (strcmp(datab->records[i - 1].name, name) != 1)
        {
            memcpy(datab->records[i].name, name, REC_SIZE-4);
            datab->records[i].val = val;
            datab->size += 1;
            return 0;
        }
        memcpy(datab->records+i, datab->records+i-1, REC_SIZE);  
    }
    memcpy(datab->records[0].name, name, REC_SIZE);
    datab->records[0].val = val;
    datab->size += 1;
    return 0;    
}

int search_db(db *datab, const char *name)
{
    if (datab->size == 0)
        return -1;
    int low, mid, high;
    low = 0;
    high = datab->size;
    mid = datab->size/2;
    while (high >= low)
    {
        mid = (low + high)/2;
        int cmp = strcmp(name, datab->records[mid].name);
        if (cmp >= 1)
        {
            low = mid+1;     
        }
        else if (cmp <= -1)
        {
            high = mid-1;
        }
        else if (cmp == 0)
        {
            return mid;
        }
    }
    return -1;
}

int del(db *datab, const char *name)
{
    int rec_ind = search_db(datab, name);
    if (rec_ind == -1)
    {
        return -1;
    }
    for (int i = rec_ind; i < (datab->size - 1); i++)
    {
        memcpy(datab->records + i, datab->records + i + 1, REC_SIZE);
    }
    datab->size--;
    return 0;
}