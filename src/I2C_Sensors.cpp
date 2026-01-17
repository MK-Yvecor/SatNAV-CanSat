#include "I2C_Sensors.h"

using namespace I2C_Sensors;

// INA219 class constructor
INA219::INA219(uint8_t addr, float ResitorValue, float MaxExpectedCurrent){
    INA_addr = addr;
    ResistorValue_ = ResitorValue;
    MaxExpectedCurrent_ = MaxExpectedCurrent;
}

//Initialization check
bool INA219::Init(){
    Wire.beginTransmission(INA_addr);
    uint8_t errCode = Wire.endTransmission();
    if(errCode == 0){
        return true;
    }
    else
    {
        return false;
    }    
}

bool INA219::ConfigureSensor(){
    
   Current_LSB = MaxExpectedCurrent_ / 32768.0f;
   uint16_t Calibration_Value = uint16_t(0.04096f /( Current_LSB * ResistorValue_));
   uint8_t u_ByteCalibrationValue = Calibration_Value >> 8;
   uint8_t l_ByteCalibrationValue = Calibration_Value & 0xFF;

   Wire.beginTransmission(INA_addr);
   Wire.write(0x05);
   Wire.write(u_ByteCalibrationValue);
   Wire.write(l_ByteCalibrationValue);

   uint8_t errCode = Wire.endTransmission();

    if(errCode == 0){
        return true;
    }
    else
    {
        return false;
    }    

}

uint16_t INA219::readByte(uint8_t regAddress){

    uint16_t rawValue;
    uint8_t upperByte;
    uint8_t lowerByte;

    Wire.beginTransmission(INA_addr);
    Wire.write(regAddress);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)INA_addr ,(uint8_t)2);

    if(Wire.available()<2){
        return 0;
    }

    upperByte = Wire.read();
    lowerByte = Wire.read();

    rawValue = upperByte;
    rawValue = rawValue << 8;
    rawValue = rawValue | lowerByte;

    return rawValue;
}

/*
Calculation formulas are available in INA219 datasheet page 12 and 13: https://www.ti.com/lit/ds/symlink/ina219.pdf?ts=1760067833070&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FINA219
All values after conversion are expressed in SI units: [V], [V], [A], [W].
*/

//result in V
float INA219::readShuntVoltage(){
    float result = (float)(int16_t)readByte(0x01) * 0.00001f;
    return result;
}

//Result in volts
float INA219::readBusVoltage(){
    float result = (float)(readByte(0x02) >> 3) * 0.004f;
    return result;
}

//result in W
float INA219::readPower(){
    float result = (float)readByte(0x03) * Current_LSB * 20.0f;
    return result;
}

//Result in A
float INA219::readCurrent(){
    float result = (float)(int16_t)readByte(0x04) * Current_LSB;
    return result;
}



/*

   EX_01 CLASS METHODS

*/


void EX_01::i2c_delay() { delayMicroseconds(I2C_DELAY_US); }
void EX_01::sda_high() { pinMode(SDA, INPUT_PULLUP); }
void EX_01::sda_low()  { pinMode(SDA, OUTPUT); digitalWrite(SDA, LOW); }
void EX_01::scl_high() { pinMode(SCL, INPUT_PULLUP); }
void EX_01::scl_low()  { pinMode(SCL, OUTPUT); digitalWrite(SCL, LOW); }


void EX_01::i2c_start() {
  sda_high(); scl_high(); i2c_delay();
  sda_low();  i2c_delay();
  scl_low();
}

void EX_01::i2c_stop() {
  sda_low();  scl_high(); i2c_delay();
  sda_high(); i2c_delay();
}


bool EX_01::i2c_write(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    (data & 0x80) ? sda_high() : sda_low();
    scl_high(); i2c_delay();
    scl_low();  i2c_delay();
    data <<= 1;
  }
  sda_high();
  scl_high(); i2c_delay();
  bool ack = (digitalRead(SDA) == 0);
  scl_low();
  return ack;
}

uint8_t EX_01::i2c_read(bool send_ack) {
  uint8_t data = 0;
  sda_high();
  for (int i = 0; i < 8; i++) {
    data <<= 1;
    scl_high(); i2c_delay();
    if (digitalRead(SDA)) data |= 1;
    scl_low(); i2c_delay();
  }
  send_ack ? sda_low() : sda_high();
  scl_high(); i2c_delay();
  scl_low();
  sda_high();
  return data;
}

void EX_01::ads_write_reg(uint8_t reg, uint8_t val) {
  i2c_start();
  i2c_write(ADS_ADDR << 1); 
  i2c_write(0x40 | (reg << 2)); 
  i2c_write(val);
  i2c_stop();
}

void EX_01::ads_start_conversion() {
  i2c_start();
  i2c_write(ADS_ADDR << 1);
  i2c_write(0x08);
  i2c_stop();
}

void EX_01::ads_reset() {
  i2c_start();
  i2c_write(ADS_ADDR << 1);
  i2c_write(0x06); 
  i2c_stop();
  delay(10);
}

int16_t EX_01::ads_read_data_safe() {

  i2c_start();
  i2c_write(ADS_ADDR << 1); 
  i2c_write(0x10);      

  i2c_stop();

  i2c_start();
  i2c_write((ADS_ADDR << 1) | 1); 
  uint8_t msb = i2c_read(true);   
  uint8_t lsb = i2c_read(false);  
  i2c_stop();

  return (int16_t)((msb << 8) | lsb);
}

float EX_01::get_voltage(uint8_t mux_config) {

  ads_write_reg(0x00, mux_config);

  ads_start_conversion();

  delay(60); 

  int16_t raw = ads_read_data_safe();

  return (raw * 2.048f) / 32768.0f;
}