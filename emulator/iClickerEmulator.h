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
    //encodes iclicker id for transmission
    static void id_encode(uint8_t *id, uint8_t *ret);
    // decodes a transmitted id
    static void id_decode(uint8_t *id, uint8_t *ret);
    //checks to make sure valid iclicker id
    static bool valid_id(uint8_t *id);

private:
    RFM69 _radio;
    const uint8_t _irqpin;
}

#endif
