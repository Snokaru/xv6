/* 
*  Usage: find <directory> <file> 
*  Example: find /a/b file
*
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUF_SIZE 512

char* get_filename(char* path) {
    char* p;
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

char* get_new_path(char* path, char* file) {
    //printf("path: %s file: %s\n", path, file);
    char* resulting_path = (char*)malloc(sizeof(char) * (strlen(path) + strlen(file) + 2));
    resulting_path[0] = '\0';
    strcpy(resulting_path, path);
    //printf("resulting_path: %s\n", resulting_path);
    int path_length = strlen(resulting_path);
    //printf("path_length: %d\n", path_length);
    resulting_path[path_length] = '/';
    strcpy(resulting_path + path_length + 1, file);
    //printf("resulting path: %s\n", resulting_path);
    return resulting_path;
}

int find(char* file, char* cumulated_path) {
    int fd;
    struct stat st;
    struct dirent de;

    if ((fd = open(".", 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", ".");
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", ".");
        return -1;
    }

    if (st.type == T_FILE || st.type == T_DEVICE) {
        return -1;
    }

    while (read(fd, (void*) &de, sizeof(de)) == sizeof(de)) {
        // invalid inum
        if (de.inum == 0) {
            //printf("bad inum!\n");
            continue;
        }
        // found file
        if (strcmp(file, de.name) == 0) {
            char* new_path = get_new_path(cumulated_path, file);
            printf("%s\n", new_path);
            free(new_path);
            continue;
        }
        // is dot file
        if (strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0) {
            //printf("is . or ..\n");
            continue;
        }
        char* new_path = get_new_path(cumulated_path, de.name);        
        //printf("new path: %s\n", new_path);
        if (chdir(de.name) < 0) {
            //printf("cant go in subdir\n");
            continue;
        }
        find(file, new_path);
        chdir("..");
        free(new_path);
    }
    return 0;
}

int main(int argc, char** argv) {
    chdir(argv[1]);
    find(argv[2], argv[1]);
    exit(0);
}