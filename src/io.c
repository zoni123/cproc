/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "types.h"

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