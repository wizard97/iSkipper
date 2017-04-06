
#ifndef SA_ARM_h
#define SA_ARM_h

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <stdint.h>
#endif


/******************** HELPERS ***********************/
static __inline__ uint8_t __iSeiRetVal(void)
{
    __asm__ __volatile__ ("cpsie i" ::);
    return 1;
}

static __inline__ uint8_t __iCliRetVal(void)
{
    __asm__ __volatile__ ("cpsid i" ::);
    return 1;
}

static __inline__ void __iRestore(const  uint32_t *__s)
{
    uint32_t res = *__s;
    __asm__ __volatile__ ("MSR primask, %0" : : "r" (res) );
}

static __inline__ uint32_t __iGetIReg( void )
{
        uint32_t reg;
        __asm__ __volatile__ ("MRS %0, primask" : "=r" (reg) );
        return reg;
}


#define ATOMIC_RESTORESTATE uint32_t _sa_saved                           \
    __attribute__((__cleanup__(__iRestore))) = __iGetIReg()

/******************** MACRO ***********************/

#define ATOMIC()                                                         \
for ( ATOMIC_RESTORESTATE, _sa_done =  __iCliRetVal();                   \
    _sa_done; _sa_done = 0 )

#endif
