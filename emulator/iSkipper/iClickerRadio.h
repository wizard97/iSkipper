#ifndef I_CLICKER_RADIO_H
#define I_CLICKER_RADIO_H

#include <RingBufCPP.h>
#include <Arduino.h>


// Can buffer up to 20 packets
#define PACKET_BUF_SIZE 20

// This is the bitrate of the iclicker (152381 bits/s)
#define RF_BITRATEMSB_152381          0x00
#define RF_BITRATELSB_152381          0xD2

// frequency dev (222833 Hz)
#define RF_FDEVMSB_222833              0x0E
#define RF_FDEVLSB_222833               0x45

//sync bytes
#define RF_SYNC_BYTE1_VALUE_IC          0x85
#define RF_SYNC_BYTE2_VALUE_IC          0x85
#define RF_SYNC_BYTE3_VALUE_IC          0x85

//packet length
#define PAYLOAD_LENGTH_SEND               0x05
#define PAYLOAD_LENGTH_RECV               0x07

// threshold for triggerins fifo transmit interrupt
#define RF_FIFOTHRESH_TXSTART_FIFOTHRESH_IC 0x04

#define ANSWER_A 0xB2
#define ANSWER_B 0xB6
#define ANSWER_C 0xBE
#define ANSWER_D 0xBF
#define ANSWER_E 0xBB
#define ANSWER_PING 0xB3 //pings reciever

//RegSyncValue1-8 for sending is:
// 0x85, 0x85, 0x85, 0, 0...

//RegSyncValue1-8 for recving is:
// encoded_id[0], encoded_id[1], encoded_id[2], 0, 0...

// Note: sending packets are 5 bytes
//  Recieving packets from base are 7 bytes
namespace iClickerChannels
{
    typdef struct iClickerChannel
    {
        uint8_t send[3];
        uint8_t recv[3];
    } iClickerChannel_t;
    // MSB to LSB RegFrf

    // Channel AA (Tx: RF_FRFMSB_917, Rx: ~903)
    const iClickerChannel_t AA =
    {
        { 0xE5, 0x40, 0x00 },
        { 0xE1, 0xE0, 0x00 },
    };

    // Channel AB (Tx: RF_FRFMSB_913, Rx: ~904)
    const iClickerChannel_t AB =
    {
        { 0xE4, 0x40, 0x00 },
        { 0xE2, 0x20, 0x00 },
    };

    // Channel AC (Tx: RF_FRFMSB_914, Rx: )
    const iClickerChannel_t AC =
    {
        { 0xE4, 0x80, 0x00 },
        { 0x00, 0x00, 0x00 }, //todo
    };

    // Channel AD (Tx: RF_FRFMSB_915, Rx: )
    const iClickerChannel_t AD =
    {
        { 0xE4, 0xC0, 0x00 },
        { 0x00, 0x00, 0x00 }, //todo
    };

    // Channel BA (Tx: RF_FRFMSB_916, Rx: )
    const iClickerChannel_t BA =
    {
        { 0xE5, 0x00, 0x00 },
        { 0x00, 0x00, 0x00 }, //todo
    };


    // Channel BB (Tx: RF_FRFMSB_919, Rx: RF_FRFMSB_910)
    const iClickerChannel_t BB =
    {
        { 0xE5, 0xC0, 0x00 },
        { 0xE3, 0x80, 0x00 },
    };

    // Channel BC (Tx: RF_FRFMSB_920, Rx: )
    const iClickerChannel_t BC =
    {
        { 0xE6, 0x00, 0x00 },
        { 0x00, 0x00, 0x00 }, //todo
    };

    // Channel BD (Tx: RF_FRFMSB_921, Rx: )
    const iClickerChannel_t BD =
    {
        { 0xE6, 0x40, 0x00 },
        { 0x00, 0x00, 0x00 }, //todo
    };

    // Channel DD (Tx: RF_FRFMSB_909, Rx: ~920)?
    const iClickerChannel_t DD =
    {
        { 0xE3, 0x40, 0x00 },
        { 0xE6, 0x20, 0x00 },
    };

}

typedef struct iClickerAnswerPacket
{
    uint8_t encoded_id[4];
    uint8_t answer;
} iClickerAnswerPacket_t;

class iClickerRadio : protected RFM69
{
public:


    iClickerRadio(uint8_t slaveSelectPin=RF69_SPI_CS, uint8_t interruptPin=RF69_IRQ_PIN, bool isRFM69HW=false, uint8_t interruptNum=RF69_IRQ_NUM);

    virtual bool initialize(uint8_t freqBand, uint8_t ID, uint8_t networkID=1);


protected:

    iClickerChannel::iClickerChannel_t *_chan;
    bool _clickerMode;

};

#endif
