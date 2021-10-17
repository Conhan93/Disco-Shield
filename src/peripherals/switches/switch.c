#include <peripherals\switches\switch.h>
#include <timer.h>
#include "state_queue.h"


#include "avr/interrupt.h"

#define KEY1 PB0
#define KEY2 PB4


static uint8_t key1_triggered = 0;
static uint8_t key2_triggered = 0;

static uint64_t key1_debounce_time;
static uint64_t key2_debounce_time;
static const uint8_t debounce_interval = 50;

static void enable_switch_interrupts() {
    // enable interrupts on set pins on PCI_INT0
    PCICR |= (1 <<  PCIE0);
    // set pins to trigger interrupt when changed
    PCMSK0 |= (1 << KEY1) | (1 << KEY2);
    sei();
}

void init_switch() {
    enable_switch_interrupts();
}

// check state of keys and set flag if triggered
void switch_update() {

    uint64_t now = millis();

    if((now - key1_debounce_time > debounce_interval) && key1_triggered) {
        key1_triggered ^= 1;
        // push STATE_EVENT onto queue
        state_queue_push_back(KEY_1_PRESSED);

        key1_debounce_time = 0;
    }
    if((now - key2_debounce_time > debounce_interval) && key2_triggered) {
        key2_triggered ^= 1;
        // push STATE_EVENT onto queue
        state_queue_push_back(KEY_2_PRESSED);

        key2_debounce_time = 0;
    }
}


// interrupt triggered by pins on PCINT0
ISR(PCINT0_vect) {

    if (PINB & (1 << KEY1)) { // KEY1 change
        key1_triggered = 1;
        key1_debounce_time = millis();
    }
    if (PINB & (1 << KEY2)) { // KEY2 change
        key2_triggered = 1;
        key2_debounce_time = millis();
    }

}