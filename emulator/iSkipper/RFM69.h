// Improved by Aaron Wisner, without all the built in packet packing crap

// **********************************************************************************
// Driver definition for HopeRF RFM69W/RFM69HW/RFM69CW/RFM69HCW, Semtech SX1231/1231H
// **********************************************************************************
// Copyright Felix Rusu 2016, http://www.LowPowerLab.com/contact
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software
// Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public
// License for more details.
//
// Licence can be viewed at
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************
#ifndef RFM69_h
#define RFM69_h
#include <Arduino.h>            // assumes Arduino IDE v1.0 or greater

#define RF69_MAX_DATA_LEN       61 // to take advantage of the built in AES/CRC we want to limit the frame size to the internal FIFO size (66 bytes - 3 bytes overhead - 2 bytes crc)
#define RF69_SPI_CS             SS // SS is the SPI slave select pin, for instance D10 on ATmega328

// INT0 on AVRs should be connected to RFM69's DIO0 (ex on ATmega328 it's D2, on ATmega644/1284 it's D2)
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
  #define RF69_IRQ_PIN          2
  #define RF69_IRQ_NUM          0
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
  #define RF69_IRQ_PIN          2
  #define RF69_IRQ_NUM          2
#elif defined(__AVR_ATmega32U4__)
  #define RF69_IRQ_PIN          3
  #define RF69_IRQ_NUM          0
#elif defined(__arm__)//Use pin 10 or any pin you want
  #define RF69_IRQ_PIN          10
  #define RF69_IRQ_NUM          10
#else
  #define RF69_IRQ_PIN          2
  #define RF69_IRQ_NUM          0
#endif


#define CSMA_LIMIT              -90 // upper RX signal sensitivity threshold in dBm for carrier sense access
#define RF69_MODE_SLEEP         0 // XTAL OFF
#define RF69_MODE_STANDBY       1 // XTAL ON
#define RF69_MODE_SYNTH         2 // PLL ON
#define RF69_MODE_RX            3 // RX MODE
#define RF69_MODE_TX            4 // TX MODE

// available frequency bands
#define RF69_315MHZ            31 // non trivial values to avoid misconfiguration
#define RF69_433MHZ            43
#define RF69_868MHZ            86
#define RF69_915MHZ            91

#define null                  0
#define COURSE_TEMP_COEF    -90 // puts the temperature reading in the ballpark, user can fine tune the returned value
#define RF69_BROADCAST_ADDR 255
#define RF69_CSMA_LIMIT_MS 1000
#define RF69_TX_LIMIT_MS   1000
#define RF69_FSTEP  61.03515625 // == FXOSC / 2^19 = 32MHz / 2^19 (p13 in datasheet)

// TWS: define CTLbyte bits
#define RFM69_CTL_SENDACK   0x80
#define RFM69_CTL_REQACK    0x40

#define PACKET_LEN_VARIABLE 256

class RFM69
{
public:
    // need to make public and static for isr
    static volatile uint8_t DATA[RF69_MAX_DATA_LEN]; // recv/xmit buf, including header & crc bytes
    static volatile uint8_t PAYLOADLEN;
    static volatile int16_t RSSI; // most accurate RSSI during reception (closest to the reception)
    static volatile uint8_t _mode; // should be protected?

    RFM69(uint8_t slaveSelectPin=RF69_SPI_CS, uint8_t interruptPin=RF69_IRQ_PIN, bool isRFM69HW=false, uint8_t interruptNum=RF69_IRQ_NUM);

    virtual bool initialize(uint8_t freqBand);
    bool canSend();
    virtual void send(const void* buffer, uint8_t bufferSize, bool waitClear = true);
    virtual bool receiveDone();
    uint32_t getFrequency();
    void setPayloadLength(uint8_t len, bool variable);
    void setFrequency(uint32_t freqHz);
    void setFrequency(uint8_t freq[3]); //manually set the registers
    void setCS(uint8_t newSPISlaveSelect);
    void setSyncAddr(const uint8_t *addr, uint8_t len);
    void setPacketSize(uint8_t len);
    void standby(); //setMode(RF69_MODE_STANDBY);
    int16_t readRSSI(bool forceTrigger=false);
    virtual void setHighPower(bool onOFF=true); // has to be called after initialize() for RFM69HW
    virtual void setPowerLevel(uint8_t level); // reduce/increase transmit power level
    void sleep();
    uint8_t readTemperature(uint8_t calFactor=0); // get CMOS temperature (8bit)
    void rcCalibration(); // calibrate the internal RC oscillator for use in wide temperature variations - see datasheet section [4.3.5. RC Timer Accuracy]
    void setDataMode(uint8_t mode);
    // allow hacking registers by making these public
    uint8_t readReg(uint8_t addr);
    void writeReg(uint8_t addr, uint8_t val);
    void readAllRegs();

protected:

    static void isr0();
    void virtual interruptHandler();
    virtual void recvCallback(uint8_t *data, uint8_t numBytes) {}; //interrupt callback
    static volatile bool _inISR;
    void encrypt(const char* key);

    static RFM69* selfPointer;
    uint8_t _slaveSelectPin;
    uint8_t _interruptPin;
    uint8_t _interruptNum;
    uint8_t _powerLevel;
    bool _isRFM69HW;
#if defined (SPCR) && defined (SPSR)
    uint8_t _SPCR;
    uint8_t _SPSR;
#endif

    virtual void receiveBegin();
    virtual void setMode(uint8_t mode);
    virtual void setHighPowerRegs(bool onOff);
    virtual void select();
    virtual void unselect();
    inline void maybeInterrupts();
};

#endif
