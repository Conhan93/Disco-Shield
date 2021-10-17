#include "timer.h"
#include "avr/io.h"
#include <avr/interrupt.h>


#define CLOCK_CYCLES_PER_MICROSECOND (F_CPU/1000000L)
#define MS_PER_TIMER0_OVF ((64*256)/(CLOCK_CYCLES_PER_MICROSECOND))

#define MILLIS_INC (MS_PER_TIMER0_OVF/1000L)

#define FRACT_INC ((MS_PER_TIMER0_OVF % 1000L) >> 3)
#define FRACT_MAX (1000L >> 3)

volatile uint64_t timer0_ovf_count = 0;
volatile uint64_t timer0_millis = 0;

static uint8_t timer0_fract = 0;


ISR(TIMER0_OVF_vect) {

    // move variables into registers for quicker operations
    uint64_t milli_sec = timer0_millis;
    uint8_t fraction = timer0_fract;

    milli_sec += MILLIS_INC;
    fraction += FRACT_INC;
    if(fraction >= FRACT_MAX) {
        fraction -= FRACT_MAX;
        milli_sec++;
    }

    // restore reg values 
    timer0_fract = fraction;
    timer0_millis = milli_sec;
    timer0_ovf_count++;
}

uint64_t millis() {
    uint64_t m;

    uint8_t SREG_values = SREG;
    cli();

    m = timer0_millis;

    SREG = SREG_values;

    return m;
}
uint64_t micros() {
    uint64_t m;

    uint8_t SREG_values = SREG, t;

    cli();
    m = timer0_ovf_count;

    t = TCNT0;

    if((TIFR0 & (1 << TOV0)) && (t < 255))
        m++;
    
    SREG = SREG_values;

    return ((m << 8) + t) * (64 / CLOCK_CYCLES_PER_MICROSECOND);
}

void init_timer() {
    sei();

    sbi(TCCR0A, WGM01);
    sbi(TCCR0A, WGM00);

    sbi(TCCR0B, CS01);
    sbi(TCCR0B, CS00);

    //enable timer0 interrupt
    sbi(TIMSK0, TOIE0);
}