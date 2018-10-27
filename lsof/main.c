#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

const char directory_proc[] = "/proc/";

int main() {
    errno=0;

    char directory_fd[64];
    DIR *proc_dir;
    struct dirent *proc_entry;

    DIR *fd_dir;
    struct dirent *fd_entry;

    struct stat status_buffer;


    proc_dir = opendir(directory_proc);
    if (!proc_dir) {
        perror("proc_dir_open");
        exit(1);
    };
    while ((proc_entry = readdir(proc_dir)) != NULL) {
        if(proc_entry==NULL){
            perror("readdir");
            exit(1);
        };
        if (!isdigit(proc_entry->d_name[0]))
            continue;
        snprintf(directory_fd, sizeof(directory_fd), "/proc/%s/fd/", proc_entry->d_name);

        fd_dir = opendir(directory_fd);
        if (fd_dir) {
            while ((fd_entry = readdir(fd_dir)) != NULL) {
                if (!isdigit(fd_entry->d_name[0]))
                    continue;
                snprintf(directory_fd, sizeof(directory_fd), "/proc/%s/fd/%s", proc_entry->d_name, fd_entry->d_name);
                lstat(directory_fd, &status_buffer);

                if (S_ISDIR(status_buffer.st_mode))
                    continue;
                if (status_buffer.st_uid != getuid())
                    continue;

                unsigned int buffer_size = 1024;
                char *buffer = calloc(buffer_size, sizeof(char));//calloc makes string null-terminated
                readlink(directory_fd, buffer, buffer_size);
                printf("[pid] - %s, opened file - %s\n", proc_entry->d_name, buffer);
                free(buffer);
            }
            if (errno != 0){
                printf("Error in reading dir /proc/%s/fd/\n",proc_entry->d_name);
                perror("readdir");
//                exit(1);
            }

        }
        closedir(fd_dir);
    }

    if (errno != 0){
        printf("Error in reading dir /proc/\n");
        perror("readdir");
//        exit(1);
    }
    closedir(proc_dir);
    return 0;
}