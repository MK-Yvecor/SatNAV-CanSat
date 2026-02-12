#include <Arduino.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include "I2C_Sensors.h"
#include <Adafruit_BMP280.h>
#include <RadioLib.h>
#include <SPI.h>

#define LED_PIN 12
#define LED_COUNT 1

#define L_CS 33
#define L_DIO0 37
#define L_RST 38

#define SCK 36
#define MISO 34
#define MOSI 35

// I2C Pins
// SDA = 18, SCL = 17 

//Second I2C interface for EX01
// SDA = 2, SCL = 5
// EX_01 Address 0x40

using namespace I2C_Sensors;

SPIClass spiLoRa(FSPI);

INA219 INA219_(INA219_ADDR, 0.05f, 2.0f);
SX1278 radio = new Module(L_CS, L_DIO0, L_RST, -1, spiLoRa);
Adafruit_BMP280 _BMP280;
EX_01 EX_01_(0x40, 2, 5, 10);
MPU6050 MPU;
Adafruit_NeoPixel WS2812B(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

float PSRAM_SIZE;
float FLASH_SIZE;

float voltage;
float presssure;
float altitude;
float temperature;

EX_01::EX_Data _EX_Data;
MPU6050::GyroData GyroData;
MPU6050::AccelData AccelData;

void setup() {
  delay(500);

  Serial.begin(115200);
  Wire.begin(18,17);
  
  spiLoRa.begin(SCK, MISO, MOSI, L_CS);

  pinMode(2, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  WS2812B.begin();
  
  /*if(!_BMP280.begin(0x76)){
    Serial.println("Failed to initialize BMP280");
    WS2812B.setPixelColor(50,0,0,0);
    WS2812B.show();
    delay(3000);
    return;
  }
  
  _BMP280.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );
  
  if (!MPU.ConfigureSensor()) {   // 0x68 I2C Address
    Serial.println("MPU6050 not found!");
    WS2812B.setPixelColor(50,0,0,0);
    WS2812B.show();
    delay(3000);
    return;
  }
  
  if(!INA219_.Init()){
    Serial.println("Failed to initialize INA219");
    WS2812B.setPixelColor(50,0,0,0);
    WS2812B.show();
    delay(3000);
    return;
  }
  
  if(!INA219_.ConfigureSensor()){
      WS2812B.setPixelColor(50,0,0,0);
      WS2812B.show();
      Serial.println("Failed to configure INA219");
      delay(3000);
      return;
  }
  EX_01_.ads_reset();
  EX_01_.ads_write_reg(0x01, 0x00);
  */
  
  int state = radio.begin(433.0, 125.0, 9, 7, 0x12, 17);

  if(state != RADIOLIB_ERR_NONE){
    WS2812B.setPixelColor(50,0,0,0);
    WS2812B.show();
    Serial.print("Failed to initialize LoRa, ERR Code: ");
    Serial.println(state);
    delay(3000);
    return;
  }
  else { 
    Serial.println("Radio Ready"); 
    for(int i = 0; i<=5; i++){
      delay(250);
      WS2812B.setPixelColor(0,50,50,0);
      WS2812B.show();
      delay(250);
      WS2812B.setPixelColor(0,0,00,0);
      WS2812B.show();
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(250);
  WS2812B.setPixelColor(0,0,0,50);
  WS2812B.show();

  MPU.ReadDataIMU();
  AccelData = MPU.GetAccelData();
  GyroData = MPU.GetGyroData();

  _EX_Data = EX_01_.get_EX_01_Data();

  voltage = INA219_.readBusVoltage();
  temperature = _BMP280.readTemperature();
  presssure = _BMP280.readPressure() / 100;
  altitude = _BMP280.readAltitude();

  int state = radio.transmit("HELLO FROM LORA");

  if(state == RADIOLIB_ERR_NONE){
    Serial.println("TX Success");
  }
  else{
    Serial.print("Failed to TX, ERR CODE: ");
    Serial.println(state);
  }

  delay(1000);
  /*

  Serial.print("TE");
  Serial.println(temperature);

  Serial.print("PR");
  Serial.println(presssure);
  
  Serial.print("AL");
  Serial.println(altitude);

  delay(20);

  Serial.print("VO");
  Serial.println(voltage);

  Serial.print("ER");
  Serial.println(_EX_Data.RED_Channel);

  Serial.print("EG");
  Serial.println(_EX_Data.GREEN_Channel);

  Serial.print("EB");
  Serial.println(_EX_Data.BLUE_Channel);

  delay(20);

  Serial.print("GX");
  Serial.println(GyroData.GX);

  Serial.print("GY");
  Serial.println(GyroData.GY);

  Serial.print("GZ");
  Serial.println(GyroData.GZ);

  delay(20);

  Serial.print("AX");
  Serial.println(AccelData.AX);

  Serial.print("AY");
  Serial.println(AccelData.AY);

   Serial.print("AZ");
   Serial.println(AccelData.AZ);
   */
  delay(250);
  WS2812B.setPixelColor(0,0,0,0);
  WS2812B.show();
  
}