#include <Arduino.h>
#include <SPI.h>
#include <inttypes.h>

uint8_t data = 0;
void setup()
{
	Serial.begin(9600);
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	digitalWrite(SS,LOW);
}

void loop()
{
	data = map(analogRead(A0), 0, 1023, 0, 255);
	Serial.println(data);
	SPI.transfer(data);
	delay(1000);
}
