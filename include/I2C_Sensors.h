#include <Arduino.h>
#include <Wire.h>

#define BMP280_ADDR 0x76
#define MPU6050_ADDR 0x68

namespace I2C_Sensors{

class INA219{
    public:
    uint16_t shuntvoltage;
    uint16_t busvoltage;
    uint16_t current_mA;
    uint16_t loadvoltage;
    uint16_t power_mW;

    uint16_t readShuntVoltage();
    uint16_t readBusVoltage();
    uint16_t readCurrent_mA();
    uint16_t readLoadVoltage();
    uint16_t readPower_mW();

};

}