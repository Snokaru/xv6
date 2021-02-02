
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
    int p_parent[2], p_child[2];
    char buf[1];
    pipe(p_parent);
    pipe(p_child);
    switch(fork()) {
        case -1:
            fprintf(2, "Fail on fork!\n");
        case 0: // in child
            read(p_parent[0], buf, 1);
            fprintf(1, "%d: received ping\n", getpid());
            write(p_child[1], " ", 1);
            exit(0);
            
        default: // in parent
            write(p_parent[1], " ", 1);
            read(p_child[0], buf, 1);
            fprintf(1, "%d: received pong\n", getpid());
    }
    close(p_parent[0]);
    close(p_parent[1]);
    close(p_child[0]);
    close(p_child[1]);
    exit(0);
}