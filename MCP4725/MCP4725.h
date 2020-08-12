#include "Arduino.h"
#include <Wire.h>

#define W_DAC (0x40) //Writes only to the DAC 
#define W_DAC_EEPROM (0x60) // Writes to DAC and EEPROM
#define W_DAC_FAST (0x0)

class MCP4725{
  public:
    MCP4725();
    void begin(uint8_t address);
    void writeDAC(uint16_t output, bool write_both);
    void writeDACFast(uint16_t output);
    void enableFastMode();
    void disableFastMode();
    uint16_t readValEEPROM();
    uint16_t readCurrValDAC();
   
    private:
      uint8_t iSquare2address;
};
