/********** SAMPLE PROGRAM 2 **********/
#include <Arduino.h>
#define PIN_LED 6

unsigned char flag = 1;
unsigned long debounceTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 5;     // the debounce time; increase if the output flickers


void isr() {
	debounceTime = millis();
	// If interrupts come faster than 5ms, assume it's a bounce and ignore
	if (debounceTime - lastDebounceTime > debounceDelay) {
		flag = !flag;
		lastDebounceTime = debounceTime;
	}
}

void setup() {
	// initialize serial communications at 115200 bps:
	attachInterrupt(digitalPinToInterrupt(2), isr, RISING);
	pinMode(PIN_LED, OUTPUT);

}

void loop() {
	digitalWrite(PIN_LED, flag);
}
