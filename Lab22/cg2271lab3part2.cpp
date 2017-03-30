#include <Arduino.h>
#include "prioq.h"
#define QLEN 10
TPrioQueue *queue;
void setup()
{
	// Set up the queue.
	queue=makeQueue();
	// Initialize the serial port
	Serial.begin(115200);
	// Enqueue 10 numbers
	for(int i=0; i<QLEN; i++)
		enq(queue, (void *)i, QLEN-i-1);
}
void loop()
{
	int val;
	// If we still have an item to dequeue
	if(qlen(queue)>0)
	{
		// Dequeue it
		val=(int) deq(queue);
		// And print it on the serial port.
		Serial.println(val);
	}
	//500ms pause
	delay(500);
}
