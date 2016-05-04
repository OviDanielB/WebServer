//
// Created by laura_trive on 18/03/16.
//

/**
 * Implementations of functions to manage queue type data.
 */

#include <stdio.h>

#include "queue.h"

/*   Queue node struct   */
typedef struct queueNode {
    struct queueElem element;
    struct queueNode *prev;
    struct queueNode *next;
};

/*   Queue struct    */
typedef struct queue {
    struct queueNode *first;
    struct queueNode *last;
};

/*  Allocation in dynamic memory of a new queue's length.   */
struct queue *alloc_queue(void)
{
    struct queue *q;
    q = malloc(sizeof(struct queue));
    if (q == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return q;
}

/*   Allocation in dynamic memory of a new queue node's length.   */
struct queueNode *alloc_node(void)
{
    struct queueNode *p;
    p = malloc(sizeof(struct queueNode));
    if (p == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/*  De-allocation of a queue node from dynamic memory.   */
void deallocate_node(struct queueNode *qn)
{
    free(qn);
}

/*  Create a new queue    */
struct queue createQueue()
{
    struct queue newQueue;
    newQueue = alloc_queue();
    newQueue->first = NULL;
    newQueue->last = NULL;
    return newQueue;
}

/*  Destroy queue  */
void destroyQueue(struct queue *delQueue)
    {
        // remove each element from the queue (each element is in a dynamically-allocated node
        while (!isEmpty(delQueue))
            dequeue(delQueue);

        // reset the front and rear just in case someone tries to use them after the queue is freed.
        queue->front = queue->rear = NULL;

        // free the structure that holds information about the queue.
        free(delQueue);
    }

/*  Enqueue operation   */
void enqueue(struct queueNode *new, struct queueNode **last)
{
    new->*prev = *last;
    *last = new;
}

/*  Dequeue operation   */
struct queueNode *dequeue(struct queueNode **first)
{
    struct queueNode *head = *first;
    *first = head->next;
    newHead->*prev = NULL;
    deallocate_node(first);
    return head;
}

/*  Check number of queue's elements    */
boolean isEmpty(struct queue *queue)
{
    return queue->*first == queue->*last NULL;
}
