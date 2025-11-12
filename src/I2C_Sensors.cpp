#include "I2C_Sensors.h"
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>

using namespace I2C_Sensors;

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

// INA219 class constructor
INA219::INA219(uint8_t addr, float ResitorValue, float MaxExpectedCurrent){
    INA_addr = addr;
    ResistorValue_ = ResitorValue;
    MaxExpectedCurrent_ = MaxExpectedCurrent;
}

// BMP280 class constructor
class BMP280{
    public:

        bool Init();
        float ReadTemperature();
        float ReadPressure();

    private:
        uint8_t BMP280_addr = 0x76;
};

class MPU6050{
    public:

        bool Init();
        float ReadAccelerationX();
        float ReadAccelerationY();
        float ReadAccelerationZ();
        float ReadGyroX();
        float ReadGyroY();
        float ReadGyroZ();

    private:
        uint8_t MPU6050_addr = 0x68;
};


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

bool BMP280::Init(){
    if (!bmp.begin()) {
        Serial.println(F("BMP280_INITIALIZATION_ERROR!!!"));
        return false;
    }
    else{
        Serial.println(F("BMP280_INITIALIZATION_OK!!!"));
        return true;    
    }

}

float BMP280::ReadTemperature(){
    float temperature = bmp.readTemperature();
    return temperature;
}

float BMP280::ReadPressure(){
    float pressure = bmp.readPressure();
    return pressure;
}

bool MPU6050::Init(){
    if (!mpu.begin()) {
        Serial.println("MPU_INITIALIZATIOM_ERROR");
        return false;
    }
    else{
        Serial.println("MPU_INITIALIZATIOM_OK");
        return true;
    }
}

float MPU6050::ReadAccelerationX(){
    sensors_event_t a;
    mpu.getEvent(&a, nullptr, nullptr);
    float accelerationX = a.acceleration.x;
    return accelerationX;
}

float MPU6050::ReadAccelerationY(){
    sensors_event_t a;
    mpu.getEvent(&a, nullptr, nullptr);
    float accelerationY = a.acceleration.y;
    return accelerationY;
}

float MPU6050::ReadAccelerationZ(){
    sensors_event_t a;
    mpu.getEvent(&a, nullptr, nullptr);
    float accelerationZ = a.acceleration.z;
    return accelerationZ;
}

float MPU6050::ReadGyroX(){
    sensors_event_t g;
    mpu.getEvent(nullptr, &g, nullptr);
    float gyroX = g.gyro.x;
    return gyroX;
}

float MPU6050::ReadGyroY(){
    sensors_event_t g;
    mpu.getEvent(nullptr, &g, nullptr);
    float gyroY = g.gyro.y;
    return gyroY;
}

float MPU6050::ReadGyroZ(){
    sensors_event_t g;
    mpu.getEvent(nullptr, &g, nullptr);
    float gyroZ = g.gyro.z;
    return gyroZ;
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

