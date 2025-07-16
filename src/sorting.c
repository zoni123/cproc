/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <stdlib.h>
#include <string.h>
#include "types.h"
int compare_pid(const void *a, const void *b)
{
    return (*(process_t **)a)->pid - (*(process_t **)b)->pid;
}

int compare_pid_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->pid - (*(process_t **)a)->pid;
}

int compare_command(const void *a, const void *b)
{
    return strcmp((*(process_t **)a)->command, (*(process_t **)b)->command);
}

int compare_command_reverse(const void *a, const void *b)
{
    return strcmp((*(process_t **)b)->command, (*(process_t **)a)->command);
}

int compare_state(const void *a, const void *b)
{
    return (*(process_t **)a)->state - (*(process_t **)b)->state;
}

int compare_state_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->state - (*(process_t **)a)->state;
}

int compare_ppid(const void *a, const void *b)
{
    return (*(process_t **)a)->ppid - (*(process_t **)b)->ppid;
}

int compare_ppid_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->ppid - (*(process_t **)a)->ppid;
}

int compare_utime(const void *a, const void *b)
{
    return (*(process_t **)a)->utime - (*(process_t **)b)->utime;
}

int compare_utime_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->utime - (*(process_t **)a)->utime;
}

int compare_stime(const void *a, const void *b)
{
    return (*(process_t **)a)->stime - (*(process_t **)b)->stime;
}

int compare_stime_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->stime - (*(process_t **)a)->stime;
}

int compare_nice(const void *a, const void *b)
{
    return (*(process_t **)a)->nice - (*(process_t **)b)->nice;
}

int compare_nice_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->nice - (*(process_t **)a)->nice;
}

int compare_start_time(const void *a, const void *b)
{
    return (*(process_t **)a)->start_time - (*(process_t **)b)->start_time;
}

int compare_start_time_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->start_time - (*(process_t **)a)->start_time;
}

int compare_vsize(const void *a, const void *b)
{
    return (*(process_t **)a)->vsize - (*(process_t **)b)->vsize;
}

int compare_vsize_reverse(const void *a, const void *b)
{
    return (*(process_t **)b)->vsize - (*(process_t **)a)->vsize;
}

int compare_user(const void *a, const void *b)
{
    return strcmp((*(process_t **)a)->user, (*(process_t **)b)->user);
}

int compare_user_reverse(const void *a, const void *b)
{
    return strcmp((*(process_t **)b)->user, (*(process_t **)a)->user);
}

void sort_processes(process_t **proc, int dir_size, int criteria, int order)
{
    switch (criteria) {
        case PID:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_pid);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_pid_reverse);
            }
            break;
        case COMMAND:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_command);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_command_reverse);
            }
            break;
        case STATE:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_state);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_state_reverse);
            }
            break;
        case PPID:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_ppid);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_ppid_reverse);
            }
            break;
        case UTIME:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_utime);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_utime_reverse);
            }
            break;
        case STIME:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_stime);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_stime_reverse);
            }
            break;
        case NICE:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_nice);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_nice_reverse);
            }
            break;
        case START_TIME:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_start_time);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_start_time_reverse);
            }
            break;
        case VSIZE:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_vsize);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_vsize_reverse);
            }
            break;
        case USER:
            if (order == 1) {
                qsort(proc, dir_size, sizeof(process_t *), compare_user);
            } else {
                qsort(proc, dir_size, sizeof(process_t *), compare_user_reverse);
            }
            break;
    }
}