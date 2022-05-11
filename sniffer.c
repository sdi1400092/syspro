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
        queue_init(&Q);

        //getting number of existing files in directory
        char *la[] = {">>", "lala.txt", NULL};
        system("ls kaka | wc -w > wcout.txt");
        
        FILE *fp;
        fp = fopen("wcout.txt", "r");
        char p = fgetc(fp);
        int i = atoi(&p);

        // creating as many workers as the existing files in directory
        int id2 = getppid(), tempid, fifofd;
        for(int j=0 ; j<i ; j++){                
            
            char *fifo = fifoname(j+1);
            mkfifo(fifo, 0777);
            
            if(id2 > 0) {
                id2 = fork();
                if(id2 == 0) {
                    // child process will get its id write it to the named pipe just created for it

                    tempid = getpid();
                    fifofd = open(fifo, O_WRONLY);

                    if (fifofd == -1) printf("opening fifo not succesfull\n");
                    
                    write(fifofd, &tempid, sizeof(int));
                    close(fifofd);
                    
                    break; // only need one time for each child                    
                } else if(id2 == -1) {
                    printf("process: %d creation of worker failed\n", getpid());
                    exit(EXIT_FAILURE);
                }
            }
        }

        // opening each fifo file from the manager process so that the manager
        // can read the worker's id from the fifo
        int childid;
        if(id2 > 0) {

            // still the manager            
            int j;
            for(j=0 ; j<i ; j++){
                char *fifo = fifoname(j+1);
                
                // manager will wait for the child's id to push it into the queue
                // alongside the name of the named pipe dedicated to that process
                fifofd = open(fifo, O_RDONLY);

                if (fifofd == -1) printf("opening fifo not succesfull\n");

                read(fifofd, &childid, sizeof(int));
                close(fifofd);
                queue_push(&Q, childid, fifo);

            }

            // waiting for a change in dir from listener
            printf("waiting for changes in dir...\n");

            char *buffer;
            char *FileName;
            char ch = ' ';
            char *str;
            int n = fcntl(fd[0], F_GETPIPE_SZ), id3 = id2;
            buffer = (char *) malloc(n*sizeof(char));
            FileName = (char *) malloc(sizeof(buffer));

            while(1){

                if(read(fd[0], buffer, n) > 0){
                    

                    FileName = strrchr(buffer, ch);
                    printf("file name to be opened by worker--->%s", ++FileName);

                    // if the Q is empty and we are on the parent process (still the manager)
                    // then create a new worker and push him into the Q
                    if (queue_isempty(Q) && id3 >0) {
                        // create a new worker since no worker is available to work 
                        
                        char *newfifo;
                        newfifo = fifoname(++j);
                        mkfifo(newfifo, 0777);

                        id3 = fork();
                        if(id3 == -1) {
                            printf("something went wrong with fork for worker\n");
                            exit(EXIT_FAILURE);
                        } else if(id3 == 0) {

                            tempid = getpid();
                            fifofd = open(newfifo, O_WRONLY);

                            if (fifofd == -1) printf("opening fifo not succesfull\n");
                            
                            write(fifofd, &tempid, sizeof(int));
                            close(fifofd);

                        } else {

                            fifofd = open(newfifo, O_RDONLY);

                            if (fifofd == -1) printf("opening fifo not succesfull\n");

                            read(fifofd, &childid, sizeof(int));
                            close(fifofd);
                            queue_push(&Q, childid, newfifo);

                        }
                    }

                    // Pop the first available worker from Q and send him the FileName for him to do its work
                    Queue Qitem;
                    Qitem = queue_pop(&Q);

                    printf("going to open fifo %s\n", Qitem.fifoname);
                    fifofd = open(Qitem.fifoname, O_WRONLY);
                    if(fifofd == -1) printf("opening fifo failed\n");
                    write(fifofd, FileName, sizeof(FileName));
                    close(fifofd);

                    // check if the worker we poped from the list is the process we are currently on
                    if (Qitem.processid = getpid()){
                        // search open the FileName and find Links...
                        char *file;
                        file = (char *) malloc(n*sizeof(char));
                        printf("worker reporting for duty\n");

                        fifofd = open(Qitem.fifoname, O_RDONLY);
                        if(fifofd == -1) printf("opening fifo failed\n");
                        read(fifofd, file, n*sizeof(char));
                        close(fifofd);

                        printf("done with the fifo\n");
                        int filefd = open(file, O_RDONLY);
                        if(filefd == -1) printf("opening file failed\n");
                        printf("done with opening file\n");
                    }
                }

            }

        }

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