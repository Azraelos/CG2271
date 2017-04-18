#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE 200
#define PIN_PTTM     0
#define LED_PIN      6
#define LED_PIN_2    7
#define LED_PIN_3    8
#define LED_PIN_4    9  //automated brake
#define BUZZER      10

SemaphoreHandle_t xAccelerator;
SemaphoreHandle_t xBrake;

QueueHandle_t xSpeed;
QueueHandle_t xPTTM;

unsigned long debounceTime = 0;
unsigned long debounceDelay = 200; // the debounce time; increase if the output flickers
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTime2 = 0; // the last time the output pin was toggled

void buzzer(void *p) {
	const TickType_t xFrequency0 = 2;
	const TickType_t xFrequency1 = 4;
	const TickType_t xFrequency2 = 6;
	const TickType_t xFrequency3 = 8;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int buzzSpeed;

	for (;;) {
		if (xSpeed != 0) {
			xQueueReceive(xSpeed, (void * ) &buzzSpeed,
					(TickType_t ) 0);
		}
		switch (buzzSpeed) {
		case (0): {
			digitalWrite(BUZZER, HIGH);
			vTaskDelayUntil(&xLastWakeTime, xFrequency0 / 2);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency0 / 2);
			break;
		}
		case (1): {
			digitalWrite(BUZZER, HIGH);
			vTaskDelayUntil(&xLastWakeTime, xFrequency1 / 2);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency1 / 2);
			break;
		}
		case (2): {
			digitalWrite(BUZZER, HIGH);
			vTaskDelayUntil(&xLastWakeTime, xFrequency2 / 2);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency2 / 2);
			break;
		}
		case (3): {
			digitalWrite(BUZZER, HIGH);
			vTaskDelayUntil(&xLastWakeTime, xFrequency3 / 2);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency3 / 2);
			break;
		}
		}
	}
}

void setLed(int lightSpeed) {
	switch (lightSpeed) {
	case (0): {
		digitalWrite(LED_PIN, LOW);
		digitalWrite(LED_PIN_2, LOW);
		digitalWrite(LED_PIN_3, LOW);
		break;
	}
	case (1): {
		digitalWrite(LED_PIN, HIGH);
		digitalWrite(LED_PIN_2, LOW);
		digitalWrite(LED_PIN_3, LOW);
		break;
	}
	case (2): {
		digitalWrite(LED_PIN, HIGH);
		digitalWrite(LED_PIN_2, HIGH);
		digitalWrite(LED_PIN_3, LOW);
		break;
	}
	case (3): {
		digitalWrite(LED_PIN, HIGH);
		digitalWrite(LED_PIN_2, HIGH);
		digitalWrite(LED_PIN_3, HIGH);
		break;
	}
	}
}

void potentiometer(void *p) {
	const TickType_t xFrequency = 500;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int pttm_reading = 0;

	for (;;) {
		pttm_reading = analogRead(PIN_PTTM) / 256;
		if (xPTTM != 0) {
			xQueueSend(xPTTM, (void * ) &pttm_reading,
					(TickType_t ) portMAX_DELAY);
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void autoBrake() {
	digitalWrite(LED_PIN_4, HIGH);
	delay(1000);
	digitalWrite(LED_PIN_4, LOW);
}

void speed_controller(void *p) {
	int speed = 0;
	int safe_speed;

	const TickType_t xFrequency = 500;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		if (xPTTM != 0) {
			xQueueReceive(xPTTM, &(safe_speed), (TickType_t ) 0);
		}
		if (speed > safe_speed) {
			speed = safe_speed;
			autoBrake();

		}
		if (xSemaphoreTake(xAccelerator, 0) == pdTRUE) {
			if (speed < 3 && speed < safe_speed) {
				speed++;
			}

		}
		if (xSemaphoreTake(xBrake, 0) == pdTRUE) {
			if (speed > 0) {
				speed--;
			}
		}

		if (xSpeed != 0) {
			xQueueSend(xSpeed, (void * ) &speed,
					(TickType_t ) portMAX_DELAY);
		}
		setLed(speed);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void UARTCommunication(void *p) {
	const TickType_t xFrequency = 1000;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	//	Serial.print("PTTM: ");
	//	Serial.println(pttm_reading);
	//	Serial.print("Safe: ");
	//	Serial.println(safe_speed);
	//	Serial.print("Speed: ");
	//	Serial.println(speed);
}

void accelerator() {
	static BaseType_t xHigherPriorityTaskWoken;
	debounceTime = millis();
	xHigherPriorityTaskWoken = pdFALSE;
	// If interrupts come faster than 20ms, assume it's a bounce and ignore
	if (debounceTime - lastDebounceTime > debounceDelay) {
		lastDebounceTime = debounceTime;
		xSemaphoreGiveFromISR(xAccelerator, &xHigherPriorityTaskWoken);
	}
	if (xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD();
	}
}

void brake() {
	static BaseType_t xHigherPriorityTaskWoken;
	debounceTime = millis();
	xHigherPriorityTaskWoken = pdFALSE;
	// If interrupts come faster than 20ms, assume it's a bounce and ignore
	if (debounceTime - lastDebounceTime2 > debounceDelay) {
		lastDebounceTime2 = debounceTime;
		xSemaphoreGiveFromISR(xBrake, &xHigherPriorityTaskWoken);
	}
	if (xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD();
	}
}

void setup() {
	Serial.begin(115200);
	pinMode(LED_PIN, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
	pinMode(LED_PIN_3, OUTPUT);
	pinMode(LED_PIN_4, OUTPUT);
	pinMode(BUZZER, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(2), accelerator, RISING);
	attachInterrupt(digitalPinToInterrupt(3), brake, RISING);

	xAccelerator = xSemaphoreCreateBinary();
	xBrake = xSemaphoreCreateBinary();
	xSpeed = xQueueCreate(3, sizeof(int));
	xPTTM = xQueueCreate(3, sizeof(int));
}

void loop() {
	/* create two tasks one with higher priority than the other */
	xTaskCreate(buzzer, "buzzer", STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(speed_controller, "speed_controller", STACK_SIZE, NULL,
			2, NULL);
	xTaskCreate(potentiometer, "potentiometer", STACK_SIZE, NULL, 3,
			NULL);
	/* start scheduler */
	vTaskStartScheduler();
}
