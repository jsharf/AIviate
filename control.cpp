#include "control.h"

#define MAXPROC 15

Serial pc(USBTX, USBRX);
process procs[MAXPROC] = {EMPTY};

int main()
{
    pc.baud(115200);
    init();
    while (true) {
        schedule();
    }
}

void init()
{
    // set all processes to "empty"
    for (int i=0; i<MAXPROC; i++) {
        procs[i].status = EMPTY;
    }
    
    schedule_proc("MAINPROC", main_proc);
    
    return;
}

// takes string process identifier
// and returns its pid
// -1 if sid not found
int find_proc(char *sid)
{
    int i;
    for (i=0; i<MAXPROC; i++) {
        process *proc = &(procs[i]);
        if (proc->status == READY && strncmp(proc->sid, sid, MAX_SID_LEN) == 0) {
            return i;
        }
    }
    return -1;
}

char* get_output(int pid)
{
    return procs[pid].output;
}

db* get_db(int pid)
{
    return procs[pid].data;
}

void print_all_db()
{
    for (int i=0; i<MAXPROC; i++) {
        if (procs[i].status == READY) {
            pc.printf("Proc: %s (%i)\r\n", procs[i].sid, i);
            print_db(procs[i].data);
        }
    }
}

void schedule()
{
#ifdef PRINT_PROC_TABLE
    for (int i=0; i<MAXPROC; i++) {
        pc.printf("Proc: %s\tStat:%d\r\n", procs[i].sid, (int)procs[i].status);
    }
#endif
    for (int i=0; i<MAXPROC; i++) {
        process *proc = &(procs[i]);
        if(proc->status == READY) {
            int ret = proc->start(i);
            // if the process returns 0, it means don't run again
            if (ret == PROC_BREAK) {
                // set proc.status to ZOMBIE
                // ZOMBIE means process
                // is no longer active, but
                // it's output buffer is still
                // needed.
                //proc->status = ZOMBIE;
                proc->status = EMPTY;
            }
        }
    }
}

int schedule_proc(char *sid, int (*funcptr)(int))
{
    for (int i=0; i<MAXPROC; i++) {
        process *proc = &(procs[i]);
        if(proc->status == EMPTY) {
            proc->status = READY;
            proc->start = funcptr;
            strncpy(proc->sid, sid, MAX_SID_LEN-1);
            // null terminate proc.sid
            proc->sid[MAX_SID_LEN-1] = 0;
            proc->data = init_database(proc->output, OUTPUT_SIZE);
            return i;
        }
    }
    // if no open processes, return -1
    return -1;
}