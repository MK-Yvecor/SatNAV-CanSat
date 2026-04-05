#include <Arduino.h>
#include <EEPROM.h>

void lora_band(int band_value){

    UartSend(int band_value, 'LB');

    if (Serial.available() > 0) {
        byte odebrany = Serial.read();

        if (odebrany == 0xFF) {

            while (Serial.available() < 2) {}
            char LB_header[3];
            LB_header[0] = Serial.read();
            LB_header[1] = Serial.read();
            LB_header[2] = '\0';

            if (strcmp(LB_header, "LB") == 0) {

                while (Serial.available() < 2) {}
                char datatype = Serial.read();
                byte dataSize = Serial.read();

                byte data[16];

                if (dataSize <= 16) {
                    while (Serial.available() < dataSize + 1) {} 

                    for (int i = 0; i < dataSize; i++) {
                        data[i] = Serial.read();
                    }

                    byte endByte = Serial.read();

                    if (endByte == 0xF0) {
                        if (datatype == 'i' && dataSize == sizeof(int)) {
                            int value;
                            memcpy(&value, data, sizeof(int));
                        }
                    }
                }
            }
        }
    }

    EEPROM.write(address, value);
}   

