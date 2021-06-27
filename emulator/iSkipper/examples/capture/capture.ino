#include "iClickerEmulator.h"
#include <RingBufCPP.h>
#include <string.h>

/* UPDATE THESE FOR YOUR PARTICULAR BOARD */
#define IS_RFM69HW true //make true if using w version
#define IRQ_PIN 3 // This is 3 on adafruit feather
#define CSN 8 // This is 8 on adafruit feather
/* END THINGS YOU MUST UPDATE */


#define MAX_BUFFERED_PACKETS 20
#define SEND_ACKS true

iClickerEmulator clicker(CSN, IRQ_PIN, digitalPinToInterrupt(IRQ_PIN), IS_RFM69HW);
RingBufCPP<iClickerPacket, MAX_BUFFERED_PACKETS> recvBuf;

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
  iClickerPacket r;

  Serial.println("Sending welcome message");
  clicker.sendWelcomePacket("_0+2-2=x_?", QuestionModes::MULTIPLE_NUMERIC, 5);
  clicker.setChannel(iClickerChannels::AA);
  clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);

  //see if there is a pending packet, check if its an answer packet
  while (recvBuf.pull(&r) && r.type == PACKET_ANSWER) {
    uint8_t *id = r.packet.answerPacket.id;
    char answer = iClickerEmulator::answerChar(r.packet.answerPacket.answer);
    snprintf(tmp, sizeof(tmp), "Captured: %c (%02X, %02X, %02X, %02X) \n", answer, id[0], id[1], id[2], id[3]);
    Serial.println(tmp);
  }

  delay(100);
}


void recvPacketHandler(iClickerPacket *recvd)
{
    if (SEND_ACKS && recvd->type == PACKET_ANSWER) {
      clicker.acknowledgeAnswer(&recvd->packet.answerPacket, true);
      // restore the frequency back to AA and go back to promiscous mode
      clicker.setChannel(iClickerChannels::AA);
      clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
    }

    recvBuf.add(*recvd);
}
