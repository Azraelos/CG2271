#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>

#define STACK_SIZE 200
#define LED_PIN      6
#define LED_PIN_2    7
#define LED_PIN_3    8
#define LED_PIN_4    9

#define TASK_WCET    1500
#define TASK2_WCET   2500
#define TASK3_WCET   1700
#define TASK4_WCET   700

void myDelay(int ms);

void task1()
{
	int count = 0;
	while (count < TASK_WCET) {
		digitalWrite(LED_PIN, HIGH);
		myDelay(50);
		digitalWrite(LED_PIN, LOW);
		myDelay(50);
		count +=100;
	}
}

void task2()
{
	int count2 = 0;
	while (count2 < TASK2_WCET) {
		digitalWrite(LED_PIN_2, HIGH);
		myDelay(50);
		digitalWrite(LED_PIN_2, LOW);
		myDelay(50);
		count2 +=100;
	}
}

void task3()
{
	int count3 = 0;
	while (count3 < TASK3_WCET) {
		digitalWrite(LED_PIN_3, HIGH);
		myDelay(50);
		digitalWrite(LED_PIN_3, LOW);
		myDelay(50);
		count3 +=100;
	}
}

void task4()
{
	int count4 = 0;
	while (count4 < TASK4_WCET) {
		digitalWrite(LED_PIN_4, HIGH);
		myDelay(50);
		digitalWrite(LED_PIN_4, LOW);
		myDelay(50);
		count4 +=100;
	}
}
void task5(void *p)
{
	TickType_t xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();

	for(;;){
		task1();
		task2();
		vTaskDelayUntil(&xLastWakeTime, 5000);
		task1();
		task3();
		task4();
		vTaskDelayUntil(&xLastWakeTime, 5000);
		task1();
		task2();
		vTaskDelayUntil(&xLastWakeTime, 5000);
		task1();
		task3();
		vTaskDelayUntil(&xLastWakeTime, 5000);
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
	xTaskCreate(task5,       // Pointer to the task entry function
				"Task5",         // Task name
				STACK_SIZE,      // Stack size
				NULL,            // Pointer that will be used as parameter
				1,               // Task priority
				NULL);           // Used to pass back a handle by which the created task can be referenced.
		vTaskStartScheduler();
}

void loop() {
}
