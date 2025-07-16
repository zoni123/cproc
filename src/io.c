/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "types.h"
#include "sorting.h"

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

int check_exit(char *exit_buffer, int buff_len)
{
    if (buff_len > 0) {
        for (int i = 0; i < buff_len; i++) {
            if (exit_buffer[i] == 'q' || exit_buffer[i] == 'Q') {
                return SUCCESS;
            }
        }
    }
    return NO_EXIT;
}

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

void get_sorting_criteria(char *buff, int buff_len, int *criteria)
{
    if (buff_len > 0) {
        for (int i = 0; i < buff_len; i++) {
            switch (buff[i]) {
                case '1':
                    *criteria = PID;
                    break;
                case '2':
                    *criteria = COMMAND;
                    break;
                case '3':
                    *criteria = STATE;
                    break;
                case '4':
                    *criteria = PPID;
                    break;
                case '5':
                    *criteria = UTIME;
                    break;
                case '6':
                    *criteria = STIME;
                    break;
                case '7':
                    *criteria = NICE;
                    break;
                case '8':
                    *criteria = START_TIME;
                    break;
                case '9':
                    *criteria = VSIZE;
                    break;
                case '0':
                    *criteria = USER;
                    break;
            }
        }
    }
}

void get_sorting_order(char *buff, int buff_len, int *order)
{
    if (buff_len > 0) {
        for (int i = 0; i < buff_len; i++) {
            if (buff[i] == 'a' || buff[i] == 'A') {
                *order = 1;
            } else if (buff[i] == 'd' || buff[i] == 'D') {
                *order = -1;
            }
        }
    }
}

void get_process_info(process_t **proc, int *dir_size, char *stat_content, struct passwd *user)
{
    for (int i = 0; i < 40; i++) {
        fscanf(proc[*dir_size]->stat_file, "%s", stat_content);
        switch (i) {
            case PID:
                proc[*dir_size]->pid = atoi(stat_content);
                break;
            case COMMAND:
                strcpy(proc[*dir_size]->command, stat_content);
                while (stat_content[strlen(stat_content) - 1] != ')') {
                    if (fscanf(proc[*dir_size]->stat_file, "%s", stat_content) != 1) {
                        break;
                    }
                    strcat(proc[*dir_size]->command, " ");
                    strcat(proc[*dir_size]->command, stat_content);
                }
                break;
            case STATE:
                proc[*dir_size]->state = stat_content[0];
                break;
            case PPID:
                proc[*dir_size]->ppid = atoi(stat_content);
                break;
            case UTIME:
                proc[*dir_size]->utime = atoi(stat_content);
                break;
            case STIME:
                proc[*dir_size]->stime = atoi(stat_content);
                break;
            case NICE:
                proc[*dir_size]->nice = atoi(stat_content);
                break;
            case START_TIME:
                proc[*dir_size]->start_time = atoi(stat_content);
                break;
            case VSIZE:
                proc[*dir_size]->vsize = atoi(stat_content);
                break;
        }
    }
    strcpy(proc[*dir_size]->user, user->pw_name);
    (*dir_size)++;
}

void display_processes(process_t **proc, int dir_size)
{
    for (int i = 0; i < dir_size; i++) {
        printf("%d\t", proc[i]->pid);
        if (strlen(proc[i]->command) < 8) {
            printf("%.*s\t\t", TRUNC_CHAR, proc[i]->command);
        } else if (strlen(proc[i]->command) >= TRUNC_CHAR) {
            printf("%.*s...\t", TRUNC_CHAR, proc[i]->command);
        } else {
            printf("%.*s\t", TRUNC_CHAR, proc[i]->command);
        }
        printf("%c\t", proc[i]->state);
        printf("%d\t", proc[i]->ppid);
        printf("%lu\t", proc[i]->utime);
        printf("%lu\t", proc[i]->stime);
        printf("%d\t", proc[i]->nice);
        printf("%lu\t\t", proc[i]->start_time);
        if (proc[i]->vsize > 1024 * 1024) {
            printf("%luMB\t", proc[i]->vsize / 1024 / 1024);
        } else if (proc[i]->vsize > 1024) {
            printf("%luKB\t", proc[i]->vsize / 1024);
        } else {
            printf("%luB\t", proc[i]->vsize);
        }
        if (!strcmp(proc[i]->user, "root")) {
            printf("\033[1;35m%s\033[0m", proc[i]->user);
        } else {
            printf("%s", proc[i]->user);
        }
        printf("\n");
    }
}

void display_keys(void)
{
    printf("\nq: exit\t\t\ta: ascending\t\td: descending\n1: sort by PID\t\t2: sort by COMMAND\t3: sort by STATE\n"
        "4: sort by PPID\t\t5: sort by UTIME\t6: sort by STIME\n7: sort by NICE\t\t"
        "8: sort by START TIME\t9: sort by VSIZE\n0: sort by USER\n");
}

void refresh(void)
{
    printf("\033[H\033[J");
    printf("PID\tCOMMAND\t\tSTATE\tPPID\tUTIME\tSTIME\tNICE\tSTART\t\tVSIZE\tUSER\n");
}

void show_all(process_t **proc, int dir_size, int criteria, int order)
{
    refresh();
    sort_processes(proc, dir_size, criteria, order);
    display_processes(proc, dir_size);
    display_keys();
    usleep(REFRESH_RATE);
}