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

int main(void){

    int id=1, fd;

    mkfifo("myfifo", 0777);

    for (int i=0 ; i<5 ; i++){
        if(id > 0) {
            id = fork();
        }
    }
    if(id == 0) {
        printf("1\n");
        fd = open("myfifo", O_WRONLY);
        printf("2\n");
        char *str = "fifo test succesfull!";
        printf("3\n");
        write(fd, str, 20*sizeof(char));
        printf("4\n");
    } else {
        fd = open("myfifo", O_RDONLY);
        printf("5\n");
        char *s;
        s = (char*) malloc(20*sizeof(char));
        read(fd, s, 20*sizeof(char));
        printf("6\n");
        printf("%s\n", s);
        printf("7\n");
    }

    return 0;
}