#ifndef I_CLICKER_EMULATOR_H
#define I_CLICKER_EMULATOR_H

#include "RF24.h"
#include "Arduino.h"


#define MAC_SIZE 3
#define DATA_SIZE 1
#define PACKET_SIZE (MAC_SIZE + DATA_SIZE)

#define CHANNEL 47


typedef struct iClickerPacket
{
    uint8_t mac[MAC_SIZE];  //iClicker mac address
    uint8_t data[DATA_SIZE];  //iClicker payload
} iClickerPacket_t;


typedef enum iClickerAnswer
{
    CHOICE_A = '1',
    CHOICE_B = '2',
    CHOICE_C = '3',
    CHOICE_D = '4',
    CHOICE_E = '5',
    CHOICE_F = '6',
    CHOICE_G = '7',
    CHOICE_H = '8',
    CHOICE_I = '9',
    CHOICE_J = '0',

} iClickerAnswer_t;


class iClickerEmulator
{
public:
    iClickerEmulator(uint16_t _cepin, uint16_t _cspin, uint16_t _irqpin);

    bool begin();

    bool send(iClickerAnswer_t c);

    static const uint8_t clickerMasterMAC[MAC_SIZE];
    const uint8_t clickerMyMAC[MAC_SIZE] = { 0xab, 0xcd, 0xef }; //eventually should add a setter for this

protected:
    // Rx related stuff
    void startListening();
    void stopListening();
    bool isListening();
    bool beginRecv();

    // tx related stuff
    bool beginTrans();

private:
    RF24 _radio;
    const uint16_t _irqpin;
    bool _rxlisten; //Rx is currently listening

};

#endif
