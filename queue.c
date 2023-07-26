#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue.h"

QUEUE *queueCreate()
{
    // allocate space for a queue struct
    QUEUE *queue = malloc(sizeof(QUEUE));

    if ( queue == NULL )
    {
        fprintf(stderr, "queueCreate(): malloc error\n");
        return NULL;
    }

    // set default values
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    return queue;
}

int enqueue(QUEUE *q, void *item)
{
    NODE *node;

    // set up new node
    node = malloc(sizeof(NODE));

    if ( node == NULL )
    {
        fprintf(stderr, "enqueue(): malloc error\n");
        return -1;
    }
    else
    {
        node->next = NULL;
        node->item = item;
    }

    // queue is empty
    if ( q->size == 0 )
    {
        q->head = node;
        q->tail = node;
        q->size = 1;
    }
    // queue is non-empty
    else
    {
        q->tail->next = node;
        q->tail = node;
        q->size++;
    }

    return 0;
}

int dequeue(QUEUE *q)
{
    // queue is empty
    if ( q->size == 0 )
    {
        fprintf(stderr, "dequeue(): cannot remove item from empty queue\n");
        return -1;
    }

    // queue contains 1 item
    if ( q->size == 1 )
    {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    // queue contains multiple items
    else
    {
        q->head = q->head->next;
        q->size--;
    }

    return 0;
}

void *peek(QUEUE *q)
{
    // queue is empty
    if ( q->size == 0 )
    {
        fprintf(stderr, "peek(): queue is empty\n");
        return NULL;
    }
    // queue is non-empty
    return q->head->item;
}

int size(QUEUE *q)
{
    return q->size;
}
