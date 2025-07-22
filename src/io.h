/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#ifndef IO_H
#define IO_H

#include <dirent.h>
#include <pwd.h>
#include <stdbool.h>
#include <termios.h>
#include "types.h"

void enable_raw_mode(struct termios *original);

void disable_raw_mode(struct termios *original);

void enable_nonblocking_mode(void);

void disable_nonblocking_mode(void);

int check_exit(char *exit_buffer, int buff_len);

bool check_dir_validity(struct dirent *ent);

void get_sorting_criteria(char *buff, int buff_len, int *criteria);

void get_sorting_order(char *buff, int buff_len, int *order);

void get_signal(char *buff, int buff_len, int *signal);

void send_signal(char *signal_pid, int signal, struct termios original);

void get_process_info(process_t **proc, int *dir_size, char *stat_content, struct passwd *user);

void display_processes(process_t **proc, int dir_size);

void display_keys(void);

void refresh(void);

void show_all(process_t **proc, int dir_size, int criteria, int order);

#endif