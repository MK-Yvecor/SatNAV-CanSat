#include <Arduino.h>

// Open serial port at baud rate 115200 to check if the coms are working properly

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("TEST");
  delay(500);
}
