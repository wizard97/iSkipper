#include "iClickerEmulator.h"

iClickerEmulator::iClickerEmulator(uint8_t _cspin, uint8_t _irqpin)
: _radio(_cspin, _irqpin)
{

}

bool iClickerEmulator::begin()
{
    _radio.initialize();

    return true;
}



void iClickerEmulator::id_encode(uint8_t *id, uint8_t *ret)
{
    //bits 4-0
    ret[0] = ((id[0] >> 5) & 0x4) | ((id[0]<<3) & 0xf8) | (id[1] >> 7);
    ret[1] = ((id[1] << 1) & 0xfc) | ((id[0] >> 6) & 0x01);
    ret[2] = (id[1] << 7) | ((id[0] >> 5) & 0x01) | ((id[2] >> 1) & 0x7c);
    ret[3] = ((id[2] & 0x7) << 5) | ((id[2] & 0x1) << 4);
}


void iClickerEmulator::id_decode(uint8_t *id, uint8_t *ret)
{
    ret[0] = (id[0] >> 3) | ((id[2] & 0x1) << 5) | ((id[1] & 0x1) << 6) | ((id[0] & 0x4) << 5);
    ret[1] = ((id[0] & 0x1) << 7) | (id[1] >> 1) | (id[2] >> 7);
    ret[2] = ((id[2] & 0x7c) << 1) | (id[3] >> 5);
    ret[3] = ret[0]^ret[1]^ret[2];
}


bool iClickerEmulator::valid_id(uint8_t *id)
{
    return (id[0]^id[1]^id[2]) == id[3];
}


bool iClickerEmulator::submitAnswer(uint8_t id[ICLICKER_ID_LEN], iClickerAnswer_t ans, bool withAck, uin32_t timeout)
{
    setChannelType(CHANNEL_SEND);

    iClickerAnswerPacket_t toSend;

    id_encode(id, toSend.encoded_id); //encode the id for transmission
    toSend.answer = (uint8_t)ans;

    //send packet, we can cast toSend to array since all uint8_t bytes
    send(&toSend, PACKET_LENGTH_SEND);

    // need to determine packet format!
    if (withAck)
    {
        uint32_t start = millis();
        setSyncAddr(toSend.encoded_id, ICLICKER_ID_LEN - 1);
        setChannelType(CHANNEL_RECV);

        bool recvd = false;
        while(millis() - start < timeout & !recvd) {
            recvd = receiveDone();
        }

        PAYLOADLEN = 0;

        setChannelType(CHANNEL_SEND);
        return recvd;
    }

    return true;
}
