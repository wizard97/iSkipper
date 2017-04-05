#include <SPI.h>
#include "iClickerRadio.h"
#include "RFM69registers.h"
#include <string.h>

iClickerRadio::iClickerRadio(uint8_t slaveSelectPin, uint8_t interruptPin, bool isRFM69HW, uint8_t interruptNum)
: RFM69(slaveSelectPin, interruptPin, isRFM69HW, interruptNum)
{

    // default to AA
    _chan = iClickerChannels::AA;
    _chanType = CHANNEL_SEND; //by default go to send channel
    _recvCallback = NULL;
}

bool iClickerRadio::initialize()
{
    const uint8_t CONFIG[][2] =
    {
        /* 0x01 */ { REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY },  //standby instead of sleep
        /* 0x02 */ { REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00 }, // no shaping
        /* 0x03 */ { REG_BITRATEMSB, RF_BITRATEMSB_152381}, // default: 4.8 KBPS, now 152KBPS
        /* 0x04 */ { REG_BITRATELSB, RF_BITRATELSB_152381},
        /* 0x05 */ { REG_FDEVMSB, RF_FDEVMSB_222833 }, // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
        /* 0x06 */ { REG_FDEVLSB, RF_FDEVLSB_222833 }, //fstep = 61.0hz, now fstep*0xe45 = 2228333

        /* 0x07 */ { REG_FRFMSB, _chan.send[0] }, //default to channel AA
        /* 0x08 */ { REG_FRFMID, _chan.send[1] },
        /* 0x09 */ { REG_FRFLSB, _chan.send[2] },

        //RegAfcCtrl is set to 0x40?, iclicker bug

        /* 0x0D */ {REG_LISTEN1, RF_LISTEN1_RESOL_IDLE_4100 | RF_LISTEN1_RESOL_RX_4100 | RF_LISTEN1_CRITERIA_RSSI | RF_LISTEN1_END_01 },

        ///* 0x10 */ {REG_VERSION, RF_VERSION_VER_21}, read only?

        /* 0x12 */ {REG_PARAMP, RF_PARAMP_10 },
        /* 0x13 */  { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_100 }, // over current protection (default is 95mA)

        // This is what LNA should be to match the iclicker, but I think the default is better
        ///* 0x18 */ {REG_LNA, RF_LNA_ZIN_200 | RF_LNA_CURRENTGAIN | RF_LNA_GAINSELECT_MAX },

        // RXBW defaults are { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5} (RxBw: 10.4KHz)
        /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_0 }, // (BitRate < 2 * RxBw)
        /* 0x1A */ {REG_AFCBW, RF_AFCBW_DCCFREQAFC_010 | RF_AFCBW_MANTAFC_16 | RF_AFCBW_EXPAFC_0 },
        ///* 0x1E */ {REG_AFCFEI, RF_AFCFEI_AFC_DONE }, they write to read reg

        //23 they write to read reg

        //Need to check DIO mapping

        /* 0x25 */ { REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01 }, // DIO0 is the only IRQ we're using
        /* 0x26 */ { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF }, // DIO5 ClkOut disable for power saving
        /* 0x28 */ { REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN }, // writing to this bit ensures that the FIFO & status flags are reset
        /* 0x29 */ { REG_RSSITHRESH, 220 }, // must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
        ///* 0x2D */ { REG_PREAMBLELSB, RF_PREAMBLESIZE_LSB_VALUE } // default 3 preamble bytes 0xAAAAAA
        /* 0x2E */ { REG_SYNCCONFIG, RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_3 | RF_SYNC_TOL_0 },
        /* 0x2F */ { REG_SYNCVALUE1, RF_SYNC_BYTE1_VALUE_IC },
        /* 0x30 */ { REG_SYNCVALUE2, RF_SYNC_BYTE2_VALUE_IC },
        /* 0x31 */ { REG_SYNCVALUE3, RF_SYNC_BYTE3_VALUE_IC },
        /* 0x37 */ { REG_PACKETCONFIG1, RF_PACKET1_FORMAT_FIXED | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_OFF },
        /* 0x38 */ { REG_PAYLOADLENGTH, PAYLOAD_LENGTH_SEND }, // in variable length mode: the max frame size, not used in TX
        ///* 0x39 */ { REG_NODEADRS, nodeID }, // turned off because we're not using address filtering
        /* 0x3C */ { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFOTHRESH | RF_FIFOTHRESH_TXSTART_FIFOTHRESH_IC  }, // TX on FIFO not empty
        /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_1BIT | RF_PACKET2_AUTORXRESTART_OFF | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
        //for BR-19200: /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
        /* 0x6F */ { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0 }, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0

        /* 0x58 */ { REG_TESTLNA, RF_TESTLNA_HIGH_SENSITIVITY},

        {255, 0}
    };

    digitalWrite(_slaveSelectPin, HIGH);
    pinMode(_slaveSelectPin, OUTPUT);
    SPI.begin();
    unsigned long start = millis();
    uint8_t timeout = 50;
    do writeReg(REG_SYNCVALUE1, 0xAA); while (readReg(REG_SYNCVALUE1) != 0xaa && millis()-start < timeout);
    start = millis();
    do writeReg(REG_SYNCVALUE1, 0x55); while (readReg(REG_SYNCVALUE1) != 0x55 && millis()-start < timeout);

    for (uint8_t i = 0; CONFIG[i][0] != 255; i++)
    writeReg(CONFIG[i][0], CONFIG[i][1]);

    // Encryption is persistent between resets and can trip you up during debugging.
    // Disable it during initialization so we always start from a known state.
    encrypt(0);

    setHighPower(_isRFM69HW); // called regardless if it's a RFM69W or RFM69HW
    setMode(RF69_MODE_STANDBY);
    start = millis();
    while (((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00) && millis()-start < timeout); // wait for ModeReady
    if (millis()-start >= timeout)
    return false;
    _inISR = false;
    attachInterrupt(_interruptNum, RFM69::isr0, RISING);

    selfPointer = this;

    _chan = iClickerChannels::AA;

    return true;
}



void iClickerRadio::setChannel(iClickerChannel_t chan)
{
    _chan = chan;
    setChannelType(CHANNEL_SEND);
    setFrequency(_chan.send);
}


void iClickerRadio::setChannelType(iClickerChannelType_t chanType)
{
    // dont change it if allready correct
    if (_chanType != chanType) {
        _chanType = chanType;
        setFrequency(_chanType == CHANNEL_SEND ? _chan.send : _chan.recv);
        setPayloadLength(_chanType == CHANNEL_SEND ? PAYLOAD_LENGTH_SEND : PAYLOAD_LENGTH_RECV, false);
    }
}


iClickerChannelType_t iClickerRadio::getChannelType()
{
    return _chanType;
}

void iClickerRadio::recvCallback(uint8_t *data, uint8_t numBytes)
{
    if(_recvCallback) {
        _recvCallback(data, numBytes);
    }
}


void iClickerRadio::setRecvCallback(void (*recvCallback)(uint8_t *, uint8_t))
{
    _recvCallback = recvCallback;
}
