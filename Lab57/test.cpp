#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE 200
#define LED_PIN      6
#define LED_PIN_2    7
#define LED_PIN_3    8
#define LED_PIN_4    9

#define TASK_WCET    3000
#define TASK2_WCET   4000
#define TASK3_WCET   10000

SemaphoreHandle_t xSemaphore;

unsigned long debounceTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200; // the debounce time; increase if the output flickers

void myDelay(int ms);

void task1(void *p) {
	const TickType_t xFrequency = 10000;
	TickType_t xLastWakeTime = 0;

	for (;;) {
		int count = 0;
		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		while (count < TASK_WCET) {
			digitalWrite(LED_PIN, HIGH);
			digitalWrite(LED_PIN_4, HIGH);
			myDelay(500);
			digitalWrite(LED_PIN, LOW);
			digitalWrite(LED_PIN_4, LOW);
			myDelay(500);
			count += 1000;
		}
		xSemaphoreGive(xSemaphore);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task2(void *p) {
	const TickType_t xFrequency = 15000;
	TickType_t xLastWakeTime = 0;

	for (;;) {
		int count2 = 0;
		while (count2 < TASK2_WCET) {
			digitalWrite(LED_PIN_2, HIGH);
			myDelay(500);
			digitalWrite(LED_PIN_2, LOW);
			myDelay(500);
			count2 += 1000;
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void task3(void *p) {
	const TickType_t xFrequency = 35000;
	TickType_t xLastWakeTime = 0;

	for (;;) {
		int count3 = 0;
		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		while (count3 < TASK3_WCET) {
			digitalWrite(LED_PIN_3, HIGH);
			digitalWrite(LED_PIN_4, HIGH);
			myDelay(500);
			digitalWrite(LED_PIN_3, LOW);
			digitalWrite(LED_PIN_4, LOW);
			myDelay(500);
			count3 += 1000;
		}
		xSemaphoreGive(xSemaphore);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void setup() {
	Serial.begin(115200);
	pinMode(LED_PIN, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
	pinMode(LED_PIN_3, OUTPUT);
	pinMode(LED_PIN_4, OUTPUT);
	xSemaphore = xSemaphoreCreateMutex();
	xSemaphoreGive(xSemaphore);
}

void myDelay(int ms) {
	for (int i = 0; i < ms; i++) {
		delayMicroseconds(1000);
	}
}

void loop() {
	/* create two tasks one with higher priority than the other */
	xTaskCreate(task1, "Task1", STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(task2, "Task2", STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(task3, "Task3", STACK_SIZE, NULL, 1, NULL);
	/* start scheduler */
	vTaskStartScheduler();
}