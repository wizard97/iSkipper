#ifndef I_CLICKER_EMULATOR_H
#define I_CLICKER_EMULATOR_H

#include "RF24.h"
#include "Arduino.h"

class iClickerEmulator
{
public:
    iClickerEmulator(uint8_t _cepin, uint8_t _cspin, uint8_t _irqpin);

private:
    RF24 _radio;
    const uint8_t _irqpin;
}

#endif
