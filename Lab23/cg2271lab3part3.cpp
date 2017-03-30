

#include <Arduino.h>
#include "prioq.h"

#define PIN_LED 7
#define PIN_LED_2 6

TPrioQueue *queue;

// Declares a new type called "funcptr"

typedef void (*funcptr)(void);
funcptr function;

unsigned long debounceTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;     // the debounce time; increase if the output flickers

unsigned long debounceTime_2 = 0;
unsigned long lastDebounceTime_2 = 0;  // the last time the output pin was toggled
unsigned long debounceDelay_2 = 100;     // the debounce time; increase if the output flickers

// Flashes LED at pin 7: 5 times at 4 Hz
void int0task()
{
	for(int i = 0; i < 5; i++){
		digitalWrite(PIN_LED, HIGH);
		delay(125);
		digitalWrite(PIN_LED, LOW);
		delay(125);
	}
	Serial.println("test0");
}

// Flashes LED at pin 6: 5 times at 2HZ
void int1task()
{
	for(int i = 0; i < 5; i++){
		digitalWrite(PIN_LED_2, HIGH);
		delay(250);
		digitalWrite(PIN_LED_2, LOW);
		delay(250);
	}
	Serial.println("test1");
}

void int0ISR()
{
	debounceTime = millis();
	// If interrupts come faster than 5ms, assume it's a bounce and ignore
	if (debounceTime - lastDebounceTime > debounceDelay) {
		lastDebounceTime = debounceTime;
		enq(queue, (void *)int0task, 0);
		Serial.println("isr0");
	}

}

void int1ISR()
{
	debounceTime_2 = millis();
	// If interrupts come faster than 5ms, assume it's a bounce and ignore
	if (debounceTime_2 - lastDebounceTime_2 > debounceDelay_2) {
		lastDebounceTime_2 = debounceTime_2;
		enq(queue, (void *)int1task, 1);
		Serial.println("isr1");
	}

}

void setup()
{
	// Set up the queue.
	queue=makeQueue();
	// Initialize the serial port
	Serial.begin(115200);
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_LED_2, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(2), int0ISR, RISING);
	attachInterrupt(digitalPinToInterrupt(3), int1ISR, RISING);
}

// Dequeues and calls functions if the queue is not empty
void loop()
{
	if(qlen(queue)>0){
		function = (funcptr) deq(queue);
		function();
	}
}
