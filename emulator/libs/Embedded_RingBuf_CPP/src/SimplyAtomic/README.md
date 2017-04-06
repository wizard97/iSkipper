# SimplyAtomic
Finally a portable way to implement atomic blocks of code within Arduino projects.

## Background
Many projects need a way to disable interrupts, perform some atomic operation, then restore the interrupt register to its previous state. 
A common solution is found in the AVRs `util/atomic.h` using these macros:
```c++
ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
{
  // Do some atomic operation inside here
}
```

The problem is this method only works on AVRs. Nowdays the Arduino ecosystem has expanded to many different architectures.
SimplyAtomic creates one easy to use portable macro that accomplishes the same thing as `ATOMIC_BLOCK(ATOMIC_RESTORESTATE)`

## Usage
Simply surround the critical sections of your program with the following macro:

```c++
// The interrupt status register will be saved here
ATOMIC()
{
  // Do some atomic operation inside here
}
// The interrupt status register will be restored here
```

## Supported Platforms
- AVR (Uno, Mega, etc...)
- ARM (Zero, Due, etc...)
- ESP8266
