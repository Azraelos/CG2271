#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>

#define STACK_SIZE   200
#define LED_PIN      6
#define LED_PIN_2    7
#define LED_PIN_3    8
#define LED_PIN_4    9

void task1(void *p)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(750);   //in ms

	xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		digitalWrite(LED_PIN, HIGH);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		digitalWrite(LED_PIN, LOW);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task2(void *p)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;   //in ms

	xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		digitalWrite(LED_PIN_2, HIGH);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		digitalWrite(LED_PIN_2, LOW);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task3(void *p)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 2000;   //in ms

	xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		digitalWrite(LED_PIN_3, HIGH);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		digitalWrite(LED_PIN_3, LOW);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task4(void *p)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;   //in ms
	int i = -10;

	for (;;) {
		xLastWakeTime = xTaskGetTickCount();
		digitalWrite(LED_PIN_4, HIGH);
		vTaskDelayUntil(&xLastWakeTime, xFrequency+i);
		digitalWrite(LED_PIN_4, LOW);
		vTaskDelayUntil(&xLastWakeTime, xFrequency-i);
		if(i==10) {
			i = -10;
		} else {
			i++;
		}
	}
}

void setup()
{
	pinMode(LED_PIN, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
	pinMode(LED_PIN_3, OUTPUT);
	pinMode(LED_PIN_4, OUTPUT);
}

void loop() {
	xTaskCreate(task1,       // Pointer to the task entry function
			"Task1",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			3,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	xTaskCreate(task2,       // Pointer to the task entry function
			"Task2",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			2,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	xTaskCreate(task3,       // Pointer to the task entry function
			"Task3",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			1,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	xTaskCreate(task4,       // Pointer to the task entry function
			"Task4",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			4,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	vTaskStartScheduler();
}
