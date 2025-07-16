/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#ifndef SORTING_H
#define SORTING_H
#include "types.h"

int compare_pid(const void *a, const void *b);

int compare_pid_reverse(const void *a, const void *b);

int compare_command(const void *a, const void *b);

int compare_command_reverse(const void *a, const void *b);

int compare_state(const void *a, const void *b);

int compare_state_reverse(const void *a, const void *b);

int compare_ppid(const void *a, const void *b);

int compare_ppid_reverse(const void *a, const void *b);

int compare_utime(const void *a, const void *b);

int compare_utime_reverse(const void *a, const void *b);

int compare_stime(const void *a, const void *b);

int compare_stime_reverse(const void *a, const void *b);

int compare_nice(const void *a, const void *b);

int compare_nice_reverse(const void *a, const void *b);

int compare_start_time(const void *a, const void *b);

int compare_start_time_reverse(const void *a, const void *b);

int compare_vsize(const void *a, const void *b);

int compare_vsize_reverse(const void *a, const void *b);

int compare_user(const void *a, const void *b);

int compare_user_reverse(const void *a, const void *b);

void sort_processes(process_t **proc, int dir_size, int criteria, int order);

#endif