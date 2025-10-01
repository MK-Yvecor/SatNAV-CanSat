#include <Arduino.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 12
#define LED_COUNT 1

Adafruit_NeoPixel WS2812B(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
// Open serial port at baud rate 115200 to check if the coms are working properly

float PSRAM_SIZE;
float FLASH_SIZE;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WS2812B.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  WS2812B.setPixelColor(0,0,0,50);
  WS2812B.show();
  PSRAM_SIZE = ESP.getPsramSize();
  Serial.print("PSRAM SIZE [KB]: ");
  Serial.println(PSRAM_SIZE / 1024);
  Serial.print("PSRAM SIZE [MB]: ");
  Serial.println(PSRAM_SIZE / 1024 / 1024);

  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=");

  FLASH_SIZE = ESP.getFlashChipSize();
  Serial.print("FLASH SIZE [KB]: ");
  Serial.println(FLASH_SIZE / 1024);
  Serial.print("FLASH SIZE [MB]: ");
  Serial.println(FLASH_SIZE / 1024 / 1024);

   Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=");
  
  Serial.print("CPU Frequency [MHz]: ");
  Serial.println(ESP.getCpuFreqMHz());

  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=");
  delay(1000);
  WS2812B.setPixelColor(0,0,0,0);
  WS2812B.show();
  
}
