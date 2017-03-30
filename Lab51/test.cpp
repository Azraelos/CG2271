#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE 200

SemaphoreHandle_t xSemaphore;

void task1and2(void *p) {
	if (xSemaphore != NULL){
		while (1){
			if( xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE ) {
				int taskNum = (int) p;
				Serial.print("Task ");
				Serial.println(taskNum);
				xSemaphoreGive( xSemaphore );
				vTaskDelay(1);
			}
		}
	}
}

void setup() {
	Serial.begin(115200);
	xSemaphore = xSemaphoreCreateMutex();
	xSemaphoreGive(xSemaphore);
}

void loop() {
	/* create two tasks one with higher priority than the other */
	xTaskCreate(task1and2, "Task1", STACK_SIZE, (void * ) 1, 1, NULL);
	xTaskCreate(task1and2, "Task2", STACK_SIZE, (void * ) 2, 2, NULL);
	/* start scheduler */
	vTaskStartScheduler();
}
