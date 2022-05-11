#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
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

int main(void){

    int cid=12, id = fork();
    if(id == 0) {
        cid = getpid();
        printf("child id = %d, parent's id = %d\n", getpid(), getppid());
    } else {
        printf("parent id = %d \n", getpid());
    }

    if(id > 0) {
        sleep(3);
        if(cid == getpid()) printf("who the fuck knows what going on!!!!\n");
    }

    return 0;
}