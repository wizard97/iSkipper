// Source:
// https://github.com/esp8266/Arduino/issues/615

#ifndef SA_ESP8266_h
#define SA_ESP8266_h

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <stdint.h>
#endif

#ifndef __STRINGIFY
    #define __STRINGIFY(a) #a
#endif

#ifndef xt_rsil
    #define xt_rsil(level) (__extension__({uint32_t state; __asm__ __volatile__("rsil %0," __STRINGIFY(level) : "=a" (state)); state;}))
#endif

#ifndef xt_wsr_ps
    #define xt_wsr_ps(state)  __asm__ __volatile__("wsr %0,ps; isync" :: "a" (state) : "memory")
#endif


static __inline__ void __iRestore(const  uint32_t *__s)
{
    xt_wsr_ps(*__s);
}

// Note value can be 0-15, 0 = Enable all interrupts, 15 = no interrupts
#define ATOMIC_RESTORESTATE uint32_t _sa_saved              \
    __attribute__((__cleanup__(__iRestore))) = xt_rsil(15)


/*************** MACRO **********************/
#define ATOMIC()                                            \
for ( ATOMIC_RESTORESTATE, _sa_done =  1;                   \
    _sa_done; _sa_done = 0 )

#endif
