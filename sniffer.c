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

typedef struct {
    char *url;
    int counter;
} urls;

// a function that gets an int and adds it as text next to the string "fifo"
// i.e. if(i=5) return value--> fifo5
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

// a function that returns the string after the last space of a given string
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

// a function the opens file <filename> and gets all of its urls
// along with the counter of how many times each shows up
// at the end the function creates a file with the name of the original opened file
// and .out at the end and puts all of the urls, with how many they appeared in the file, to it
void geturls(char *filename){

	int i = 0, fd, b, flag;
	char *str, *ch;
	ch = (char *) malloc(sizeof(char));
	// char *arr[1000];
    urls arr[1000];

    for(int a=0 ; a<1000 ; a++){
        arr[a].counter = 0;
    }

	fd = open(filename, O_RDONLY);
    if(fd == -1) printf("get urls: opening file failed\n");

    // finding urls and storing them into an array of a struct
    // with one member of the the url and one with how many times it was found in the file
	while(1) {
		if(read(fd, ch, sizeof(char)) > 0) {
			str = (char *) malloc(100*sizeof(char));
			if(ch[0] == 'h'){
				strcat(str, ch);
				if(read(fd, ch, sizeof(char)) > 0) {
					if(ch[0] == 't') {
						strcat(str, ch);
						if(read(fd, ch, sizeof(char)) > 0){
							if(ch[0] == 't') {
								strcat(str, ch);
								if(read(fd, ch, sizeof(char)) > 0) {
									if(ch[0] == 'p') {
										strcat(str, ch);
										if(read(fd, ch, sizeof(char)) > 0) {
											while(ch[0] != ' ' && ch[0] != '\n'){
												strcat(str, ch);
												read(fd, ch, sizeof(char));
											}
                                            flag = 1;
                                            b = 0;
                                            while(arr[b].url != NULL) {
                                                if(!strcmp(arr[b].url, str)) {
                                                    flag = 0;
                                                    break;
                                                }
                                                b++;
                                            }
                                            if(flag) {
                                                arr[i].url = str;
                                                arr[i].counter++;
                                                i++;
                                            } else {
                                                arr[b].counter++;
                                            }
										}
									}
								}
							}
						}
					}
				}
			}
		} else break;
	}
	printf("\n");
	close(fd);
	free(str);
    free(ch);
    
	int j = 0;
	i = 0;
	char *temp;

    // getting the location part of the url
	while(arr[i].url != NULL) {

		temp = (char *) malloc(200*sizeof(char));

		strcpy(temp, arr[i].url);
        temp = strchr(arr[i].url, '/');
        temp += 2;

        if(temp[0] == 'w'){
            temp += 4;
        }

        j = 0;
        while(temp[j] != '/') {
            j++;
        }

        temp[j] = '\0';
        arr[i].url = temp;

        i++;
	}

    i = -1;
	while(arr[i+1].url != NULL) {
        printf("%s %d\n", arr[i+1].url, arr[i+1].counter);
        i++;
    }
    if(i == -1) printf("no urls found\n");


    filename += strlen("./kaka/");
    strcat(filename, ".out");
    printf("%s\n", filename);

    fd = open(filename, O_WRONLY | O_CREAT);
    i = 0;
    while(arr[i].url != NULL) {
     
        write(fd, arr[i].url, sizeof(arr[i].url));
        write(fd, " ", sizeof(char));
        write(fd, &(arr[i].counter), sizeof(int));
        write(fd, "\n", sizeof(char));
    }

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
                    FileName++;
                }

                // if the Q is empty and we are on the parent process (still the manager)
                // then create a new worker and push him into the Q
                if (queue_isempty(Q) && id2 >0) {

                    // create a new worker since all other workers are busy
                    char *newfifo;
                    newfifo = fifoname(++j);
                    mkfifo(newfifo, 0777);

                    id2 = fork();
                    if(id2 == -1) {
                        printf("something went wrong with fork for worker\n");
                        exit(EXIT_FAILURE);
                    } else if(id2 == 0) {

                        // open fifo and put workers pid inside so the manager can push it into the queue
                        // with the available workers...
                        int tempid = getpid();
                        fifofd = open(newfifo, O_WRONLY);

                        if (fifofd == -1) printf("opening fifo not succesfull\n");
                            
                        write(fifofd, &tempid, sizeof(int));
                        close(fifofd);


                            // open fifo and check if the id of the process the manager wanted to open
                            // is this one also read the name of the file that has to be opened
                            char *file;
                            file = (char *) malloc(n*sizeof(char));

                            fifofd = open(newfifo, O_RDONLY);
                            if(fifofd == -1) printf("opening fifo failed\n");
                            read(fifofd, file, n*sizeof(char));
                            read(fifofd, &(Qitem.processid), sizeof(int));
                            close(fifofd);
                        
                            if(Qitem.processid == getpid()) {

                                char *str;
                                str = (char *) malloc(n*sizeof(char));

                                // check if the worker we popped from the queue is the process we are currently on
                                while(1) {

                                        // add the path of the directory my files are in
                                        // so that when i try to open it, it won't return -1
                                        // char *str;
                                        // str = (char *) malloc(n*sizeof(char));
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
                                        // call of function geturls() (summary of geturls' function in its definition)
                                        geturls(str);

                                        break;

                                        // kill(getpid(), SIGSTOP);
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
                kill(Qitem.processid, SIGCONT);

                if(id2 > 0) {

                    // open the fifo corresponding to the process that was popped from the queue
                    // and write filename to be opened inside and the id of the worker that will work...
                    fifofd = open(Qitem.fifoname, O_WRONLY);
                    if(fifofd == -1) printf("opening fifo failed\n");
                    write(fifofd, FileName, n*sizeof(char));
                    write(fifofd, &(Qitem.processid), sizeof(int));
                    close(fifofd);

                    int status;
                    // waitpid(Qitem.processid, &status, WSTOPPED);

                    // queue_push(&Q, Qitem.processid, Qitem.fifoname);

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