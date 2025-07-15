/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include "types.h"
#include "io.h"
#include "memory.h"

bool check_dir_validity(struct dirent *ent)
{
    if (ent->d_type == DT_DIR) {
        for (size_t i = 0; i < strlen(ent->d_name); i++) {
            if (ent->d_name[i] < '0' || ent->d_name[i] > '9') {
                return false;
            }
        }
        return true;
    }
    return false;
}

void refresh(void)
{
    printf("\033[H\033[J");
    printf("PID\tCOMMAND\t\tSTATE\tPPID\tUTIME\tSTIME\tPRIOR\tNICE\tSTART\t\tVSIZE\t\tUSER\n");
}

int main(void)
{
    DIR *dir = opendir("/proc");
    struct termios original;
    enable_raw_mode(&original);

    if (!dir) {
        perror("Failed to open /proc");
        return PROC_OPEN_ERROR;
    }

    while (1) {
        int dir_size = 0, buff_len = 0;
        process_t **proc = NULL;
        char exit_buffer[EXIT_BUFFER_SIZE], stat_content[STAT_LINE_LENGTH];
        struct dirent *ent = NULL;
        struct passwd *user = NULL;
        struct stat status_uid;

        if (!check_exit(exit_buffer, buff_len)) {
            cleanup(proc, dir_size, dir, &original);
            return SUCCESS;
        }

        refresh();

        rewinddir(dir);
        while ((ent = readdir(dir))) {
            if (check_dir_validity(ent)) {
                proc = realloc(proc, sizeof(process_t *) * (dir_size + 1));

                if (!proc) {
                    perror("Malloc failed");
                    cleanup(proc, dir_size, dir, &original);
                    return MALLOC_FAILED;
                }

                proc[dir_size] = malloc(sizeof(process_t));
                if (!proc[dir_size]) {
                    perror("Malloc failed");
                    cleanup(proc, dir_size, dir, &original);
                    return MALLOC_FAILED;
                }

                proc[dir_size]->pid = atoi(ent->d_name);
                proc[dir_size]->stat_path = malloc(strlen("/proc/") + strlen(ent->d_name) + strlen("/stat") + 1);

                if (!proc[dir_size]->stat_path) {
                    perror("Malloc failed");
                    cleanup(proc, dir_size, dir, &original);
                    return MALLOC_FAILED;
                }

                strcpy(proc[dir_size]->stat_path, "/proc/");
                strcat(proc[dir_size]->stat_path, ent->d_name);

                if (!stat(proc[dir_size]->stat_path, &status_uid)) {
                    user = getpwuid(status_uid.st_uid);
                }

                strcat(proc[dir_size]->stat_path, "/stat");
                proc[dir_size]->stat_file = fopen(proc[dir_size]->stat_path, "r");

                if (!proc[dir_size]->stat_file) {
                    perror("No stat file");
                    cleanup(proc, dir_size, dir, &original);
                    return STAT_ERROR;
                }
                get_process_info(proc, &dir_size, stat_content, user);
            }
        }
        usleep(REFRESH_RATE);
        free_process_list(proc, dir_size);
    }
    closedir(dir);
    disable_raw_mode(&original);
    return SUCCESS;
}