#ifndef QUEUE_H
#define QUEUE_H

/* Represents a node in a queue */
typedef struct node
{
    void *item; // data contained in node
    struct node *next; // next item in the queue
} NODE;

/* Represents a queue */
typedef struct queue
{
    NODE *head; // head of queue
    NODE *tail; // tail of queue
    int size;   // size of queue
} QUEUE;

/*
 * Creates an empty queue.
 *
 * Input:
 * None
 *
 * Output:
 * A pointer to the created QUEUE. NULL pointer
 * if the operation fails.
 */ 
QUEUE *queueCreate(void);

/*
 * Adds and item to the back of the queue.
 *
 * Input:
 * Queue *: pointer to a QUEUE
 * void *: item to be added
 *
 * Output:
 * 0 on success, -1 on failure
 */
int enqueue(QUEUE *, void *);

/*
 * Removes an item from the front of the queue.
 *
 * Input:
 * QUEUE *: pointer to a QUEUE
 *
 * Output:
 * 0 on success, -1 on failure
 */
int dequeue(QUEUE *);

/*
 * Returns the items at the front of the queue.
 *
 * Input:
 * QUEUE *: pointer to a QUEUE
 *
 * Output:
 * A pointer to the item at the front of the queue,
 * NULL pointer if the queue is empty.
 */ 
void *peek(QUEUE *);

/*
 * Returns the size of the queue
 */
int size(QUEUE *q);

#endif // QUEUE_H
