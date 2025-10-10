#include <Arduino.h>
#include <Wire.h>

//list of default addresses of I2C devices used in SatNAV. If the addresses are diffrent you can specify a custom address in class constructor

#define BMP280_ADDR 0x76
#define MPU6050_ADDR 0x68
#define INA219_ADDR 0x40

namespace I2C_Sensors{
    class INA219 {

    public:
    INA219(uint8_t addr_, float ResitorValue, float MaxExpectedCurrent);
    bool Init();
    bool ConfigureSensor();
    uint16_t readByte(uint8_t regAddress);
    uint16_t writeByte(uint8_t regAddress);
    float readShuntVoltage();
    float readBusVoltage();
    float readCurrent();
    float readPower();

    private:
    uint8_t INA_addr;
    //Expressed in Ohms
    float ResistorValue_;
    //Expressed in Amps
    float MaxExpectedCurrent_;
    //
    float Current_LSB;
};

}