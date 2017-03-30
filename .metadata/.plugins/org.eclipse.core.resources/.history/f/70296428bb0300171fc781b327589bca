/*
 * prioq.h
 *
 * Created: 11/9/2013 5:24:34 PM
 *  Author: dcstanc
 */ 


#ifndef PRIOQ_H_
#define PRIOQ_H_

// Priority Queue entries. You do not need to use this.
typedef struct pe
{
	void *item;
	int prio;
	struct pe *prev, *next;
} TPrioQEntry;

/* Basic priority queue data structure */
typedef struct pt
{
	TPrioQEntry *head;
	int itemCount;
} TPrioQueue;

// Initializes and returns an empty priority queue.
TPrioQueue *makeQueue();

// Enqueues a new item. 
// Arguments:	Priority queue created by makeQueue
//				Item to add
//				Priority number. Lower number == higher priority
// Returns:		Nothing.

void enq(TPrioQueue *queue, void *item, int priority);

// Dequeues an item from the queue
// Arguments:	Priority queue to dequeue from.
// Returns:		Value dequeued.  NULL if empty.
void *deq(TPrioQueue *queue);

// Returns the number of items left in the queue
// Arguments:	Priority queue to test.
// Returns:		Number of items in the queue.
int qlen(TPrioQueue *queue);

#endif /* PRIOQ_H_ */