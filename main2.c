#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void){
    
    int i=1;
    char *s = "myfifo";

    int len = strlen(s);

    char *str = malloc(len + i/10 + 2);
    for(i = 1 ; i<=256 ; i++){
        str = realloc(str, len + i/10 + 2);
        if(i/10 == 0){
            strcpy(str, s);
            str[len] = i + '0';
            str[len+1] = '\0';
            printf("%s\n", str);
        } else if (i/10 < 10){
            strcpy(str, s);
            str[len] = i/10 + '0';
            str[len+1] = i%10 + '0';
            str[len+2] = '\0';
            printf("%s\n", str);
        } else if (i/10 < 100){
            // for numbers with three digits
            strcpy(str, s);
            str[len] = i/100 + '0';
            str[len+1] = (i%100)/10 + '0';
            str[len+2] = i%10 + '0';
            str[len+3] = '\0';
            printf("%s\n", str);
        }
        
    }

    return 0;
}