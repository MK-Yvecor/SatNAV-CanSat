#include <Arduino.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include "I2C_Sensors.h"
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>

#define LED_PIN 12
#define LED_COUNT 1

// I2C Pins
// SDA = 18, SCL = 17 

//Second I2C interface for EX01
// SDA = 2, SCL = 5

using namespace I2C_Sensors;
INA219 INA219_(INA219_ADDR, 0.05f, 2.0f);
Adafruit_BMP280 _BMP280;
EX_01 EX_01_(0x40, 2, 5, 10);
Adafruit_MPU6050 MPU;
Adafruit_NeoPixel WS2812B(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

float PSRAM_SIZE;
float FLASH_SIZE;

float voltage;
float ER;
float EG;
float EB;
float presssure;
float altitude;
float temperature;

float GX;
float GY;
float GZ;
float AX;
float AY;
float AZ;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(18,17);


  pinMode(2, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  if(!_BMP280.begin(0x76)){
    return;
  }

    _BMP280.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );

  if (!MPU.begin(0x68)) {   // 0x68 jeśli AD0=GND, 0x69 jeśli AD0=VCC
    Serial.println("MPU6050 not found!");
    while (1);
  }

  if(!INA219_.Init()){
    Serial.println("Failed to initialize INA219");
    return;
  }

  if(!INA219_.ConfigureSensor()){
    Serial.println("Failed to configure INA219");
    return;
  }
  EX_01_.ads_reset();
  EX_01_.ads_write_reg(0x01, 0x00);


  WS2812B.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(250);
  WS2812B.setPixelColor(0,0,0,50);
  WS2812B.show();

  sensors_event_t a,g,t;

  MPU.getEvent(&a,&g,&t);

  AX = a.acceleration.x;
  AY = a.acceleration.y;
  AZ = a.acceleration.z;

  GX = g.gyro.x;
  GY = g.gyro.y;
  GZ = g.gyro.z;
  
  ER = EX_01_.get_voltage(0x80);
  EG = EX_01_.get_voltage(0x90);
  EB = EX_01_.get_voltage(0xA0);
  voltage = INA219_.readBusVoltage();
  temperature = _BMP280.readTemperature();
  presssure = _BMP280.readPressure() / 100;
  altitude = _BMP280.readAltitude();


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
  Serial.println(ER);

  Serial.print("EG");
  Serial.println(EG);

  Serial.print("EB");
  Serial.println(EB);

  delay(20);

  Serial.print("GX");
  Serial.println(GX);

  Serial.print("GY");
  Serial.println(GY);

  Serial.print("GZ");
  Serial.println(GZ);

  delay(20);

  Serial.print("AX");
  Serial.println(AX);

  Serial.print("AY");
  Serial.println(AY);

   Serial.print("AZ");
   Serial.println(AZ);

  delay(250);
  WS2812B.setPixelColor(0,0,0,0);
  WS2812B.show();
  
}