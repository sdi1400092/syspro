#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main(void){
    int i;
    char *str1 = "la", *str2 = "lala", *str3 = "lalala", *str4 = "lalalala";
    Queue *Q;
    queue_init(&Q);
    queue_push(&Q, 1, str1);
    queue_push(&Q, 2, str2);
    Queue temp = queue_pop(&Q);
    print_node(temp);
    temp = queue_pop(&Q);
    print_node(temp);
    if(queue_isempty(Q)){
        printf("empty\n");
    } else {
        printf("not emtpy\n");
    }
    queue_push(&Q, 3, str3);
    if(queue_isempty(Q)){
        printf("empty\n");
    } else {
        printf("not emtpy\n");
    }
    temp = queue_pop(&Q);
    print_node(temp);

    return 0;
}