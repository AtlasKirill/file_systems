#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


const char directory_proc[] = "/proc/";
const char stat[] = "/stat";

struct stat_file {
    int pid;
    char name[30];
    char status;
};

int main() {
    DIR *dir;
    FILE *file_status;
    struct dirent *entry;
    struct stat_file statFile;

    dir = opendir(directory_proc);
    if (!dir) {
        perror("diropen");
        exit(1);
    };

    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(entry->d_name[0]))
            continue;

        char *tmp_proc=malloc(sizeof(char)*sizeof(directory_proc));
        char *tmp_stat=malloc(sizeof(char)*sizeof(stat));

        strcpy(tmp_proc,directory_proc);
        strcpy(tmp_stat,stat);

        char *partly_way = strcat(tmp_proc, entry->d_name);
        char *way_to_stat = strcat(partly_way, tmp_stat);
        file_status = fopen(way_to_stat, "r");
        if (file_status == NULL) {
            perror("file open:");
            exit(1);
        }
        fscanf(file_status, "%d %s %c", &statFile.pid, statFile.name, &statFile.status);
        fclose(file_status);
        if (statFile.status == 'R') {
//            printf("%c\n", statFile.status);
            printf("PID - %s, name - %s \n",
                   entry->d_name,statFile.name );
        }
    };

    closedir(dir);
    return 0;
};