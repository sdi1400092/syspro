#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int main(void){

    char *str = "kaka/ MOVED_TO lalala.txt", *tempstr;
    int i=0;

    tempstr = (char *) malloc(sizeof(str));

    while (str != "\0"){

        if(isspace(str[i]) == 0){
            str++;
        } else {
            tempstr = str;
            str++;
            printf("1\n");
        }
    }

    printf("%s\n", ++tempstr);


    return 0;
}