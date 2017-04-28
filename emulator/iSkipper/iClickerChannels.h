#ifndef ICLICKER_CHANNELS_H
#define ICLICKER_CHANNELS_H

#include <Arduino.h>


#define NUM_ICLICKER_CHANNELS (sizeof(iClickerChannels::channels)/sizeof(iClickerChannel_t))

typedef uin16_t iClickerChannelMask_t;

typedef struct iClickerChannel
{
    uint8_t send[3];
    uint8_t recv[3];
    iClickerChannelMask_t mask;
} iClickerChannel_t;



namespace iClickerChannels
{
    // MSB to LSB RegFrf

    const iClickerChannel_t AA =
    {
        { 0xE5, 0x40, 0x00 }, //send
        { 0xE1, 0xE0, 0x00 }, //recv
        0x01, //mask
    };


    const iClickerChannel_t AB =
    {
        { 0xE4, 0x40, 0x00 }, //send
        { 0xE2, 0x20, 0x00 }, //recv
        0x02, //mask
    };


    const iClickerChannel_t AC =
    {
        { 0xE4, 0x80, 0x00 }, //send
        { 0xE2, 0x80, 0x00 }, //recv
        0x04, //mask
    };


    const iClickerChannel_t AD =
    {
        { 0xE4, 0xC0, 0x00 }, //send
        { 0xE2, 0xE0, 0x00 }, //recv
        0x08, //mask
    };


    const iClickerChannel_t BA =
    {
        { 0xE5, 0x00, 0x00 }, //send
        { 0xE3, 0x20, 0x00 }, //recv
        0x10, //mask
    };


    const iClickerChannel_t BB =
    {
        { 0xE5, 0xC0, 0x00 }, //send
        { 0xE3, 0x80, 0x00 }, //recv
        0x20, //mask
    };


    const iClickerChannel_t BC =
    {
        { 0xE6, 0x00, 0x00 }, //send
        { 0xE3, 0xE0, 0x00 }, //recv
        0x40, //mask
    };


    const iClickerChannel_t BD =
    {
        { 0xE6, 0x40, 0x00 }, //send
        { 0xE4, 0x20, 0x00 }, //recv
        0x80, //mask
    };


    const iClickerChannel_t CA =
    {
        { 0xE6, 0x80, 0x00 }, //send
        { 0xE4, 0x80, 0x00 }, //recv
        0x100, //mask
    };


    const iClickerChannel_t CB =
    {
        { 0xE6, 0xC0, 0x00 }, //send
        { 0xE4, 0xE0, 0x00 }, //recv
        0x200, //mask
    };


    const iClickerChannel_t CC =
    {
        { 0xE2, 0xC0, 0x00 }, //send
        { 0xE5, 0x20, 0x00 }, //recv
        0x400, //mask
    };


    const iClickerChannel_t CD =
    {
        { 0xE3, 0x00, 0x00 }, //send
        { 0xE5, 0x80, 0x00 }, //recv
        0x800, //mask
    };


    const iClickerChannel_t DA =
    {
        { 0xE2, 0x60, 0x00 }, //send
        { 0xE5, 0xC0, 0x00 }, //recv
        0x1000, //mask
    };


    const iClickerChannel_t DB =
    {
        { 0xE3, 0x40, 0x00 }, //send
        { 0xE6, 0x20, 0x00 }, //recv
        0x2000, //mask
    };


    const iClickerChannel_t DC =
    {
        { 0xE3, 0xC0, 0x00 }, //send
        { 0xE6, 0x80, 0x00 }, //recv
        0x4000, //mask
    };


    const iClickerChannel_t DD =
    {
        { 0xE3, 0x80, 0x00 }, //send
        { 0xE6, 0xC0, 0x00 }, //recv
        0x8000, //mask
    };

    const iClickerChannel_t channels[] =
        {AA, AB, AC, AD, BA, BB, BC, BD, CA, CB, CC, CD, DA, DB, DC, DD};


}
#endif
