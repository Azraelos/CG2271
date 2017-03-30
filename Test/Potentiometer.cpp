/**********   SAMPLE PROGRAM 2 **********/
#include <Arduino.h>
#define PIN_LED  6
#define PIN_PTTM 0

int remapPot(int val);

void setup() {
	pinMode(PIN_LED, OUTPUT);
}

void loop() {
	int val;
	val = analogRead(PIN_PTTM);
	val = remapPot(val);
	analogWrite(PIN_LED, val);
	delay(500);
}

int remapPot(int val) {
	return val/4;
}
