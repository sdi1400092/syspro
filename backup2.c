#define _GNU_SOURCE

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

char *afterspace(char *str){
    

    char *tempstr, ch;
    int i=0;
    tempstr = (char *) malloc(sizeof(str));

    printf("%s\n", str);

    while (str != "\0"){

        if(isspace(str[i]) == 0){
            str++;
        } else {
            tempstr = str;
            str++;
            printf("1\n");
        }
    }
    printf("end of while\n");

    printf("lalalala %s\n", tempstr);

    return ++tempstr;

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

        Queue *Q;
        Queue Qitem;
        queue_init(&Q);
        int n = fcntl(fd[0], F_GETPIPE_SZ);
        Qitem.processid = -1;

        // waiting for a change in dir from listener
        printf("waiting for changes in dir...\n");

        char *buffer;
        char *FileName;
        char ch = ' ';
        char *str;
        buffer = (char *) malloc(n*sizeof(char));
        FileName = (char *) malloc(sizeof(buffer));
        int id2 = getpid(), j=0, fifofd;

        while(1){

            if(read(fd[0], buffer, n) > 0){

                if(id2 > 0) {
                    FileName = strrchr(buffer, ch);
                    printf("file name to be opened by worker--->%s", ++FileName);
                }

                // if the Q is empty and we are on the parent process (still the manager)
                // then create a new worker and push him into the Q
                if (queue_isempty(Q) && id2 >0) {
                    // create a new worker since no worker is available to work 
                        
                    char *newfifo;
                    newfifo = fifoname(++j);
                    mkfifo(newfifo, 0777);

                    id2 = fork();
                    if(id2 == -1) {
                        printf("something went wrong with fork for worker\n");
                        exit(EXIT_FAILURE);
                    } else if(id2 == 0) {

                        int tempid = getpid();
                        fifofd = open(newfifo, O_WRONLY);

                        if (fifofd == -1) printf("opening fifo not succesfull\n");
                            
                        write(fifofd, &tempid, sizeof(int));
                        close(fifofd);

                            char *file;
                            file = (char *) malloc(n*sizeof(char));

                            fifofd = open(newfifo, O_RDONLY);
                            if(fifofd == -1) printf("opening fifo failed\n");
                            read(fifofd, file, n*sizeof(char));
                            read(fifofd, &(Qitem.processid), sizeof(int));
                            close(fifofd);
                        
                            if(Qitem.processid == getpid()) {

                                // check if the worker we popped from the queue is the process we are currently on
                                while(1) {

                                        // open the FileName and find Links...
                                        char *str;
                                        str = (char *) malloc(n*sizeof(char));
                                        strcpy(str, "./kaka/");
                                        strcat(str, file);
                                        int z = 0; 
                                        while(str != "\0"){
                                            if(str[z] != '\n'){
                                                z++;
                                            } else {
                                                str[z] = '\0';
                                                break;
                                            }
                                        }
                                        int filefd = open(str, O_RDONLY);
                                        if(filefd == -1) printf("opening file failed\n");
                                        // Find the URLs from the file... and delete break;
                                        break;
                                        
                                }


                            }

                    } else {

                        int fifofd = open(newfifo, O_RDONLY), childid;

                        if (fifofd == -1) printf("opening fifo not succesfull\n");

                        read(fifofd, &childid, sizeof(int));
                        close(fifofd);
                        queue_push(&Q, childid, newfifo);

                    }
                }

                //  Pop the first available worker from Q and send him the FileName for him to do its thing
                Qitem = queue_pop(&Q);          

                if(id2 > 0) {

                    fifofd = open(Qitem.fifoname, O_WRONLY);
                    if(fifofd == -1) printf("opening fifo failed\n");
                    write(fifofd, FileName, n*sizeof(char));
                    write(fifofd, &(Qitem.processid), sizeof(int));
                    close(fifofd);

                }

            } // if read >0

        } // while

        close(fd[0]);

        // creating workers and setting them to sleep while no change in dir

    } else {
        //listener
        
        
        close(fd[0]);

        // array of arguments for exec system call
        char *arr[] = {
            "inotifywait",
            "-m",
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

    }

    close(fd[1]);

    return 0;
}