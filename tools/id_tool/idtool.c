// This is just a test of reversing an iclicker id
// compile with gcc idtool.c -o idtool
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>       /* time */

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

void encode(uint8_t *addr, uint8_t *ret);
void decode(uint8_t *addr, uint8_t *ret);
bool valid_id(uint8_t addr[4]);
void generate_id(uint8_t addr[4]);
void print_help();


int main(int argc, char **argv)
{
    if (argc != 3 && argc != 2) {
        print_help();
        exit(1);
    }
    srand(time(NULL));


    uint8_t tmp[4];


    if (!strcmp(argv[1], "-o")) {
        uint32_t input;
        sscanf(argv[2], "%x", &input);
        uint8_t *inb = (uint8_t *)&input;
        uint8_t addr[] = { inb[3], inb[2], inb[1], inb[0] };

        printf("%sInput: %#x %#x %#x %#x\n", KBLU, addr[0], addr[1], addr[2], addr[3]);

        if (!valid_id(addr))
            printf("%sNOTE: THIS IS NOT A VALID ICLICKER ID!!\n", KRED);
        encode(addr, tmp);
        printf("%sEncoded: %#x %#x %#x %#x\n",KGRN, tmp[0], tmp[1], tmp[2], tmp[3]);
    } else if (!strcmp(argv[1], "-f")) {
        uint32_t input;
        sscanf(argv[2], "%x", &input);
        uint8_t *inb = (uint8_t *)&input;
        uint8_t addr[] = { inb[3], inb[2], inb[1], inb[0] };

        printf("%sInput: %#x %#x %#x %#x\n", KBLU, addr[0], addr[1], addr[2], addr[3]);
        
        decode(addr, tmp);
        printf("%sDecoded: %#x %#x %#x %#x\n", KGRN, tmp[0], tmp[1], tmp[2], tmp[3]);
    } else if(!strcmp(argv[1], "-g")) {
        generate_id(tmp);
        printf("%sRandom ID: %#x %#x %#x %#x\n", KGRN, tmp[0], tmp[1], tmp[2], tmp[3]);
    } else {
        print_help();
    }

    printf("%s", KNRM);
    return 0;
}


void print_help()
{
    printf("-o: iClicker id to encoded id\n"
            "-f: Encoded id to iClicker id\n"
            "-g: Generate random iClicker ID\n"
            "Examples:\n"
            "./idtool -o 0x8F941803\n"
            "./idtool -f 0x12345678\n"
            "./idtool -g\n"
        );

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


bool valid_id(uint8_t addr[4])
{
    return (addr[0]^addr[1]^addr[2]) == addr[3];
}

void generate_id(uint8_t addr[4])
{
    addr[0] = rand()%256;
    addr[1] = rand()%256;
    addr[2] = rand()%256;
    addr[3] = addr[0]^addr[1]^addr[2];
}
