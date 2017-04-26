#ifndef I_CLICKER_RADIO_INCLUDES_H
#define I_CLICKER_RADIO_INCLUDES_H


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

typedef enum iClickerChannelType
{
    CHANNEL_SEND = 0, //act as a clicker
    CHANNEL_RECV = 1, // act as base station
} iClickerChannelType_t;


typedef struct iClickerChannel
{
    uint8_t send[3];
    uint8_t recv[3];
} iClickerChannel_t;


namespace iClickerChannels
{
    // MSB to LSB RegFrf

    const iClickerChannel_t AA =
    {
        { 0xE5, 0x40, 0x00 },
        { 0xE1, 0xE0, 0x00 },
    };


    const iClickerChannel_t AB =
    {
        { 0xE4, 0x40, 0x00 },
        { 0xE2, 0x20, 0x00 },
    };


    const iClickerChannel_t AC =
    {
        { 0xE4, 0x80, 0x00 },
        { 0xE2, 0x80, 0x00 },
    };


    const iClickerChannel_t AD =
    {
        { 0xE4, 0xC0, 0x00 },
        { 0xE2, 0xE0, 0x00 },
    };


    const iClickerChannel_t BA =
    {
        { 0xE5, 0x00, 0x00 },
        { 0xE3, 0x20, 0x00 },
    };


    const iClickerChannel_t BB =
    {
        { 0xE5, 0xC0, 0x00 },
        { 0xE3, 0x80, 0x00 },
    };


    const iClickerChannel_t BC =
    {
        { 0xE6, 0x00, 0x00 },
        { 0xE3, 0xE0, 0x00 },
    };


    const iClickerChannel_t BD =
    {
        { 0xE6, 0x40, 0x00 },
        { 0xE4, 0x20, 0x00 },
    };


    const iClickerChannel_t CA =
    {
        { 0xE6, 0x80, 0x00 },
        { 0xE4, 0x80, 0x00 },
    };


    const iClickerChannel_t CB =
    {
        { 0xE6, 0xC0, 0x00 },
        { 0xE4, 0xE0, 0x00 },
    };


    const iClickerChannel_t CC =
    {
        { 0xE2, 0xC0, 0x00 },
        { 0xE5, 0x20, 0x00 },
    };


    const iClickerChannel_t CD =
    {
        { 0xE3, 0x00, 0x00 },
        { 0xE5, 0x80, 0x00 },
    };


    const iClickerChannel_t DA =
    {
        { 0xE2, 0x60, 0x00 },
        { 0xE5, 0xC0, 0x00 },
    };


    const iClickerChannel_t DB =
    {
        { 0xE3, 0x40, 0x00 },
        { 0xE6, 0x20, 0x00 },
    };


    const iClickerChannel_t DC =
    {
        { 0xE3, 0xC0, 0x00 },
        { 0xE6, 0x80, 0x00 },
    };


    const iClickerChannel_t DD =
    {
        { 0xE3, 0x80, 0x00 },
        { 0xE6, 0xC0, 0x00 },
    };


}
/*
// here are the possible channels
namespace iClickerChannels
{
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

    // Channel DB (Tx: RF_FRFMSB_909, Rx: ~920)
    const iClickerChannel_t DB =
    {
        { 0xE3, 0x40, 0x00 },
        { 0xE6, 0x20, 0x00 },
    };

}
*/
#endif
