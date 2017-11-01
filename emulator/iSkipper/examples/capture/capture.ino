#include "iClickerEmulator.h"
#include <RingBufCPP.h>
#include <string.h>

#define IS_RFM69HW false //make true if using w version
#define IRQ_PIN 6
#define CSN 10
#define MAX_BUFFERED_PACKETS 20

iClickerEmulator clicker(CSN, IRQ_PIN, digitalPinToInterrupt(IRQ_PIN), IS_RFM69HW);
RingBufCPP<iClickerPacket_t, MAX_BUFFERED_PACKETS> recvBuf;

void setup()
{
    Serial.begin(115200);
    clicker.begin(iClickerChannels::AA);
    // enter promiscouse mode on sending channel
    clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
    delay(1000);
    Serial.println("Started promiscous");
    //clicker.dumpRegisters();
}


void loop()
{
  char tmp[50];
  iClickerPacket_t r;

  //see if there is a pending packet, check if its an answer packet

  while (recvBuf.pull(&r)) {
    uint8_t *id = r.packet.answerPacket.id;
    char answer = iClickerEmulator::answerChar((iClickerAnswer_t)r.packet.answerPacket.answer);
    snprintf(tmp, sizeof(tmp), "Captured: %c (%02X, %02X, %02X, %02X) \n", answer, id[0], id[1], id[2], id[3]);
    Serial.println(tmp);
  }

  delay(100);

}


void recvPacketHandler(iClickerPacket_t *recvd)
{
    recvBuf.add(*recvd);
}
