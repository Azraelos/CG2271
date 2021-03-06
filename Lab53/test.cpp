#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE 200
#define LED_PIN      7
#define LED_PIN_2    6

SemaphoreHandle_t xSemaphore;
SemaphoreHandle_t xSemaphore2;

unsigned long debounceTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200; // the debounce time; increase if the output flickers

unsigned long debounceTime_2 = 0;
unsigned long lastDebounceTime_2 = 0; // the last time the output pin was toggled
unsigned long debounceDelay_2 = 200; // the debounce time; increase if the output flickers

void int0task(void *p)
{
	while (1) {
		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
			for (int i = 0; i < 7; i++) {
				digitalWrite(LED_PIN, HIGH);
				delay(125);
				digitalWrite(LED_PIN, LOW);
				delay(125);
			}
		}
	}
}

void int1task(void *p)
{
	while (1) {
		if (xSemaphoreTake(xSemaphore2, portMAX_DELAY) == pdTRUE) {
			for (int i = 0; i < 6; i++) {
				digitalWrite(LED_PIN_2, HIGH);
				delay(250);
				digitalWrite(LED_PIN_2, LOW);
				delay(250);
			}
		}
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
		Serial.println("isr0");
	}
	if (xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD();
	}
}

void int1ISR() {
	static BaseType_t xHigherPriorityTaskWoken;
	debounceTime_2 = millis();
	xHigherPriorityTaskWoken = pdFALSE;
	// If interrupts come faster than 10ms, assume it's a bounce and ignore
	if (debounceTime_2 - lastDebounceTime_2 > debounceDelay_2) {
		lastDebounceTime_2 = debounceTime_2;
		xSemaphoreGiveFromISR(xSemaphore2, &xHigherPriorityTaskWoken);
		Serial.println("isr1");
	}
	if (xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD();
	}
}

void setup() {
	Serial.begin(115200);
	pinMode(LED_PIN, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(2), int0ISR, RISING);
	attachInterrupt(digitalPinToInterrupt(3), int1ISR, RISING);
	xSemaphore = xSemaphoreCreateBinary();
	xSemaphore2 = xSemaphoreCreateBinary();
}

void loop() {
	/* create two tasks one with higher priority than the other */
	xTaskCreate(int0task, "Task1", STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(int1task, "Task2", STACK_SIZE, NULL, 1, NULL);
	/* start scheduler */
	vTaskStartScheduler();
}
