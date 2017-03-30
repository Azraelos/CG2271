/********** SAMPLE PROGRAM 2 **********/ #include <Arduino.h>
#define PIN_PTTM 0
#define PIN_TOUCH 1

int remapTouch(int value);
int remapPot(int val);

void setup() {
	// initialize serial communications at 115200 bps:
	Serial.begin(115200);
}
void loop() {
	int val, touch;

	// read potentiometer's value
	val = analogRead(PIN_PTTM); // read touch sensor's value
	val = remapPot(val);
	touch = analogRead(PIN_TOUCH);
	touch = remapTouch(touch);

	// dump them to serial port
	Serial.print(val);
	Serial.print(" ");
	Serial.print(touch);
	Serial.println();

	// 200ms pause
	delay(200);
}

int remapPot(int val) {
	return val/4;
}

int remapTouch(int value) {
	return (value / 3) + 125;
}
