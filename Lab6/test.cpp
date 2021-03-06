#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define STACK_SIZE    100
#define PIN_PTTM      0
#define LED_PIN       6
#define LED_PIN_2     7
#define LED_PIN_3     8
#define LED_PIN_4     9  //automated brake
#define BUZZER        10

#define CURRENT_SPEED 0
#define SAFE_SPEED    1
#define DESIRED_SPEED 2

SemaphoreHandle_t xAccelerator;
SemaphoreHandle_t xBrake;

QueueHandle_t xSpeed;
QueueHandle_t xPTTM;

int speed[3] = {0};

void buzzer(void *p) {
	const TickType_t xFrequency0 = 1;
	const TickType_t xFrequency1 = 2;
	const TickType_t xFrequency2 = 3;
	const TickType_t xFrequency3 = 4;
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
			vTaskDelayUntil(&xLastWakeTime, xFrequency0);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency0);
			break;
		}
		case (1): {
			digitalWrite(BUZZER, HIGH);
			vTaskDelayUntil(&xLastWakeTime, xFrequency1);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency1);
			break;
		}
		case (2): {
			digitalWrite(BUZZER, HIGH);
			vTaskDelayUntil(&xLastWakeTime, xFrequency2);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency2);
			break;
		}
		case (3): {
			digitalWrite(BUZZER, HIGH);
			vTaskDelayUntil(&xLastWakeTime, xFrequency3);
			digitalWrite(BUZZER, LOW);
			vTaskDelayUntil(&xLastWakeTime, xFrequency3);
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
	const TickType_t xFrequency = 1000;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	digitalWrite(LED_PIN_4, HIGH);
	vTaskDelayUntil(&xLastWakeTime, xFrequency);
	digitalWrite(LED_PIN_4, LOW);
}

void speed_controller(void *p) {
	const TickType_t xFrequency = 500;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	int autoBrakeFlag = 0;
	int previousSafeSpeed = 0;

	for (;;) {
		if (xPTTM != 0) {
			previousSafeSpeed = speed[SAFE_SPEED];
			xQueueReceive(xPTTM, &(speed[SAFE_SPEED]), (TickType_t ) 0);
		}
		if (xSemaphoreTake(xAccelerator, 0) == pdTRUE) {
			if (speed[DESIRED_SPEED] < 3) {
				speed[DESIRED_SPEED]++;
			}
		}
		if (xSemaphoreTake(xBrake, 0) == pdTRUE) {
			if (speed[DESIRED_SPEED] > 0) {
				speed[DESIRED_SPEED]--;
			}
		}
		if (speed[DESIRED_SPEED] > speed[SAFE_SPEED]) {
			speed[CURRENT_SPEED] = speed[SAFE_SPEED];
			if (previousSafeSpeed > speed[SAFE_SPEED]) {
				autoBrakeFlag = 0;
			}
			if (autoBrakeFlag == 0) {
				autoBrake();
				autoBrakeFlag = 1;
			}
		} else {
			speed[CURRENT_SPEED] = speed[DESIRED_SPEED];
			autoBrakeFlag = 0;
		}
		if (xSpeed != 0) {
			xQueueSend(xSpeed, (void *) &speed, (TickType_t) portMAX_DELAY);
		}
		setLed(speed[CURRENT_SPEED]);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void UARTCommunication(void *p) {
	const TickType_t xFrequency = 1000;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for (;;) {
		if (xSpeed != 0) {
			xQueueReceive(xSpeed, &(speed), (TickType_t) 0);
		}
		Serial.print(F("Desired Speed: "));
		Serial.print(speed[DESIRED_SPEED]);
		Serial.print(F("   Current Speed: "));
		Serial.print(speed[CURRENT_SPEED]);
		Serial.println();
		Serial.print(F("Safe Speed to adhere: "));
		Serial.print(speed[SAFE_SPEED]);
		Serial.println();
		Serial.print(F("Distance from vehicle in front: "));
		if (speed[SAFE_SPEED] == 0) {
			Serial.println(F("d"));
		} else if (speed[SAFE_SPEED] == 1) {
			Serial.println(F("2d"));
		} else if (speed[SAFE_SPEED] == 2) {
			Serial.println(F("3d"));
		} else if (speed[SAFE_SPEED] == 3) {
			Serial.println(F("4d"));
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void accelerator() {
	unsigned long debounceTime = 0;
	unsigned long debounceDelay = 200; // the debounce time; increase if the output flickers
	unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled

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
	unsigned long debounceTime = 0;
	unsigned long debounceDelay = 200; // the debounce time; increase if the output flickers
	unsigned long lastDebounceTime = 0; // the last time the output pin was toggled

	static BaseType_t xHigherPriorityTaskWoken;
	debounceTime = millis();
	xHigherPriorityTaskWoken = pdFALSE;
	// If interrupts come faster than 20ms, assume it's a bounce and ignore
	if (debounceTime - lastDebounceTime > debounceDelay) {
		lastDebounceTime = debounceTime;
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
	xTaskCreate(UARTCommunication, "uart_communication", STACK_SIZE,
			NULL, 1,
			NULL);
	xTaskCreate(buzzer, "buzzer", STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(speed_controller, "speed_controller", STACK_SIZE, NULL,
			3,
			NULL);
	xTaskCreate(potentiometer, "potentiometer", STACK_SIZE, NULL, 4,
			NULL);
	/* start scheduler */
	vTaskStartScheduler();
}
