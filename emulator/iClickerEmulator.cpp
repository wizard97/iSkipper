#include "iClickerEmulator.h"

iClickerEmulator::iClickerEmulator(uint8_t _cepin, uint8_t _cspin, uint8_t _irqpin)
: _radio(_cepin, _cspin), _irqpin(_irqpin)
{
    pinMode(_irqpin, OUTPUT);
}

bool iClickerEmulator::begin()
{
    _radio.begin();
    _radio.setCRCLength(RF24_CRC_16 );
    _radio.powerUp();
}
