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

void enable_raw_mode(struct termios *original) {
    struct termios raw;

    tcgetattr(STDIN_FILENO, original);
    raw = *original;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void disable_raw_mode(struct termios *original) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, original);
}

void free_process(process_t *proc)
{
    if (proc) {
        if (proc->stat) {
            fclose(proc->stat);
        }
        if (proc->stat_path) {
            free(proc->stat_path);
        }
        free(proc);
    }
}

void free_process_list(process_t **proc, int size)
{
    if (proc) {
        for (int i = 0; i < size; i++) {
            free_process(proc[i]);
        }
        free(proc);
    }
}

int check_exit(char *exit_buffer, int buff_len, DIR *dir, struct termios *original)
{
    if ((buff_len = read(STDIN_FILENO, exit_buffer, EXIT_BUFFER_SIZE))) {
        for (int i = 0; i < buff_len; i++) {
            if (exit_buffer[i] == 'q' || exit_buffer[i] == 'Q') {
                return SUCCESS;
            }
        }
    }
    return NO_EXIT;
}

void get_process_info(process_t **proc, int *dir_size, char *stat_content, struct passwd *user)
{
    for (int i = 0; i < 40; i++) {
        if (i == 0 || i == 1 || i == 2 || i == 3 || i == 13 || i == 14 || i == 17 || i == 22 || i == 23) {
            fscanf(proc[*dir_size]->stat, "%s", stat_content);
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
    printf("%s", user->pw_name);
    printf("\n");
    (*dir_size)++;
}

bool check_dir_validity(struct dirent *ent)
{
    if (ent->d_type == DT_DIR) {
        for (int i = 0; i < strlen(ent->d_name); i++) {
            if (ent->d_name[i] < '0' || ent->d_name[i] > '9') {
                return false;
            }
        }
        return true;
    }
    return false;
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

        if (!check_exit(exit_buffer, buff_len, dir, &original)) {
            disable_raw_mode(&original);
            closedir(dir);
            free_process_list(proc, dir_size);
            return SUCCESS;
        }

        printf("\033[H\033[J");
        printf("PID\tCOMMAND\t\tSTATE\tPPID\tUTIME\tSTIME\tPRIOR\tSTART\tVSIZE\tUSER\n");

        rewinddir(dir);
        while (ent = readdir(dir)) {
            if (check_dir_validity(ent)) {
                proc = realloc(proc, sizeof(process_t *) * (dir_size + 1));

                if (!proc) {
                    perror("Malloc failed");
                    closedir(dir);
                    disable_raw_mode(&original);
                    return MALLOC_FAILED;
                }

                proc[dir_size] = malloc(sizeof(process_t));
                if (!proc[dir_size]) {
                    perror("Malloc failed");
                    free_process_list(proc, dir_size);
                    closedir(dir);
                    disable_raw_mode(&original);
                    return MALLOC_FAILED;
                }

                proc[dir_size]->pid = atoi(ent->d_name);
                proc[dir_size]->stat_path = malloc(strlen("/proc/") + strlen(ent->d_name) + strlen("/stat") + 1);

                if (!proc[dir_size]->stat_path) {
                    perror("Malloc failed");
                    free_process_list(proc, dir_size);
                    closedir(dir);
                    disable_raw_mode(&original);
                    return MALLOC_FAILED;
                }

                strcpy(proc[dir_size]->stat_path, "/proc/");
                strcat(proc[dir_size]->stat_path, ent->d_name);

                if (!stat(proc[dir_size]->stat_path, &status_uid)) {
                    user = getpwuid(status_uid.st_uid);
                }

                strcat(proc[dir_size]->stat_path, "/stat");
                proc[dir_size]->stat = fopen(proc[dir_size]->stat_path, "r");

                if (!proc[dir_size]->stat) {
                    perror("No stat file");
                    free_process_list(proc, dir_size);
                    closedir(dir);
                    disable_raw_mode(&original);
                    return STAT_ERROR;
                }
                get_process_info(proc, &dir_size, stat_content, user);
            }
        }
        usleep(REFRESH_RATE);
        free_process_list(proc, dir_size);
    }
    disable_raw_mode(&original);
    closedir(dir);
    return SUCCESS;
}