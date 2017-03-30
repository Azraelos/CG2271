/**********   SAMPLE PROGRAM 2 **********/
#include <Arduino.h>

#define PIN_LED   6
#define PIN_LED_2 7
#define PIN_PTTM  0
#define PIN_TOUCH 1

int remapPot(int val);
int remapTouch(int touch);

void setup() {
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_LED_2, OUTPUT);
	Serial.begin(115200);
}

void loop() {
	int val, touch;

	val = analogRead(PIN_PTTM);
	val = remapPot(val);

	touch = analogRead(PIN_TOUCH);
	touch = remapTouch(touch);

	analogWrite(PIN_LED, val);

	digitalWrite(PIN_LED_2, HIGH);
	delay(touch);
	digitalWrite(PIN_LED_2, LOW);
	delay(touch);
	Serial.print(touch);
    Serial.println();
}

int remapPot(int val) {
	return val/4;
}

int remapTouch(int touch) {
	return (touch / 3) + 125;
	//if(touch/2 < 125){
	//	return 125;
	//}else{
	//	return touch/2;
	//}
}
