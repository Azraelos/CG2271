#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE 200

QueueHandle_t xQueue;

void serialPrint(void *p) {
	while (1) {
		int taskNum;
		/*receive taskNum from message queue */
		if(xQueue != 0) {
			xQueueReceive( xQueue, &( taskNum ), ( TickType_t ) 2 );
		}
		Serial.print("Task ");
		Serial.println(taskNum);
	}
}

void task1and2(void *p) {
	while (1) {
		int taskNum = (int) p;
		/* send taskNum to message queue */
		if(xQueue != 0) {
			xQueueSend( xQueue, ( void * ) &taskNum, ( TickType_t ) 2 );
		}
		vTaskDelay(1);
	}
}

void setup() {
	Serial.begin(115200);
	xQueue = xQueueCreate( 1, sizeof(int) );
}

void loop() {
	/* create two tasks one with higher priority than the other */
	xTaskCreate(task1and2, "Task1", STACK_SIZE, (void * ) 1, 2, NULL);
	xTaskCreate(task1and2, "Task2", STACK_SIZE, (void * ) 2, 3, NULL);
	xTaskCreate(serialPrint, "Task3", STACK_SIZE, NULL, 1, NULL);
	/* start scheduler */
	vTaskStartScheduler();
}
