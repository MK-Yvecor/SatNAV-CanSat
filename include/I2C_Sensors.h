#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

//list of default addresses of I2C devices used in SatNAV. If the addresses are diffrent you can specify a custom address in class constructor

#define BMP280_ADDR 0x76
#define MPU6050_ADDR 0x68
#define INA219_ADDR 0x40

namespace I2C_Sensors{
    //Class for INA219 measurements
    class INA219 {

    public:
    // INA219 class constructor
    INA219(uint8_t addr_, float ResitorValue, float MaxExpectedCurrent);
    //Initialization check
    bool Init();
    //Writes all needed values to the configuration register
    bool ConfigureSensor();
    // Universal function to read 2 bytes of incoming data from the sensor
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
    //Current LSB value
    float Current_LSB;
};



class EX_01{

    
    public: 
    EX_01(uint8_t _ADS_ADDR, uint8_t _SDA, uint8_t _SCL, uint8_t _I2C_DELAY_US) {

        ADS_ADDR = _ADS_ADDR;
        SDA = _SDA;
        SCL = _SCL;
        I2C_DELAY_US = _I2C_DELAY_US;
    }
    
    float get_voltage(uint8_t mux_config);
    void ads_write_reg(uint8_t reg, uint8_t val);
    void ads_reset();
    
    
    private:
    uint8_t ADS_ADDR;
    uint8_t SDA;
    uint8_t SCL;
    uint8_t I2C_DELAY_US;
    
    void i2c_delay();
    void sda_high();
    void sda_low();
    void scl_high();
    void scl_low();
    
    
    void i2c_start();
    void i2c_stop();
    
    
    bool i2c_write(uint8_t data);
    uint8_t i2c_read(bool send_ack);
    
    
    void ads_start_conversion();
    
    
    int16_t ads_read_data_safe();
    
};

}