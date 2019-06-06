#include "iClickerEmulator.h"
#include <string.h>

iClickerEmulator *iClickerEmulator::_self;

iClickerEmulator::iClickerEmulator(uint8_t _cspin, uint8_t _irqpin, uint8_t _irqnum, bool isRFM69HW)
: _radio(_cspin, _irqpin, isRFM69HW, _irqnum)
{
    _recvCallback = NULL;
    _self = this; //this sucks
    _radio.setRecvCallback(&isrRecvCallback);
}

bool iClickerEmulator::begin(iClickerChannel chan)
{
    //seed rand
    randomSeed(analogRead(0) + analogRead(1) + analogRead(2));

    _radio.initialize();
    _radio.setChannel(chan);
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);

    return true;
}



uint8_t iClickerEmulator::computeChecksum(uint8_t *msg, uint16_t len)
{
    uint8_t ret = 0;
    for (uint16_t i=0; i < len; i++) {
      ret += msg[i];
    }

    return ret;
}


iClickerAnswer iClickerEmulator::decodeAns(uint8_t encoded)
{
    encoded &= 0x0f;

    for (uint8_t i=0; i < NUM_ANSWER_CHOICES; i++)
    {
        if (answerOffsets[i] == encoded)
            return (iClickerAnswer)i;
    }

    return ANSWER_A;
}


uint8_t iClickerEmulator::getAnswerOffset(iClickerAnswer ans)
{
    if (ans >= NUM_ANSWER_CHOICES || ans < ANSWER_A)
        return 0;

    return answerOffsets[ans];
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


iClickerAnswer iClickerEmulator::randomAnswer()
{
    const iClickerAnswer ans[] = {
        ANSWER_A,
        ANSWER_B,
        ANSWER_C,
        ANSWER_D,
        ANSWER_E
    };

    return ans[random(sizeof(ans))];
}


char iClickerEmulator::answerChar(iClickerAnswer ans)
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


iClickerAnswer iClickerEmulator::charAnswer(char ans)
{
    switch(ans)
    {
        case 'a': case 'A':
            return ANSWER_A;

        case 'b': case 'B':
            return ANSWER_B;

        case 'c': case 'C':
            return ANSWER_C;

        case 'd': case 'D':
            return ANSWER_D;

        case 'e': case 'E':
            return ANSWER_E;

        case 'p': case 'P':
            return ANSWER_PING;

    }
    return ANSWER_A;
}


bool iClickerEmulator::submitAnswer(uint8_t id[ICLICKER_ID_LEN], iClickerAnswer ans, bool withAck, uint32_t timeout, bool waitClear )
{
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);

    uint8_t toSend[PAYLOAD_LENGTH_SEND];
    // Encode the ID
    encodeId(id, toSend);
    // zero out last nibble
    toSend[3] &= 0xF0;
    // add dumb redundant answer nibble
    toSend[3] |= 0x0F & getAnswerOffset(ans);
    // Compute checksum
    toSend[4] = computeChecksum(toSend, PAYLOAD_LENGTH_SEND-1);

    _radio.send(toSend, PAYLOAD_LENGTH_SEND, waitClear);

    // need to determine packet format!
    if (withAck)
    {
        uint32_t start = millis();
        configureRadio(CHANNEL_RECV, toSend);

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

void iClickerEmulator::acknowledgeAnswer(iClickerAnswerPacket* packet, bool accept) {
    // ACKs are either 4 (accept) or 5 (reject) bytes
    uint8_t ack_payload[5];
    encodeId(packet->id, ack_payload);

    configureRadio(CHANNEL_RECV, ACK_SEND_SYNC_ADDR);

    if (accept) {
        // When accepting, we simply replay the encoded payload
        // ack[0-2] = encodedId[0-2]
        // ack[3] = (encodedId[3] & 0xF0) | answer_nibble
        ack_payload[3] = (ack_payload[3] & 0xF0) | (0x0F & getAnswerOffset(packet->answer));
        // accept packets are only 4 bytes long so we zero out the last byte
        ack_payload[4] = 0;
    } else {
        // ack[0-1] = encodedId[0-1]
        // ack[2] = ~encodedId[2]
        ack_payload[2] = ~ack_payload[2];
        // ack[3] = (encodedId[3] & 0xF0) | 0x6
        ack_payload[3] = (ack_payload[3] & 0xF0) | 0x6;
        // ack[4] = 0x66
        ack_payload[4] = 0x66;
    }

    _radio.send(ack_payload, 5, false);
}

// Returns 0 on failure, otherwise converts the ascii character `c`
// to the equivilent fucky-iClicker-text-encoding
uint8_t asciiToiClickerEncoding(char c) {
    if (c >= 'a' && c <= 'z') {
        return (c - 'a') + 0x8B;
    } else if (c >= 'A' && c <= 'Z') {
        return (c - 'A') + 0x8B;
    } else if (c >= '1' && c <= '9') {
        return (c - '1') + 0x81;
    }
    switch (c) {
        case '0': return 0x8A;
        case '-': return 0xA5;
        case '+': return 0xA6;
        case '=': return 0xA7;
        case '?': return 0xA8;
        case '_': return 0xA9;
        default: return 0;
    }
}

void iClickerEmulator::sendWelcomePacket(char* msg, QuestionMode mode, uint16_t num_questions) {
    uint8_t welcome_payload[WELCOME_PACKET_SIZE];
    memset(welcome_payload, 0, WELCOME_PACKET_SIZE);

    if (msg != NULL) {
        size_t length = strlen(msg);
        // Welcome message can't be longer than 8 chars
        if (length > 8) {
            length = 8;
        }

        for (size_t i = 0; i < length; i++) {
            uint8_t mapped_character = asciiToiClickerEncoding(msg[i]);
            welcome_payload[i] = mapped_character;
        }
    }

    // set the magic bytes which tell what question mode to put iClicker2s in
    welcome_payload[8] = mode.welcome_bytes[0];
    welcome_payload[11] = mode.welcome_bytes[1];
    // num_questions only matters in the MULTIPLE_* modes
    uint8_t* num_question_bytes = (uint8_t*) &num_questions;
    welcome_payload[9] = num_question_bytes[0];
    welcome_payload[10] = num_question_bytes[1];
    // compute the checksum
    uint8_t checksum = 0;
    for (int i = 0; i < WELCOME_PACKET_SIZE - 1; i++) {
        checksum += welcome_payload[i];
    }
    welcome_payload[WELCOME_PACKET_SIZE - 1] = checksum;

    configureRadio(CHANNEL_RECV_WELCOME, WELCOME_RECV_SYNC_ADDR);
    _radio.send(welcome_payload, 16, true);
}


void iClickerEmulator::setChannel(iClickerChannel chan)
{
    _radio.setChannel(chan);
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);
}


iClickerChannel iClickerEmulator::getChannel()
{
    return _radio.getChannel();

}


void iClickerEmulator::configureRadio(iClickerChannelType type, const uint8_t *syncaddr)
{
    int syncAddrLength = 0;
    switch (type) {
        case CHANNEL_SEND:
            syncAddrLength = SEND_SYNC_ADDR_LEN;
            break;
        case CHANNEL_RECV:
            syncAddrLength = RECV_SYNC_ADDR_LEN;
            break;
        case CHANNEL_RECV_WELCOME:
            syncAddrLength = WELCOME_RECV_SYNC_ADDR_LEN;
            break;
    }
    // set the correct sync addr and len
    _radio.setSyncAddr(syncaddr, syncAddrLength);

    // put radio on the correct freq and packet size
    _radio.setChannelType(type);
}



// go into recv mode
void iClickerEmulator::startPromiscuous(iClickerChannelType chanType, void (*cb)(iClickerPacket *))
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

    iClickerPacket recvd;
    //process packet
    if (numBytes == PAYLOAD_LENGTH_SEND && _self->_radio.getChannelType() == CHANNEL_SEND) {
        //recvd from another iclicker
        recvd.type = PACKET_ANSWER;
        // Decode the ID
        decodeId(buf, recvd.packet.answerPacket.id);
        recvd.packet.answerPacket.answer = decodeAns(buf[3] & 0xf);

        // Double check the checksum
        if (computeChecksum(buf, PAYLOAD_LENGTH_SEND - 1) != buf[PAYLOAD_LENGTH_SEND-1]) {
            // invalid
            return;
        }

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
    iClickerAnswer ans;
    // put radio into correct mode
    configureRadio(CHANNEL_SEND, DEFAULT_SEND_SYNC_ADDR);

    for (uint32_t i=0; i < num; i++) {
        randomId(id); //get random id
        ans = randomAnswer();
        //submit answer
        if(!submitAnswer(id, ans, false))
            return false;

        delay(interval);
    }

    return true;
}

void iClickerEmulator::ddos(uint32_t ms)
{
    uint32_t start = millis();
    uint8_t id[5][ICLICKER_ID_LEN];

    for (uint16_t i=0; i < 5; i++)
        randomId(id[i]);

    while (millis() - start < ms)
    {
        for (uint16_t i=0; i < 5; i++)
            submitAnswer(id[i], randomAnswer(), false);

        delay(0);
    }
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

iClickerChannelMask_t iClickerEmulator::scan()
{
    iClickerChannelMask_t ret = 0x0;
    iClickerChannel old = getChannel(); //so we can restore channel

    uint8_t id[ICLICKER_ID_LEN];

    randomId(id); //use random id

    for (uint16_t j=0; j < NUM_ICLICKER_CHANNELS; j++)
    {
        // get the correct channel
        const iClickerChannel c = iClickerChannels::channels[j];
        setChannel(c);
        if (ping(id, 1))
            ret |= c.mask;
    }

    setChannel(old);

    return ret;
}
