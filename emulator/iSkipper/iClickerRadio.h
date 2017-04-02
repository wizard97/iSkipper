#ifndef I_CLICKER_RADIO_H
#define I_CLICKER_RADIO_H

#include "RFM69.h"

// This is the bitrate of the iclicker
#define RF_BITRATEMSB_152381          0x00
#define RF_BITRATELSB_152381          0xD2

// frequency dev
#define RF_FDEVMSB_222833              0x0E
#define RF_FDEVLSB_222833               0x45

//sync bytes
#define RF_SYNC_BYTE1_VALUE_IC          0x85
#define RF_SYNC_BYTE2_VALUE_IC          0x85
#define RF_SYNC_BYTE3_VALUE_IC          0x85

//packet length
#define PAYLOAD_LENGTH_IC               0x05

// threshold for triggerins fifo transmit interrupt
#define RF_FIFOTHRESH_TXSTART_FIFOTHRESH_IC 0x04

class iClickerRadio : public RFM69
{
public:
    iClickerRadio(uint8_t slaveSelectPin=RF69_SPI_CS,
        uint8_t interruptPin=RF69_IRQ_PIN, bool isRFM69HW=false, uint8_t interruptNum=RF69_IRQ_NUM);

    bool begin();
};

#endif
