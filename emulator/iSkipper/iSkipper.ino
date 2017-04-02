#define IRQ_PIN 10
#define CSN 9

#include "iClickerEmulator.h"
iClickerEmulator clicker(CSN, 10);

void setup()
{
    Serial.begin(115200);
    clicker.begin();
}


void loop()
{

}
