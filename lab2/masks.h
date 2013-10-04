// CPE123 - Fall 2013
// Kane York
//
// This file has some macros to make the bit manipulation easier.

#define MASK(pin) (1 << pin)
#define ONMASK(reg,mask) reg |= mask
#define OFFMASK(reg,mask) reg &= ~mask
#define ONPIN(reg,pin) reg |= MASK(pin)
#define OFFPIN(reg,pin) reg &= ~MASK(pin)
