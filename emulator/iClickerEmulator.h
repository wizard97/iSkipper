#ifndef I_CLICKER_EMULATOR_H
#define I_CLICKER_EMULATOR_H

#include "RFM69.h"
#include "Arduino.h"

#define NODE_ID 0
#define REG_SYNC_VAL_2 0x01

class iClickerEmulator
{
public:
    iClickerEmulator(uint8_t _cspin, uint8_t _irqpin);

private:
    RF24 _radio;
    const uint8_t _irqpin;
}

#endif
