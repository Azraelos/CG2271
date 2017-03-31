#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE 200
#define PIN_PTTM  0

SemaphoreHandle_t xSemaphore;
SemaphoreHandle_t xSemaphore2;
SemaphoreHandle_t xSemaphore3;

QueueHandle_t xQueue;

unsigned long debounceTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200; // the debounce time; increase if the output flickers

int in = 0;
int out = 0;

void consumer(void *p)
{
	const TickType_t xFrequency = 3000;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while (1) {
		xSemaphoreTake(xSemaphore3, portMAX_DELAY);
		xSemaphoreTake(xSemaphore2, portMAX_DELAY);
		if (xQueue != 0) {
			xQueueReceive(xQueue, &(out), (TickType_t ) 2);
		}
		Serial.println(out);
		xSemaphoreGive(xSemaphore2);
		xSemaphoreGive(xSemaphore3);
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
		if (xQueue != 0) {
			in = analogRead(PIN_PTTM);
			xQueueSendToBackFromISR(xQueue, (void * ) &in,
					&xHigherPriorityTaskWoken);
		}
	}
	if (xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD();
	}
}

void setup() {
	Serial.begin(115200);
	attachInterrupt(digitalPinToInterrupt(2), int0ISR, RISING);
	xSemaphore2 = xSemaphoreCreateMutex();
	xSemaphore3 = xSemaphoreCreateCounting(4, 4);
	xSemaphoreGive(xSemaphore2);
	xQueue = xQueueCreate(4, sizeof(int));
}

void loop() {
	/* create two tasks one with higher priority than the other */
	xTaskCreate(consumer, "Task2", STACK_SIZE, NULL, 1, NULL);
	/* start scheduler */
	vTaskStartScheduler();
}
