#include "iClickerEmulator.h"
#include <string.h>

const uint8_t iClickerEmulator::clickerMasterMAC[MAC_SIZE] = { 0x56, 0x34, 0x12 };


iClickerEmulator::iClickerEmulator(uint16_t _cepin, uint16_t _cspin, uint16_t _irqpin)
: _radio(_cepin, _cspin), _irqpin(_irqpin)
{
    pinMode(_irqpin, OUTPUT);
    _rxlisten = false;
}



bool iClickerEmulator::begin()
{
    _radio.begin();
    _radio.powerUp(); // Turn on
    _radio.setCRCLength(RF24_CRC_16); //Enable 16 bit crc
    _radio.setAutoAck(false); //disable auto-ack
    _radio.setPayloadSize(PACKET_SIZE); // Set packet size
    _radio.setAddressWidth(3); // Set the mac address width
    _radio.setChannel(CHANNEL); // Set to the correct channel
    _radio.setPALevel(RF24_PA_MAX); // Max out power
    _radio.setDataRate(RF24_1MBPS); // Set data rate 1mbps

    beginTrans(); //Init the transmit functionality
    beginRecv(); // Init the recv functionality

    startListening(); //default to listening

    return true;
}



void iClickerEmulator::startListening()
{
    _radio.startListening(); // Put it back into listen mode
    _rxlisten = true;
}


void iClickerEmulator::stopListening()
{
    _radio.stopListening(); // Put it back into listen mode
    _rxlisten = false;
}


bool iClickerEmulator::isListening()
{
    return _rxlisten;
}


bool iClickerEmulator::beginRecv()
{
    //_radio.maskIRQ(1, 1, 0); //Mask all interrupts except RX_DR. Don't think i need this

    // Spoof iclicker recever mac address
    _radio.openReadingPipe(1, clickerMasterMAC); // Open reading pipe on pipe 1

    // WTF is flush_rx private()?!?! I shouldn't have to do this!
    uint8_t tmp[32];
    while(_radio.available())
        _radio.read(tmp, sizeof(tmp));

    return true;
}


bool iClickerEmulator::beginTrans()
{
    _radio.setRetries(0, 0); //No retries
    _radio.openWritingPipe(clickerMyMAC); // Open reading pipe on pipe ,
    _radio.flush_tx();  // flush tx buffer

    return true;
}


// Send an iclicker packet, send ascii
bool iClickerEmulator::send(char c)
{
    bool rx_restore = isListening();

    uint8_t packet[PACKET_SIZE];
    packet[0] = (uint8_t)c;
    memcpy(&packet[1], clickerMyMAC, MAC_SIZE);

    _radio.stopListening(); //stop listening
    _rxlisten = false;

    _radio.writeFast(packet, sizeof(packet));

    bool ret = _radio.txStandBy();

    if (rx_restore)
        startListening();

    return ret;
}
