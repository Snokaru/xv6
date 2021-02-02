#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void 
execute_process(int read_fd) {

    // read number from left pipe
    int n;
    read(read_fd, &n, 1);
    printf("prime %d\n", n);

    int p[2];
    int not_in_child = 1, neighbour_already_spawned = 0;
    
    int next;
    while(read(read_fd, &next, 1) && not_in_child) {
        if (next % n != 0) { // spawn another process
            if (neighbour_already_spawned) {
                write(p[1], &next, 1);
            }
            else {
                pipe(p);
                switch(fork()) {
                    default:
                        neighbour_already_spawned = 1;
                        close(p[0]); // you only need write part in parent
                        write(p[1], &next, 1);
                        break;
                    case 0: // in child
                        not_in_child = 0;
                        close(p[1]);
                        execute_process(p[0]);
                        break;
                }
            }
        }
    }
    int status;
    if (neighbour_already_spawned) {
        close(p[1]); 
        wait(&status);
    }
    close(read_fd);
    exit(0); 

}

int
main(int argc, char* argv[]) {
    int p[2];
    pipe(p);

    switch(fork()) {
        default:
            close(p[0]);
            for (int i = 2; i <= 35; i++) {
                write(p[1], &i, 1);
            }
            close(p[1]);
            break;
        case 0:
            close(p[1]);
            execute_process(p[0]);
    }
    int status;
    wait(&status);
    exit(0);
}