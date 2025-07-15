/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <sys/types.h>

#define STAT_LINE_LENGTH 1024
#define REFRESH_RATE 1000000
#define TRUNC_CHAR 12
#define EXIT_BUFFER_SIZE 64

typedef enum {
    SUCCESS,
    PROC_OPEN_ERROR,
    MALLOC_FAILED,
    STAT_ERROR,
    NO_EXIT
} error_codes_t;

typedef enum {
    PID = 0,
    COMMAND = 1,
    STATE = 2,
    PPID = 3,
    UTIME = 13,
    STIME = 14,
    PRIORITY = 17,
    NICE = 18,
    START_TIME = 21,
    VSIZE = 22
} stat_attributes_t;

typedef struct {
    FILE *stat_file;
    pid_t pid;
    char *stat_path;
} process_t;

#endif