#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE 200
#define PIN_PTTM  0

SemaphoreHandle_t xSemaphore;
SemaphoreHandle_t xFull;
SemaphoreHandle_t xEmpty;

unsigned long debounceTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200; // the debounce time; increase if the output flickers

int in = 0;
int out = 0;
int buffer[4] = { 0 };

void producer(void *p)
{
	while (1) {
		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
			xSemaphoreTake(xEmpty, portMAX_DELAY);
			buffer[in] = analogRead(PIN_PTTM);
			in = (in + 1) % 4;
			xSemaphoreGive(xFull);
		}
	}
}

void consumer(void *p)
{
	const TickType_t xFrequency = 5000;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while (1) {
		xSemaphoreTake(xFull, portMAX_DELAY);
		Serial.println(buffer[out]);
		out = (out + 1) % 4;
		xSemaphoreGive(xEmpty);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void int0ISR() {
	static BaseType_t xHigherPriorityTaskWoken;
	debounceTime = millis();
	xHigherPriorityTaskWoken = pdFALSE;
	// If interrupts come faster than 10ms, assume it's a bounce and ignore
	if (debounceTime - lastDebounceTime > debounceDelay) {
		lastDebounceTime = debounceTime;
		xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
	}
	if (xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD();
	}
}

void setup() {
	Serial.begin(115200);
	attachInterrupt(digitalPinToInterrupt(2), int0ISR, RISING);
	xSemaphore = xSemaphoreCreateBinary();
	xFull = xSemaphoreCreateCounting(4, 0);
	xEmpty = xSemaphoreCreateCounting(4, 4);
}

void loop() {
	/* create two tasks one with higher priority than the other */
	xTaskCreate(producer, "Task1", STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(consumer, "Task2", STACK_SIZE, NULL, 1, NULL);
	/* start scheduler */
	vTaskStartScheduler();
}
