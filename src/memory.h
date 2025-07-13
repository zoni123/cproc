/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#ifndef MEMORY_H
#define MEMORY_H

#include <dirent.h>
#include <termios.h>
#include "types.h"

void free_process(process_t *proc);

void free_process_list(process_t **proc, int size);

void cleanup(process_t **proc, int dir_size, DIR *dir, struct termios *original);

#endif