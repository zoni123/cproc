/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define STAT_LINE_LENGTH 256
#define REFRESH_RATE 1000000
#define TRUNC_CHAR 12

typedef enum {
    SUCCESS,
    PROC_OPEN_ERROR,
    MALLOC_FAILED,
    STAT_ERROR
} error_codes_t;

typedef struct {
    FILE *stat;
    pid_t pid;
    char *stat_path;
} process_t;

int main(void)
{
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return PROC_OPEN_ERROR;
    }

    while (1) {
        printf("\033[H\033[J");
        printf("PID\tCOMMAND\t\tSTATE\tPPID\tUTIME\tSTIME\tPRIOR\tSTART\tVSIZE\n");
        rewinddir(dir);
        
        struct dirent *ent;
        while (ent = readdir(dir)) {
            bool valid_dir = 1;
            if (ent->d_type == DT_DIR) {
                for (int i = 0; i < strlen(ent->d_name) && valid_dir; i++) {
                    if (ent->d_name[i] < '0' || ent->d_name[i] > '9') {
                        valid_dir = 0;
                    }
                }

                if (valid_dir) {
                    process_t *proc = malloc(sizeof(process_t));
                    char stat_content[STAT_LINE_LENGTH];

                    if (!proc) {
                        perror("Malloc failed");
                        closedir(dir);
                        return MALLOC_FAILED;
                    }

                    proc->pid = atoi(ent->d_name);
                    proc->stat_path = malloc(strlen("/proc/") + strlen(ent->d_name) + strlen("/stat") + 1);

                    if (!proc->stat_path) {
                        perror("Malloc failed");
                        free(proc);
                        closedir(dir);
                        return MALLOC_FAILED;
                    }

                    strcpy(proc->stat_path, "/proc/");
                    strcat(proc->stat_path, ent->d_name);
                    strcat(proc->stat_path, "/stat");

                    proc->stat = fopen(proc->stat_path, "r");

                    if (!proc->stat) {
                        perror("No stat file");
                        free(proc->stat_path);
                        free(proc);
                        closedir(dir);
                        return STAT_ERROR;
                    }

                    for (int i = 0; i < 40; i++) {
                        if (i == 0 || i == 1 || i == 2 || i == 3 || i == 13 || i == 14 || i == 17 || i == 22 || i == 23) {
                            fscanf(proc->stat, "%s", stat_content);
                            printf("%.*s", TRUNC_CHAR, stat_content);
                            if (i == 1) {
                                if (strlen(stat_content) < 8) {
                                    printf("\t");
                                } else if (strlen(stat_content) >= TRUNC_CHAR) {
                                    printf("...");
                                }
                            }
                            printf("\t");
                        }
                        
                    }
                    printf("\n");
                    fclose(proc->stat);
                    free(proc->stat_path);
                    free(proc);
                }
            }
        }
        usleep(REFRESH_RATE);
    }
}