#ifndef I_CLICKER_EMULATOR_H
#define I_CLICKER_EMULATOR_H

#include "iClickerRadio.h"
#include "Arduino.h"

#define ICLICKER_ID_LEN 4

typedef enum iClickerAnswer
{
    ANSWER_A = 0xB2,
    ANSWER_B = 0xB6,
    ANSWER_C = 0xBE,
    ANSWER_D = 0xBF,
    ANSWER_E = 0xBB,
    ANSWER_PING = 0xB3,
} iClickerAnswer_t;


typedef struct iClickerAnswerPacket
{
    uint8_t encoded_id[ICLICKER_ID_LEN];
    uint8_t answer;
} iClickerAnswerPacket_t;

class iClickerEmulator
{
public:
    iClickerEmulator(uint8_t _cspin, uint8_t _irqpin);
    bool begin();

    //encodes iclicker id for transmission
    static void id_encode(uint8_t *id, uint8_t *ret);
    // decodes a transmitted id
    static void id_decode(uint8_t *id, uint8_t *ret);
    //checks to make sure valid iclicker id
    static bool valid_id(uint8_t *id);

    bool submitAnswer(uint8_t encoded_id[ICLICKER_ID_LEN], iClickerAnswer_t ans, bool withAck, uin32_t timeout);

private:
    iClickerRadio _radio;
};

#endif
