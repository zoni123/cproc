/* (c) 2025 Sebastian-Marian Badea - GPLv3 License */
#include <dirent.h>
#include <stdlib.h>
#include <termios.h>
#include "io.h"
#include "memory.h"

void free_process(process_t *proc)
{
    if (proc) {
        if (proc->stat_file) {
            fclose(proc->stat_file);
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

void cleanup(process_t **proc, int dir_size, DIR *dir, struct termios *original)
{
    free_process_list(proc, dir_size);
    closedir(dir);
    disable_raw_mode(original);
}