#include <Arduino.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include "I2C_Sensors.h"

#define LED_PIN 12
#define LED_COUNT 1

// I2C Pins
// SDA = 18, SCL = 17 

using namespace I2C_Sensors;
INA219 INA219_(INA219_ADDR, 0.05f, 2.0f);
Adafruit_NeoPixel WS2812B(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
// Open serial port at baud rate 115200 to check if the coms are working properly

float PSRAM_SIZE;
float FLASH_SIZE;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(18,17);
  if(!INA219_.Init()){
    Serial.println("Failed to initialize INA219");
    return;
  }

  if(!INA219_.ConfigureSensor()){
    Serial.println("Failed to configure INA219");
    return;
  }

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
  Serial.print("CPU Frequency [MHz]: ");
  Serial.println(ESP.getCpuFreqMHz());

  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=");

  Serial.print("Voltage [V]: ");
  Serial.println(INA219_.readBusVoltage());
  Serial.print("Current [mA]: ");
  Serial.println(INA219_.readCurrent() * 1000.0f);
  Serial.print("Shunt Voltage [mV]: ");
  Serial.println(INA219_.readShuntVoltage() * 1e+3f);
  Serial.print("Power [mW]: ");
  Serial.println(INA219_.readPower() * 1000.0f);

  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=");

  delay(1000);
  WS2812B.setPixelColor(0,0,0,0);
  WS2812B.show();
  
}
