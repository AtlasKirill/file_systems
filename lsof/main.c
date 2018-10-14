#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>

const char directory_proc[] = "/proc/";

int main() {
    char directory_fd[sizeof("/proc/") + sizeof(char) * 5 + sizeof("/fd/") +
                      sizeof(char) * 10]; //pid max -- 5-digit number, inode max number is 10-digit number on ext 2/3/4
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
                unsigned int buffer_size = status_buffer.st_size + 1; // one for null-terminate symbol
                char *buffer = calloc(buffer_size, sizeof(char));//calloc makes string null-terminated
                readlink(directory_fd, buffer, buffer_size);
                printf("[pid] - %s, opened file - %s\n", proc_entry->d_name, buffer);
                free(buffer);
            }
        }
        closedir(fd_dir);
    }
    closedir(proc_dir);
    return 0;
}