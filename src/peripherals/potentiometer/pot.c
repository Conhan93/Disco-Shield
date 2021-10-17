#include <peripherals\potentiometer\pot.h>

#include "avr/io.h"
#include "state_queue.h"

#define POT_PIN PC0

static uint8_t adc_initiated = 0;
static uint32_t pot_val = 0;
static uint32_t pot_in = 0;

static void init_adc() {
    // AVCC - 5V reference
    ADMUX |= (1 << REFS0);

    // enable ADC and use prescalers
    ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

    adc_initiated = 1;
}

static uint32_t adc_read(uint8_t ch)
{
    if(!adc_initiated) init_adc();

    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits
 
    // start single conversion
    ADCSRA |= (1<<ADSC);
 
    // wait for conversion to complete
    while(ADCSRA & (1<<ADSC));
 
    return (ADC);
}
void pot_update() {
    // read potentiometer
    pot_in = adc_read(POT_PIN);

    // if current and last difference greater than 5
    if(abs(pot_in - pot_val) > 5) {
        // update pot_val and push event onto queue
        pot_val = pot_in;
        state_queue_push_back(POT_MOVED);
    }
}
uint32_t pot_get() {return pot_val; }