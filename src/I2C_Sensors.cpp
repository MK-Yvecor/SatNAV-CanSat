#include "I2C_Sensors.h"

using namespace I2C_Sensors;

INA219::INA219(uint8_t addr, int ShuntResitorValue){
    INA_addr = addr;
    ShuntResistorValue_ = ShuntResitorValue;
}

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
    
   float Current_LSB = 2.0 / 32768.0;
   uint16_t Calibration_Value = uint16_t((0.04096 / Current_LSB) * ShuntResistorValue_);
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
    Wire.requestFrom(INA_addr ,2);

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

float INA219::readShuntVoltage(){
    float result = (float)readByte(0x01) * 0.00001;
    return result;
}

float INA219::readBusVoltage(){
    float result = (float)readByte(0x02) * 0.004;
    return result;
}


float INA219::readPower_mW(){
    float result = (float)readByte(0x03);
    return result;
}

float INA219::readCurrent_mA(){
    float Current_LSB = 2.0 / 32768.0;
    float result = (float)(int16_t)readByte(0x04) * Current_LSB * 1000.0;;
    return result;
}

