#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>


void signal_handler(int foo) {
    printf("\nterminated by user\n");
    kill(0, SIGKILL);
}

int main(void) {

    int id = fork(), id2;

    if(id > 0) {
        signal(SIGINT, signal_handler);

        while(1){
            for(int j=0 ; j<200 ; j++) {
                id2 = fork();
                if(id2==0) {
                    printf("worker\n");
                    kill(getpid(), SIGSTOP);
                } else {
                    printf("parent\n");
                }
            }
        }
    } else {
        while(1) {
            printf("child\n");
        }
    }

    return 0;
}