/**********   SAMPLE PROGRAM 2 **********/
#include <Arduino.h>

#define PIN_LED   6
#define PIN_LED_2 7
#define PIN_PTTM  0
#define PIN_TOUCH 1

int remapPot(int val);
int remapTouch(int touch);

unsigned char flag = LOW;
unsigned long debounceTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 5;     // the debounce time; increase if the output flickers

unsigned char flag_2 = LOW;
unsigned long debounceTime_2 = 0;
unsigned long lastDebounceTime_2 = 0;  // the last time the output pin was toggled
unsigned long debounceDelay_2 = 5;     // the debounce time; increase if the output flickers

void isr() {
	debounceTime = millis();
	// If interrupts come faster than 5ms, assume it's a bounce and ignore
	if (debounceTime - lastDebounceTime > debounceDelay) {
		flag = HIGH;
		lastDebounceTime = debounceTime;
	}
}

void isr_2() {
	debounceTime_2 = millis();
	// If interrupts come faster than 5ms, assume it's a bounce and ignore
	if (debounceTime_2 - lastDebounceTime_2 > debounceDelay_2) {
		flag_2 = HIGH;
		lastDebounceTime_2 = debounceTime_2;
	}
}

void setup() {
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_LED_2, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(2), isr, RISING);
	attachInterrupt(digitalPinToInterrupt(3), isr_2, RISING);
}

void loop() {
	int val, touch;

	val = analogRead(PIN_PTTM);
	val = remapPot(val);

	touch = analogRead(PIN_TOUCH);
	touch = remapTouch(touch);

	if(flag == HIGH) {
		analogWrite(PIN_LED, val);
	}

	if(flag_2 == HIGH){
		digitalWrite(PIN_LED_2, HIGH);
		delay(touch);
		digitalWrite(PIN_LED_2, LOW);
		delay(touch);
	}
}

int remapPot(int val) {
	return val/4;
}

int remapTouch(int touch) {
	return (touch / 3) + 125;
}
