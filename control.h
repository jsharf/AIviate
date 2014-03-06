#ifndef CONTROL_H
#define CONTROL_H

#include "database.h"
#include "sensor.h"
#include "steps.h"
#include "mbed.h"

#define OUTPUT_SIZE 512
#define MAX_SID_LEN 32

#define PI 3.1415926f

//#define PRINT_PROC_TABLE

typedef enum {
    PROC_CONTINUE,
    PROC_BREAK
} proc_ret_t;

extern int main_proc(int pid);

char* get_output(int);
db* get_db(int);
void init();
void schedule();
int schedule_proc(char *sid, int (*funcptr)(int));
int find_proc(char *sid);

typedef enum
{
    EMPTY = 0,
    READY,
    BLOCKED,
    ZOMBIE
} proc_stat;

typedef struct
{
    // process status
    proc_stat status;
    // if status == BLOCKED, wait till block_pid's status is ZOMBIE
    unsigned int block_pid;
    // function. each "process" is a function with a number of properties and storage space
    int (*start)(int);
    char sid[MAX_SID_LEN];
    // raw process output memory
    char output[OUTPUT_SIZE];
    // interface for accessing process output memory
    // that simplifies a lot of operations. see Database.h
    db *data;
} process;

void print_all_db();

#endif //CONTROL_H