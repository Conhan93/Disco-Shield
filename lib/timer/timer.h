#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <stdint.h>
#include <stdarg.h>



#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= (1 << bit))

uint64_t millis();
uint64_t micros();
void init_timer();

#endif