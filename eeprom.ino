

#include <EEPROM.h>


#define EEPROM_MODE_ADDR 0

void saveMode(char modeValue)
{
  EEPROM.write(EEPROM_MODE_ADDR, modeValue);
}


char getMode()
{
  int8_t modeValue = EEPROM.read(EEPROM_MODE_ADDR);
  return modeValue;
}
