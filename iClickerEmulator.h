#ifndef I_CLICKER_EMULATOR_H
#define I_CLICKER_EMULATOR_H

#include "RF24.h"
#include "Arduino.h"


#define MAC_SIZE 3
#define DATA_SIZE 1
#define PACKET_SIZE (MAC_SIZE + DATA_SIZE)

#define CHANNEL 47


class iClickerEmulator
{
public:
    iClickerEmulator(uint16_t _cepin, uint16_t _cspin, uint16_t _irqpin);

    bool beginRecv();

    const uint8_t clickerMasterMAC[MAC_SIZE] = { 0x56, 0x34, 0x12 };

private:
    RF24 _radio;
    const uint16_t _irqpin;
};

#endif
