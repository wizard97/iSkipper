/*
    SimplyAtomic: Finally a portable way to impliment atomic blocks
*/

#ifndef SIMPLY_ATOMIC_h
#define SIMPLY_ATOMIC_h

#if defined(__AVR__)
    #include "avr.h"

#elif defined(ARDUINO_ARCH_ESP8266)
    #include "esp8266.h"

#elif defined(__arm__)
    #include "arm.h"

#endif



#endif
