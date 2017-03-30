#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>

#define STACK_SIZE   200
#define LED_PIN      6
#define LED_PIN_2    7
#define LED_PIN_3    8
#define LED_PIN_4    9

#define TASK_WCET    1500
#define TASK2_WCET   2500
#define TASK3_WCET   1700
#define TASK4_WCET   700

void myDelay(int ms);

void task1(void *p)
{
	const TickType_t xFrequency = 5000;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		int count = 0;
		while (count < TASK_WCET) {
			digitalWrite(LED_PIN, HIGH);
			myDelay(50);
			digitalWrite(LED_PIN, LOW);
			myDelay(50);
			count +=100;
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task2(void *p)
{
	const TickType_t xFrequency = 10000;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		int count = 0;
		while (count < TASK2_WCET) {
			digitalWrite(LED_PIN_2, HIGH);
			myDelay(50);
			digitalWrite(LED_PIN_2, LOW);
			myDelay(50);
			count +=100;
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task3(void *p)
{
	const TickType_t xFrequency = 10000;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		int count = 0;
		while (count < TASK3_WCET) {
			digitalWrite(LED_PIN_3, HIGH);
			myDelay(50);
			digitalWrite(LED_PIN_3, LOW);
			myDelay(50);
			count +=100;
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task4(void *p)
{
	const TickType_t xFrequency = 20000;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		int count = 0;
		while (count < TASK4_WCET) {
			digitalWrite(LED_PIN_4, HIGH);
			myDelay(50);
			digitalWrite(LED_PIN_4, LOW);
			myDelay(50);
			count +=100;
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void myDelay(int ms) {
	for (int i = 0; i < ms; i++) {
		delayMicroseconds(1000);
	}
}

void setup()
{
	pinMode(LED_PIN, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
	pinMode(LED_PIN_3, OUTPUT);
	pinMode(LED_PIN_4, OUTPUT);
	Serial.begin(115200);
	xTaskCreate(task1,       // Pointer to the task entry function
			"Task1",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			4,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	xTaskCreate(task2,       // Pointer to the task entry function
			"Task2",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			3,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	xTaskCreate(task3,       // Pointer to the task entry function
			"Task3",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			2,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	xTaskCreate(task4,       // Pointer to the task entry function
			"Task4",         // Task name
			STACK_SIZE,      // Stack size
			NULL,            // Pointer that will be used as parameter
			1,               // Task priority
			NULL);           // Used to pass back a handle by which the created task can be referenced.
	vTaskStartScheduler();
}

void loop() {

}
