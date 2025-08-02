/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include "io.h"
#include "memory.h"
#include "sorting.h"
#include "types.h"

int main(int argc, char **argv)
{
    if (argc > 2) {
        printf("Usage: %s [user]\n", argv[0]);
        return ARGS_ERROR;
    }

    int criteria = PID, order = 1;
    DIR *dir = opendir("/proc");
    struct termios original;
    enable_raw_mode(&original);
    enable_nonblocking_mode();

    if (!dir) {
        perror("Failed to open /proc");
        return PROC_OPEN_ERROR;
    }

    while (1) {
        int dir_size = 0, buff_len = 0, signal = -1;
        process_t **proc = NULL;
        char exit_buffer[EXIT_BUFFER_SIZE], stat_content[STAT_LINE_LENGTH], signal_pid[EXIT_BUFFER_SIZE];
        struct dirent *ent = NULL;
        struct passwd *user = NULL;
        struct stat status_uid;

        buff_len = read(STDIN_FILENO, exit_buffer, EXIT_BUFFER_SIZE);        
        if (!check_exit(exit_buffer, buff_len)) {
            cleanup(proc, dir_size, dir, &original);
            return SUCCESS;
        }

        get_sorting_criteria(exit_buffer, buff_len, &criteria);
        get_sorting_order(exit_buffer, buff_len, &order);
        get_signal(exit_buffer, buff_len, &signal);

        if (signal != -1) {
            send_signal(signal_pid, signal, original);
        }

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

                strcpy(proc[dir_size]->stat_path, "/proc/");
                strcat(proc[dir_size]->stat_path, ent->d_name);

                if (!stat(proc[dir_size]->stat_path, &status_uid)) {
                    user = getpwuid(status_uid.st_uid);
                }

                strcat(proc[dir_size]->stat_path, "/stat");
                proc[dir_size]->stat_file = fopen(proc[dir_size]->stat_path, "r");

                if (!proc[dir_size]->stat_file) {
                    free(proc[dir_size]);
                    continue;
                }
                get_process_info(proc, &dir_size, stat_content, user);
            }
        }
        show_all(proc, dir_size, criteria, order, argc, argv);
        free_process_list(proc, dir_size);
    }
    closedir(dir);
    disable_raw_mode(&original);
    fflush(stdout);
    return SUCCESS;
}