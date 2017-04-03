#ifndef I_CLICKER_EMULATOR_H
#define I_CLICKER_EMULATOR_H

#include "iClickerRadio.h"
#include "Arduino.h"


class iClickerEmulator
{
public:
    iClickerEmulator(uint8_t _cspin, uint8_t _irqpin);
    bool begin();

    //encodes iclicker id for transmission
    static void id_encode(uint8_t *id, uint8_t *ret);
    // decodes a transmitted id
    static void id_decode(uint8_t *id, uint8_t *ret);
    //checks to make sure valid iclicker id
    static bool valid_id(uint8_t *id);

private:
    iClickerRadio _radio;
};

#endif
