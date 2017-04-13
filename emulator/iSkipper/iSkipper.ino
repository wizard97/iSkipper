#include "iClickerEmulator.h"
#include <RingBufCPP.h>
#include <string.h>

#define IRQ_PIN 6
#define CSN 10
#define MAX_BUFFERED_PACKETS 20

iClickerEmulator clicker(CSN, IRQ_PIN);
RingBufCPP<iClickerPacket_t, MAX_BUFFERED_PACKETS> recvBuf;

void setup()
{
    Serial.begin(115200);
    Serial.println("working");
    clicker.begin(iClickerChannels::AA);
    // enter promiscouse mode on sending channel
    //clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
}


void loop()
{
  char tmp[50];
  iClickerPacket_t r;

  uint8_t id[4];
  iClickerEmulator::randomId(id);
  clicker.submitAnswer( id, ANSWER_A,
            false, 100);

  delay(1000);
/*
  //see if there is a pending packet, check if its an answer packet
  if (recvBuf.pull(&r) && r.type == PACKET_ANSWER) {
    uint8_t *id = r.packet.answerPacket.id;
    char answer = iClickerEmulator::answerChar((iClickerAnswer_t)r.packet.answerPacket.answer);
    snprintf(tmp, sizeof(tmp), "Captured: %c (%02X, %02X, %02X, %02X) \n", answer, id[0], id[1], id[2], id[3]);
    Serial.println(tmp);
  }
  */
}


void recvPacketHandler(iClickerPacket_t *recvd)
{
  //copy into buffer
  recvBuf.add(*recvd);
}

/*
#include "iClickerEmulator.h"
#include <RingBufCPP.h>
#include <string.h>

#define IRQ_PIN 6
#define CSN 10
#define MAX_BUFFERED_PACKETS 20


iClickerEmulator clicker(CSN, IRQ_PIN);
RingBufCPP<iClickerPacket_t, MAX_BUFFERED_PACKETS> recvBuf;

void setup()
{
    Serial.begin(115200);
    Serial.println("working");
    clicker.begin(iClickerChannels::AA);
    // enter promiscouse mode on sending channel
    clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
}


void loop()
{
  char tmp[50];
  iClickerPacket_t r;

  //see if there is a pending packet, check if its an answer packet
  if (recvBuf.pull(&r) && r.type == PACKET_ANSWER) {
    uint8_t *id = r.packet.answerPacket.id;
    char answer = iClickerEmulator::answerChar((iClickerAnswer_t)r.packet.answerPacket.answer);
    snprintf(tmp, sizeof(tmp), "Captured: %c (%02X, %02X, %02X, %02X) \n", answer, id[0], id[1], id[2], id[3]);
    Serial.println(tmp);
  }
}


void recvPacketHandler(iClickerPacket_t *recvd)
{
  //copy into buffer
  recvBuf.add(*recvd);
}
*/
