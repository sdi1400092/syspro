#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>




int main(void){
    
    int fd[2], id;
    if (pipe(fd) == -1) {
        printf("Opening pipe was not succesfull\n");
        return 1;
    };

    id = fork();
    if (id == -1){
        printf("creating listener was not succesfull\n");
        return 2;
    }

    if (id > 0){
        // manager

        close(fd[1]);

        // waiting for a change in dir from listener
        char *buffer;
        while(read(fd[0], buffer, 20*sizeof(char)) > 0){
            printf("%s\n", buffer);
        };

        close(fd[0]);

        // creating workers and setting them to sleep while no change in dir

    }
    else{
        //listener
        close(fd[0]);

        // array of arguments for exec system call
        char *arr[] = {
            "-mq",
            "-e",
            "create",
            "-e",
            "moved_to",
            "kaka",
            NULL
        };
        // exec system call for inotifywait

        dup2(fd[1], 1);

        execvp("inotifywait", arr);

        close(fd[1]);

    }

    return 0;
}