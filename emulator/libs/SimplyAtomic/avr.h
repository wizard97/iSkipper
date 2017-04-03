// Source:
// http://www.nongnu.org/avr-libc/user-manual/atomic_8h_source.html

#ifndef SA_AVR_h
#define SA_AVR_h

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <stdint.h>
#endif

#include <avr/io.h>
/******************** HELPERS ***********************/
static __inline__ uint8_t __iSeiRetVal(void)
{
    sei();
    return 1;
}

static __inline__ uint8_t __iCliRetVal(void)
{
    cli();
    return 1;
}
/*
static __inline__ void __iSeiParam(const uint8_t *__s)
{
    sei();
    __asm__ volatile ("" ::: "memory");
    (void)__s;
}

static __inline__ void __iCliParam(const uint8_t *__s)
{
    cli();
    __asm__ volatile ("" ::: "memory");
    (void)__s;
}
*/
static __inline__ void __iRestore(const  uint8_t *__s)
{
    SREG = *__s;
    __asm__ volatile ("" ::: "memory");
}

#define ATOMIC_RESTORESTATE uint8_t _sa_saved                           \
    __attribute__((__cleanup__(__iRestore))) = SREG

/******************** MACRO ***********************/

#define ATOMIC()                                                        \
for ( ATOMIC_RESTORESTATE, _sa_done =  __iCliRetVal();                  \
    _sa_done; _sa_done = 0 )


#endif
