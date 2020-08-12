#include "Arduino.h"
#include "Wire.h"
#include "MCP4725.h"

/**
* This is a library for MCP4725 DAC.
*/

MCP4725::MCP4725() {
}

void MCP4725::begin(uint8_t address) {
    iSquare2address = address;
    Wire.begin();
}

void MCP4725::enableFastMode(){
    TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency for the ATMega to 400kHz
}

void MCP4725::disableFastMode(){
    TWBR = ((F_CPU / 100000L) - 16) / 2; // Set I2C frequency for the ATMega to 100kHz 
}


uint16_t MCP4725::readCurrValDAC() {
    uint8_t up_8;
    uint8_t low_8;
    Wire.requestFrom(iSquare2address, (uint8_t) 5);
    while (Wire.available() != 5){
        Serial.println("DAC is not available");
    }
    Wire.read();//current status
    up_8 = Wire.read(); //upper 8-bit
    low_8 = Wire.read(); //lower 8-bit
    Wire.read();// EEPROM DATA  [X PD1 PD0 X D11 D10 D9 D8]
    Wire.read(); //EEPROM DATA  [D7 D6 D5 D4 D3 D2 D1 D0]
    return (up_8 << 4) | (low_8 >> 4);
}

uint16_t MCP4725::readValEEPROM(){
  uint8_t isAvailable;
  uint8_t up_8_EEPROM;
  uint8_t low_8_EEPROM;
  Wire.requestFrom(iSquare2address, (uint8_t) 5);
  while (Wire.available() != 5){
    Serial.println("EEPROM is not available now");
  }
  isAvailable = Wire.read() >> 7;
  if (isAvailable != 1){
    Wire.read(); //upper DAC Register byte
    Wire.read(); // lower DAC Register byte
    up_8_EEPROM = Wire.read();
    low_8_EEPROM = Wire.read();
    // upper 8 bits is [X PD1 PD0 X D11 D10 D9 D8]
    up_8_EEPROM = up_8_EEPROM & (0xF0); //to clear first 4 bit.
    // [0 0 0 0 D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0]
    return (up_8_EEPROM << 8) | low_8_EEPROM;
  } else {
    Serial.println("Not Available. Writing to EEPROM...");
    return readValEEPROM();
  }
}

void MCP4725::writeDAC(uint16_t output, bool write_both){ 
  uint8_t upper_byte;
  uint8_t lower_byte;
  Wire.beginTransmission(iSquare2address);
  if (write_both){
    Wire.write(W_DAC_EEPROM);
    //The 12-bit output is in 16-bit form [0, 0, 0, 0, D11. D10. D9. D8. D7. D6. D5. D4, D3. D2. D1 .D0]
    upper_byte = (output >> 4); // upper_byte =  [D11 D10 D9 D8 D7 D6 D5 D4]
    lower_byte = (output << 12); // lower_byte = [D3 D2 D1 D0 0 0 0 0]
    lower_byte = (lower_byte >> 8); // lower_byte = [0 0 0 0 0 0 0 0 D3 D2 D1 D0 0 0 0 0]
    Wire.write(upper_byte);
    Wire.write(lower_byte);
    Wire.endTransmission();
  } else {
      Wire.write(W_DAC);
      upper_byte = (output >> 4); // upper_byte =  [D11 D10 D9 D8 D7 D6 D5 D4]
      lower_byte = (output << 12); // lower_byte = [D3 D2 D1 D0 0 0 0 0]
      lower_byte = (lower_byte >> 8); // lower_byte = [0 0 0 0 0 0 0 0 D3 D2 D1 D0 0 0 0 0]
      Wire.write(upper_byte);
      Wire.write(lower_byte);
      Wire.endTransmission();
  }
}

  void MCP4725::writeDACFast(uint16_t output){
    uint8_t upper_byte;
    uint8_t lower_byte;
    Wire.beginTransmission(iSquare2address);
    //output is a 12-bit value in 16-bit form, namely: [0, 0, 0, 0, D11, D10, D9, D8, D7, D6, D5, D4, D3, D2, D1, D0]
    upper_byte = (output>>8); //upper_byte = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, D11, D10, D9, D8]
    lower_byte = output;
    Wire.write(upper_byte);  // Upper data bits [0, 0, 0, 0, D11, D10, D9, D8]       
    Wire.write(lower_byte);  // Lower data = [D7, D6, D5, D4, D3, D2, D1, D0]
    Wire.endTransmission();
  }
