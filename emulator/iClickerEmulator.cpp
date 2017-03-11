#include "iClickerEmulator.h"

iClickerEmulator::iClickerEmulator(uint8_t _cspin, uint8_t _irqpin)
: _radio(_cspin, _irqpin)
{

}

bool iClickerEmulator::begin()
{
    _radio.initilize(RF69_915MHZ, NODE_ID, REG_SYNC_VAL_2);
    _radio.setCRCLength(RF24_CRC_16 );
    _radio.powerUp();
}
