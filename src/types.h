#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <sys/types.h>

#define STAT_LINE_LENGTH 256
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

typedef struct {
    FILE *stat;
    pid_t pid;
    char *stat_path;
} process_t;

#endif