/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
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
}

void enable_nonblocking_mode(void) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void disable_nonblocking_mode(void) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
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
                    *criteria = RSS;
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

void get_signal(char *buff, int buff_len, int *signal)
{
    if (buff_len > 0) {
        for (int i = 0; i < buff_len; i++) {
            switch (buff[i]) {
                case 'k':
                case 'K':
                    *signal = SIGKILL;
                    break;
                case 't':
                case 'T':
                    *signal = SIGTERM;
                    break;
                case 's':
                case 'S':
                    *signal = SIGSTOP;
                    break;
                case 'c':
                case 'C':
                    *signal = SIGCONT;
                    break;
            }
        }
    }
}

void send_signal(char *signal_pid, int signal, struct termios original)
{
    disable_raw_mode(&original);
    disable_nonblocking_mode();
    printf("PID to send signal to: ");
    fflush(stdout);

    if (fgets(signal_pid, EXIT_BUFFER_SIZE, stdin) != NULL) {
        enable_raw_mode(&original);
        enable_nonblocking_mode();
        pid_t pid = atoi(signal_pid);

        if (pid <= 0) {
            fprintf(stderr, "Invalid PID: %s\n", signal_pid);
            usleep(REFRESH_RATE);
            return;
        }

        if (kill(pid, signal) == -1) {
            perror("Failed to send signal");
            usleep(REFRESH_RATE);
        } else {
            printf("Signal sent to PID %d\n", atoi(signal_pid));
            usleep(REFRESH_RATE);
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
            case RSS:
                proc[*dir_size]->rss = atoi(stat_content) * sysconf(_SC_PAGESIZE);
                break;
        }
    }
    strcpy(proc[*dir_size]->user, user->pw_name);
    (*dir_size)++;
}

void display_processes(process_t **proc, int dir_size, int argc, char **argv)
{
    for (int i = 0; i < dir_size; i++) {
        if (argc == 2 && strcmp(proc[i]->user, argv[1])) {
            continue;
        }

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
        if (proc[i]->rss > 1024 * 1024) {
            if (proc[i]->rss > 1024 * 1024 * 512) {
                printf("\033[1;31m%.2lfMB\033[0m\t", proc[i]->rss * 1.0 / 1024 / 1024);
            } else if (proc[i]->rss > 1024 * 1024 * 256) {
                printf("\033[1;33m%.2lfMB\033[0m\t", proc[i]->rss * 1.0 / 1024 / 1024);
            } else {
                printf("%.2lfMB\t", proc[i]->rss * 1.0 / 1024 / 1024);
            }
            if (proc[i]->rss * 1.0 / 1024 / 1024 < 100) {
                printf("\t");
            }
        } else if (proc[i]->rss > 1024) {
            printf("%.2lfKB\t", proc[i]->rss * 1.0 / 1024);
            if (proc[i]->rss * 1.0 / 1024 < 100) {
                printf("\t");
            }
        } else {
            printf("%luB\t", proc[i]->rss);
            if (proc[i]->rss * 1.0 < 1000) {
                printf("\t");
            }
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
    printf("\nq: exit\t\t\ta: ascending\t\td: descending\n\n1: sort by PID\t\t2: sort by COMMAND\t3: sort by STATE\n"
        "4: sort by PPID\t\t5: sort by UTIME\t6: sort by STIME\n7: sort by NICE\t\t"
        "8: sort by START TIME\t9: sort by RAM\n0: sort by USER\n\nk: kill process\t\tt: terminate process\t"
        "s: suspend process\nc: continue process\n");
}

void refresh(void)
{
    printf("\033[H\033[J");
    printf("\033[30;47mPID\tCOMMAND\t\tSTATE\tPPID\tUTIME\tSTIME\tNICE\tSTART\t\tRAM\t\tUSER\n\033[0m");
}

void show_all(process_t **proc, int dir_size, int criteria, int order, int argc, char **argv)
{
    refresh();
    sort_processes(proc, dir_size, criteria, order);
    display_processes(proc, dir_size, argc, argv);
    display_keys();
    usleep(REFRESH_RATE);
}