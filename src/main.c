#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef enum {
    SUCCESS,
    PROC_OPEN_ERROR
} error_codes;

typedef struct {
    FILE *stat;
    pid_t pid;
} process_t;

int main(void)
{
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return PROC_OPEN_ERROR;
    } else {
        printf("Succesfully opened /proc\n");
    }

    struct dirent *ent = readdir(dir);
    while (ent) {
        bool valid_dir = 1;
        if (ent->d_type == DT_DIR) {
            for (int i = 0; i < strlen(ent->d_name); i++) {
                if (ent->d_name[i] < '0' || ent->d_name[i] > '9') {
                    valid_dir = 0;
                }
            }

            if (valid_dir) {
                printf("Found process: %s\n", ent->d_name);
            }
        }
        ent = readdir(dir);
    }

    closedir(dir);
}