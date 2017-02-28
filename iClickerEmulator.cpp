#include "iClickerEmulator.h"

iClickerEmulator::iClickerEmulator(uint16_t _cepin, uint16_t _cspin, uint16_t _irqpin)
: _radio(_cepin, _cspin), _irqpin(_irqpin)
{
    pinMode(_irqpin, OUTPUT);
}

bool iClickerEmulator::beginRecv()
{
    _radio.begin();
    _radio.powerUp(); // Turn on
    _radio.setCRCLength(RF24_CRC_16); //Enable 16 bit crc
    _radio.maskIRQ(1, 1, 0); //Mask all interrupts except RX_DR
    _radio.setPayloadSize(PACKET_SIZE); // Set packet size
    _radio.setAddressWidth(3); // Set the mac address width
    _radio.setChannel(CHANNEL); // Set to the correct channel
    _radio.setPALevel(RF24_PA_MAX); // Max out power
    _radio.setDataRate(RF24_1MBPS); // Set data rate 1mbps

    _radio.openReadingPipe(1, clickerMasterMAC); // Open reading pipe on pipe 1
    _radio.startListening(); // Start listening

    // WTF is flush_rx private?!?!
    uint8_t tmp[32];
    while(_radio.available())
        _radio.read(tmp, sizeof(tmp));


    return true;

}
