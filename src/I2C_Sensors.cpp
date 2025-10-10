#include "I2C_Sensors.h"

using namespace I2C_Sensors;

INA219::INA219(uint8_t addr, float ResitorValue, float MaxExpectedCurrent){
    INA_addr = addr;
    ResistorValue_ = ResitorValue;
    MaxExpectedCurrent_ = MaxExpectedCurrent;
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
All values after coversion are expressed in SI units: [V], [V], [A], [W].
*/

float INA219::readShuntVoltage(){
    float result = (float)(int16_t)readByte(0x01) * 0.00001f;
    return result;
}

float INA219::readBusVoltage(){
    //Result in volts
    float result = (float)(readByte(0x02) >> 3) * 0.004f;
    return result;
}

float INA219::readPower(){
    //result in 
    float result = (float)readByte(0x03) * Current_LSB * 20.0f;
    return result;
}

float INA219::readCurrent(){
    //Result in A
    float result = (float)(int16_t)readByte(0x04) * Current_LSB;
    return result;
}

