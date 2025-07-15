/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#ifndef IO_H
#define IO_H

#include <termios.h>
#include <pwd.h>
#include "types.h"

void enable_raw_mode(struct termios *original);

void disable_raw_mode(struct termios *original);

int check_exit(char *exit_buffer, int buff_len);

void get_sorting_criteria(char *buff, int buff_len, int *criteria);

void get_process_info(process_t **proc, int *dir_size, char *stat_content, struct passwd *user);

void display_processes(process_t **proc, int dir_size);

void display_keys(void);

#endif