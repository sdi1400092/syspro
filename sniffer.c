#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>
#include <ctype.h>
#include <fcntl.h>
#include "queue.h"


char *fifoname(int i){
    
    // a function turning fifo'i' into fifo1 etc...

    char *s = "fifo";

    int len = strlen(s);

    char *str = malloc(len + i/10 + 2);

    str = realloc(str, len + i/10 + 2);
    
    if(i/10 == 0){
        // for i with 1 digit
        
        strcpy(str, s);
        str[len] = i + '0';
        str[len+1] = '\0';

    } else if (i/10 < 10){
        // for i with 2 digits
        
        strcpy(str, s);
        str[len] = i/10 + '0';
        str[len+1] = i%10 + '0';
        str[len+2] = '\0';
    
    } else if (i/10 < 100){
        // for numbers with three digits
    
        strcpy(str, s);
        str[len] = i/100 + '0';
        str[len+1] = (i%100)/10 + '0';
        str[len+2] = i%10 + '0';
        str[len+3] = '\0';
    
    }
    
    return str;

}


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

        //getting number of existing files in directory
        char *la[] = {">>", "lala.txt", NULL};
        system("ls kaka | wc -w > wcout.txt");
        
        FILE *fp;
        fp = fopen("wcout.txt", "r");
        char p = fgetc(fp);
        int i = atoi(&p);

        // creating as many workers as the existing files in directory
        int id2 = getppid();
        printf("id2 = %d\n", id2);
        Queue *Q;
        queue_init(&Q);
        for(int j=1 ; j<i ; j++){
            if(id2 > 0) {
                printf("trying to create worker\n");
                id2 = fork();
                if(id2 = -1) {
                    printf("process: %d creation of worker failed\n", getpid());
                    return 5;
                } else {
                    printf("worker created\n");
                }

                // // creating name pipe(fifo) for the new child
                // char *fifo = fifoname(j+1);
                // mkfifo(fifo, 0666);
                // if (id2 > 0){
                //     // manager will wait for the child's id to put it inside the queue
                //     // alongside the name of the named pipe for that process
                //     printf("manager\n");

                //     int fifofd = open(fifo, O_RDONLY);
                //     if (fifofd == -1) printf("opening fifo not succesfull\n");
                //     printf("lala\n");
                //     int childid;
                //     read(fifofd, &childid, sizeof(int));
                //     printf("after read %s\n", fifo);
                //     close(fifofd);
                //     queue_push(&Q, childid, fifo);

                // } else if(id2 = 0) {
                //     // child process will get its id write it to the named pipe just created for it
                //     printf("worker\n");

                //     int tempid = getpid();
                //     int fifofd = open(fifo, O_WRONLY);
                //     if (fifofd == -1) printf("opening fifo not succesfull\n");
                //     write(fifofd, &tempid, sizeof(int));
                //     printf("after write %d\n", tempid);
                //     close(fifofd);

                // }
            }
        }


        // waiting for a change in dir from listener
        char *buffer;
        char *FileName;
        char ch = ' ';
        char *str;

        if (id2 > 0) {
            while(1){
                printf("1\n");
                int n = fcntl(fd[0], F_GETPIPE_SZ);
                printf("2\n");
                read(fd[0], buffer, n);
                printf("3\n");
                printf("buffer = %s\n", buffer);
                printf("4\n");
                FileName = strrchr(str, ch);
                printf("5\n");
                printf("%s\n", FileName);
                printf("6\n");
            };
        }

        close(fd[0]);

        // creating workers and setting them to sleep while no change in dir

    } else {
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
        
        // duplicating stdout of listener to the pipe
        if(dup2(fd[1], STDOUT_FILENO) == -1){
            printf("dup2 not succesfull!\n");
            return 4;
        }
        
        // exec system call for inotifywait
        execvp("inotifywait", arr);
        
        // system("inotifywait -mq -e create -e moved_to kaka > fd[1]");
        
        close(fd[1]);

    }

    return 0;
}