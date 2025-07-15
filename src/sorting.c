/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <stdlib.h>
#include <string.h>
#include "types.h"
int compare_pid(const void *a, const void *b)
{
    return (*(process_t **)a)->pid - (*(process_t **)b)->pid;
}

int compare_command(const void *a, const void *b)
{
    return strcmp((*(process_t **)a)->command, (*(process_t **)b)->command);
}

int compare_state(const void *a, const void *b)
{
    return (*(process_t **)a)->state - (*(process_t **)b)->state;
}

int compare_ppid(const void *a, const void *b)
{
    return (*(process_t **)a)->ppid - (*(process_t **)b)->ppid;
}

int compare_utime(const void *a, const void *b)
{
    return (*(process_t **)a)->utime - (*(process_t **)b)->utime;
}

int compare_stime(const void *a, const void *b)
{
    return (*(process_t **)a)->stime - (*(process_t **)b)->stime;
}

int compare_nice(const void *a, const void *b)
{
    return (*(process_t **)a)->nice - (*(process_t **)b)->nice;
}

int compare_start_time(const void *a, const void *b)
{
    return (*(process_t **)a)->start_time - (*(process_t **)b)->start_time;
}

int compare_vsize(const void *a, const void *b)
{
    return (*(process_t **)a)->vsize - (*(process_t **)b)->vsize;
}

int compare_user(const void *a, const void *b)
{
    return strcmp((*(process_t **)a)->user, (*(process_t **)b)->user);
}

void sort_processes(process_t **proc, int dir_size, int criteria)
{
    switch (criteria) {
        case PID:
            qsort(proc, dir_size, sizeof(process_t *), compare_pid);
            break;
        case COMMAND:
            qsort(proc, dir_size, sizeof(process_t *), compare_command);
            break;
        case STATE:
            qsort(proc, dir_size, sizeof(process_t *), compare_state);
            break;
        case PPID:
            qsort(proc, dir_size, sizeof(process_t *), compare_ppid);
            break;
        case UTIME:
            qsort(proc, dir_size, sizeof(process_t *), compare_utime);
            break;
        case STIME:
            qsort(proc, dir_size, sizeof(process_t *), compare_stime);
            break;
        case NICE:
            qsort(proc, dir_size, sizeof(process_t *), compare_nice);
            break;
        case START_TIME:
            qsort(proc, dir_size, sizeof(process_t *), compare_start_time);
            break;
        case VSIZE:
            qsort(proc, dir_size, sizeof(process_t *), compare_vsize);
            break;
        case USER:
            qsort(proc, dir_size, sizeof(process_t *), compare_user);
            break;
    }
}