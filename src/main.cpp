#include <Arduino.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include "I2C_Sensors.h"
#include <Adafruit_BMP280.h>
#include <RadioLib.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include "UART.h"
#include "Commands.h"

#define LED_PIN 12
#define LED_COUNT 1

#define L_CS 33
#define L_DIO0 37
#define L_RST 38

#define SCK 36
#define MISO 34
#define MOSI 35

const int rxPin = 13; // TX z L86
const int txPin = 14; // RX z L86

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
TinyGPSPlus gps;

float PSRAM_SIZE;
float FLASH_SIZE;

double voltage;
double presssure;
double altitude;
double temperature;

EX_01::EX_Data _EX_Data;
MPU6050::GyroData GyroData;
MPU6050::AccelData AccelData;

Commands comm;
UARTReader UR;
UARTSender US;


volatile bool transmitting = false;

byte buffer_LoRa[1024];


void OnTransmitDone(){
  transmitting = false;
}


void setup() {
  delay(500);

  Serial.begin(115200);
  Wire.begin(18,17);
  WS2812B.begin();
  WS2812B.setPixelColor(0,0,10,0);
  WS2812B.show();
  delay(1000);
  
  spiLoRa.begin(SCK, MISO, MOSI, L_CS);

  //pinMode(2, INPUT_PULLUP);
  //pinMode(5, INPUT_PULLUP);

  Serial1.begin(9600, SERIAL_8N1, rxPin, txPin);

  US.AllocateMemoryForBuffer();
  
 if(!_BMP280.begin(0x76)){
    Serial.println("Failed to initialize BMP280");
    WS2812B.setPixelColor(50,0,0,0);
    WS2812B.show();
    delay(3000);
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
  }
  
  if(!INA219_.Init()){
    Serial.println("Failed to initialize INA219");
    WS2812B.setPixelColor(50,0,0,0);
    WS2812B.show();
    delay(3000);
  }
  
  if(!INA219_.ConfigureSensor()){
      WS2812B.setPixelColor(50,0,0,0);
      WS2812B.show();
      Serial.println("Failed to configure INA219");
      delay(3000);
      //return;
  }
 // EX_01_.ads_reset();
  //EX_01_.ads_write_reg(0x01, 0x00);
  
  
  int state = radio.begin(433.96, 125.0, 9, 7, 0x12, 17);

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
      WS2812B.setPixelColor(0,0,0,0);
      WS2812B.show();
    }
  }

  radio.setDio0Action(OnTransmitDone, RISING);
}


void wyswietlStatystyki() {
  Serial.print(F("Lokalizacja: ")); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Satelity: "));
  Serial.println(gps.satellites.value());
}

void loop() {
  // put your main code here, to run repeatedly:
  WS2812B.setPixelColor(0,0,0,50);
  WS2812B.show();

  MPU.ReadDataIMU();
  AccelData = MPU.GetAccelData();
  GyroData = MPU.GetGyroData();

  /*_EX_Data = EX_01_.get_EX_01_Data();

  Serial.print("R: ");
  Serial.println(_EX_Data.RED_Channel);
  Serial.print("G: ");
  Serial.println(_EX_Data.GREEN_Channel);
  Serial.print("B: ");
  Serial.println(_EX_Data.BLUE_Channel);*/
  
  voltage = INA219_.readBusVoltage();
  temperature = _BMP280.readTemperature();
  presssure = _BMP280.readPressure() / 100;
  altitude = _BMP280.readAltitude();

  US.WriteToBuffer(altitude, (char*)"AL");
  US.WriteToBuffer(temperature, (char*)"TE");
  US.WriteToBuffer(presssure, (char*)"PR");
  US.WriteToBuffer(voltage, (char*)"VO");
  US.WriteToBuffer(GyroData.GX, (char*)"GX");
  US.WriteToBuffer(GyroData.GY, (char*)"GY");
  US.WriteToBuffer(GyroData.GZ, (char*)"GZ");
  US.WriteToBuffer(AccelData.AX, (char*)"AX");
  US.WriteToBuffer(AccelData.AY, (char*)"AY");
  US.WriteToBuffer(AccelData.AZ, (char*)"AZ");

  Serial.println(millis());

  if(!transmitting){
    memcpy(buffer_LoRa, US.GetBuffer(), US.GetBufferSize());
    int state = radio.startTransmit(buffer_LoRa, US.GetBufferSize());
    if(state == RADIOLIB_ERR_NONE){
      transmitting = true;
      Serial.println("TX Success");
    }
    else{
      Serial.print("Failed to TX, ERR CODE: ");
      Serial.println(state);
    }
  }

  Serial.println(millis());
  US.SendBuffer();



 /* while (Serial1.available() > 0) {
    char c = Serial1.read();
    
    if (gps.encode(c)) {
      wyswietlStatystyki();
    }
  }


  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) {
    if (gps.charsProcessed() < 10) {
      Serial.println("NMEA frame error");
    }
    lastCheck = millis();
  }*/
  
  WS2812B.setPixelColor(0,0,0,0);
  WS2812B.show();
  
  

}