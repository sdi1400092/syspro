#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"




void queue_init(Queue **Q){
    (*Q) = NULL;
}

Queue queue_pop(Queue **Q){
    Queue *temp;
    temp = *Q;
    (*Q) = (*Q)->next;
    return *temp;
}

void queue_push(Queue **Q, int id, char *name){
    Queue *temp, *newitem;
    newitem = (Queue *) malloc (sizeof(Queue));
    strcpy(newitem->fifoname, name);
    newitem->processid = id;
    newitem->next = NULL;
    if ((*Q) == NULL){
        (*Q) = newitem;
    } else {
        temp = (*Q);
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = newitem;
    }
}

int queue_isempty(Queue *Q){
    if(Q == NULL) return 1;
    else return 0;
}

void print_node(Queue Q){
    printf("process id = %d and and name: %s\n", Q.processid, Q.fifoname);
}