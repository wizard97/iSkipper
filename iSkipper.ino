#include "iClickerEmulator.h"

#define CSN 10
#define CE 9
#define IRQ 8

iClickerEmulator iSkip(CE, CSN, IRQ);

void setup()
{
  Serial.begin(115200); 
  iSkip.beginRecv();
}


void loop()
{

}
