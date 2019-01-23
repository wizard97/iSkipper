#ifndef ICLICKER_CHANNELS_H
#define ICLICKER_CHANNELS_H

#include <Arduino.h>

// What sort of moron though it would be a good idea to make a macro that gets included everywhere be named AC ?!?!?!
// WTF samd21g18a.h
#undef AC

#define NUM_ICLICKER_CHANNELS (sizeof(iClickerChannels::channels)/sizeof(iClickerChannel))

typedef uint16_t iClickerChannelMask_t;

struct iClickerChannel
{
    uint8_t send[3];
    uint8_t recv[3];
    iClickerChannelMask_t mask;
};



namespace iClickerChannels
{
    // MSB to LSB RegFrf

    const iClickerChannel AA =
    {
        { 0xE5, 0x40, 0x00 }, //send
        { 0xE1, 0xE0, 0x00 }, //recv
        0x01, //mask
    };


    const iClickerChannel AB =
    {
        { 0xE4, 0x40, 0x00 }, //send
        { 0xE2, 0x20, 0x00 }, //recv
        0x02, //mask
    };


    const iClickerChannel AC =
    {
        { 0xE4, 0x80, 0x00 }, //send
        { 0xE2, 0x80, 0x00 }, //recv
        0x04, //mask
    };


    const iClickerChannel AD =
    {
        { 0xE4, 0xC0, 0x00 }, //send
        { 0xE2, 0xE0, 0x00 }, //recv
        0x08, //mask
    };


    const iClickerChannel BA =
    {
        { 0xE5, 0x00, 0x00 }, //send
        { 0xE3, 0x20, 0x00 }, //recv
        0x10, //mask
    };


    const iClickerChannel BB =
    {
        { 0xE5, 0xC0, 0x00 }, //send
        { 0xE3, 0x80, 0x00 }, //recv
        0x20, //mask
    };


    const iClickerChannel BC =
    {
        { 0xE6, 0x00, 0x00 }, //send
        { 0xE3, 0xE0, 0x00 }, //recv
        0x40, //mask
    };


    const iClickerChannel BD =
    {
        { 0xE6, 0x40, 0x00 }, //send
        { 0xE4, 0x20, 0x00 }, //recv
        0x80, //mask
    };


    const iClickerChannel CA =
    {
        { 0xE6, 0x80, 0x00 }, //send
        { 0xE4, 0x80, 0x00 }, //recv
        0x100, //mask
    };


    const iClickerChannel CB =
    {
        { 0xE6, 0xC0, 0x00 }, //send
        { 0xE4, 0xE0, 0x00 }, //recv
        0x200, //mask
    };


    const iClickerChannel CC =
    {
        { 0xE2, 0xC0, 0x00 }, //send
        { 0xE5, 0x20, 0x00 }, //recv
        0x400, //mask
    };


    const iClickerChannel CD =
    {
        { 0xE3, 0x00, 0x00 }, //send
        { 0xE5, 0x80, 0x00 }, //recv
        0x800, //mask
    };


    const iClickerChannel DA =
    {
        { 0xE2, 0x60, 0x00 }, //send
        { 0xE5, 0xC0, 0x00 }, //recv
        0x1000, //mask
    };


    const iClickerChannel DB =
    {
        { 0xE3, 0x40, 0x00 }, //send
        { 0xE6, 0x20, 0x00 }, //recv
        0x2000, //mask
    };


    const iClickerChannel DC =
    {
        { 0xE3, 0xC0, 0x00 }, //send
        { 0xE6, 0x80, 0x00 }, //recv
        0x4000, //mask
    };


    const iClickerChannel DD =
    {
        { 0xE3, 0x80, 0x00 }, //send
        { 0xE6, 0xC0, 0x00 }, //recv
        0x8000, //mask
    };

    const iClickerChannel channels[] =
        {AA, AB, AC, AD, BA, BB, BC, BD, CA, CB, CC, CD, DA, DB, DC, DD};


}
#endif
