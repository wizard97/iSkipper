#ifndef I_CLICKER_RADIO_H
#define I_CLICKER_RADIO_H

// This is the bitrate of the iclicker
#define RF_BITRATEMSB_152381          0x00
#define RF_BITRATELSB_152381          0xD2

// frequency dev
#define RF_FDEVMSB_222833              0x0E
#define RF_FDEVLSB_222833               0x45

#define RF_SYNC_BYTE1_VALUE_IC          0x85
#define RF_SYNC_BYTE2_VALUE_IC          0x85
#define RF_SYNC_BYTE3_VALUE_IC          0x85

#define PAYLOAD_LENGTH_IC               0x05

#define RF_FIFOTHRESH_TXSTART_FIFOTHRESH_IC 0x04

class iClickerRadio : public RFM69
{
public:
    iClickerRadio();

    bool begin();
};

#endif
