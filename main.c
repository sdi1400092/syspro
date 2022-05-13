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

int main(void) {

	int i, id;
	id = fork();
	if(id == -1) {
		printf("fork not succesfull\n");
		return 1;
	}

	if(id == 0) {
		while(1){
			printf("1\n");
			sleep(2);
			printf("4\n");
			kill(getpid(), SIGSTOP);
		}
	} else {
		while(1) {
			printf("2\n");
			int status;
            waitpid(0, &status, WSTOPPED);
			printf("3\n");
			sleep(2);
			kill(id, SIGCONT);
		}
	}

	return 0;
}