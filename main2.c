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

	char *str;
	str = (char *) malloc(20*sizeof(char));

	while(1) {
		str = (char *) malloc(20*sizeof(char));
		strcpy(str, "lala");
		strcpy(str, "aa");
	}

	return 0;
}