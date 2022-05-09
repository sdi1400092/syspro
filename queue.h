
typedef struct queue{
    int processid;
    char fifoname[50];
    struct queue *next;
} Queue;

void queue_init(Queue **);
Queue queue_pop(Queue **);
void queue_push(Queue **, int, char *);
int queue_isempty(Queue *);
void print_node(Queue);
