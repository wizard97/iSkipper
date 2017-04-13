#ifndef I_CLICKER_EMULATOR_H
#define I_CLICKER_EMULATOR_H

#include "iClickerRadio.h"
#include "Arduino.h"

#define ICLICKER_ID_LEN 4

#define SEND_SYNC_ADDR_LEN 3
#define RECV_SYNC_ADDR_LEN (ICLICKER_ID_LEN - 1)


typedef enum iClickerAnswer
{
    ANSWER_A = 0,
    ANSWER_B,
    ANSWER_C,
    ANSWER_D,
    ANSWER_E,
    ANSWER_PING,

    NUM_ANSWER_CHOICES,
} iClickerAnswer_t;

// The encoded answer choice is the value sum(encoded_id) + answerOffsets[iClickerAnswer_t]
const uint8_t answerOffsets[NUM_ANSWER_CHOICES] = { 0, 4, 12, 13, 9 , 1 };

const uint8_t DEFAULT_SEND_SYNC_ADDR[SEND_SYNC_ADDR_LEN] =
    {RF_SYNC_BYTE1_VALUE_IC, RF_SYNC_BYTE2_VALUE_IC , RF_SYNC_BYTE3_VALUE_IC };


// 5 bytes
typedef struct iClickerAnswerPacket
{
    uint8_t id[ICLICKER_ID_LEN];
    uint8_t answer;
} iClickerAnswerPacket_t;


typedef struct iClickerResponsePacket
{
    uint8_t unknown[7]; //dont know whats in it yet, but 7 bytes
} iClickerResponsePacket_t;


typedef union iClickerPacketUnion
{
    iClickerAnswerPacket_t answerPacket; //if an answer packet
    iClickerResponsePacket_t respPacket; //if base station response
} iClickerPacketUnion_t;


typedef enum iClickerPacketType
{
    PACKET_ANSWER = 0,
    PACKET_RESPONSE,
} iClickerPacketType_t;

typedef struct iClickerPacket
{
    iClickerPacketType_t type;
    iClickerPacketUnion_t packet;

} iClickerPacket_t;

class iClickerEmulator
{
public:
    static void isrRecvCallback(uint8_t *buf, uint8_t numBytes);
    static iClickerEmulator *_self; //Sucks I have to do this, but must be able to be called through isr
    /***** STATIC METHODS *****/
    //encodes iclicker id for transmission
    static void encodeId(uint8_t *id, uint8_t *ret);
    // decodes a transmitted id
    static void decodeId(uint8_t *id, uint8_t *ret);
    //checks to make sure valid iclicker id
    static bool validId(uint8_t *id);
    //generate random iClicker id
    static void randomId(uint8_t *ret);
    //generate random answer
    static iClickerAnswer_t randomAnswer();
    //concert answer to char
    static char answerChar(iClickerAnswer_t ans);

    static iClickerAnswer_t decodeAns(uint8_t id[ICLICKER_ID_LEN], uint8_t encoded);
    static uint8_t encodeAns(uint8_t id[ICLICKER_ID_LEN], iClickerAnswer_t ans);

    /***** NON-STATIC METHODS *****/
    iClickerEmulator(uint8_t _cspin, uint8_t _irqpin, uint8_t _irqnum);
    bool begin(iClickerChannel_t chan);
    bool submitAnswer(uint8_t id[ICLICKER_ID_LEN], iClickerAnswer_t ans,
            bool withAck=false, uint32_t timeout=100);

    void startPromiscuous(iClickerChannelType_t chanType, void (*cb)(iClickerPacket_t *));
    void stopPromiscuous();
    void setChannel(iClickerChannel_t chan);
    void dumpRegisters() { _radio.readAllRegs(); }

    //return number of ping responses (wait= how many ms for wait for a single ping response)
    uint16_t ping(uint8_t id[ICLICKER_ID_LEN], uint16_t tries = 1, uint16_t wait = 50);


    //ATTACKS
    bool floodAttack(uint32_t num, uint32_t interval);


protected:
    iClickerRadio _radio;
    void (*_recvCallback)(iClickerPacket_t *);

    void configureRadio(iClickerChannelType_t type, const uint8_t *syncaddr = DEFAULT_SEND_SYNC_ADDR);
};

#endif
