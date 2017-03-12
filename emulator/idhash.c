// This is just a test of reversing an iclicker id
// compile with gcc idhash.c -o idhash
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


void encode(uint8_t *addr, uint8_t *ret);
void decode(uint8_t *addr, uint8_t *ret);
bool valid_addr(uint8_t addr[4]);

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Ex (Original id to transmit hash):\n./idhash -o 0x8F941803\n./idhash -f 0x12345678\n");
        exit(1);
    }

    uint32_t input;
    uint8_t tmp[4];
    sscanf(argv[2], "%x", &input);

    uint8_t *inb = (uint8_t *)&input;
    uint8_t addr[] = { inb[3], inb[2], inb[1], inb[0] };


    printf("Input: %#x %#x %#x %#x\n", addr[0], addr[1], addr[2], addr[3]);

    if (!strcmp(argv[1], "-o")) {
        encode(addr, tmp);
        printf("Encoded: %#x %#x %#x %#x\n", tmp[0], tmp[1], tmp[2], tmp[3]);
    } else if (!strcmp(argv[1], "-f")) {
        decode(addr, tmp);
        printf("Decoded: %#x %#x %#x %#x\n", tmp[0], tmp[1], tmp[2], tmp[3]);
    }

    printf("\n");
    return 0;
}



void encode(uint8_t *addr, uint8_t *ret)
{
    //bits 4-0
    ret[0] = ((addr[0] >> 5) & 0x4) | ((addr[0]<<3) & 0xf8) | (addr[1] >> 7);
    ret[1] = ((addr[1] << 1) & 0xfc) | ((addr[0] >> 6) & 0x01);
    ret[2] = (addr[1] << 7) | ((addr[0] >> 5) & 0x01) | ((addr[2] >> 1) & 0x7c);
    ret[3] = ((addr[2] & 0x7) << 5) | ((addr[2] & 0x1) << 4);
}

void decode(uint8_t *addr, uint8_t *ret)
{
    ret[0] = (addr[0] >> 3) | ((addr[2] & 0x1) << 5) | ((addr[1] & 0x1) << 6) | ((addr[0] & 0x4) << 5);
    ret[1] = ((addr[0] & 0x1) << 7) | (addr[1] >> 1) | (addr[2] >> 7);
    ret[2] = ((addr[2] & 0x7c) << 1) | (addr[3] >> 5);
    ret[3] = ret[0]^ret[1]^ret[2];
}


bool valid_addr(uint8_t addr[4])
{
    return (addr[0]^addr[1]^addr[2]) == addr[3];
}
