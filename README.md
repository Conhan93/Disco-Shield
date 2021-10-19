# Disco-Shield

Arduino Uno with a shield extension attached, containing an RGB diode controlled by switches, rotary encoder and UART terminal.  


## Peripherals 
- RGB Diode
    
    RGB Diode with a common anode with each colour connected to GPIOs, because of the common
    anode high output will decrease the voltage across the diode and turn it off, inverting the
    controls.

    PWM is used to simulate DAC, creating a gradient of colours. In the software the inverted mode
    caused by the common anode is negated by setting the pmw output compare registries to reset at
    bottom, so instead of rising until reset the current falls.

    ```c
    static void pmw_init() {

    /*
    * Set Timer Counter Control Register 1A
    *  To fast pwm mode with OCR1A and B(low and high) set at bottom(inverted)
    */
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0) | (1 << WGM10);
    TCCR1B |= (1 << CS10) | (1 << WGM12);
    /*
    * Set Timer Counter Control Register 2A
    *  To fast pwm mode with OCR2A set at bottom(inverted)
    */
    TCCR2A |= (1 << COM2A1) | (1 << COM2A0) | (1 << WGM20) | (1 << WGM21);
    TCCR2B |= (1 << CS20);

    }
    ```
- Switches
    
    The switches are connected to GPIOs set as input and are kept low by pull downs. The switches are normally open and when closed connect to the Arduino 5V pin, giving a HIGH read when closed.

    The state of the switches are read on interrupt, since the pins connected to switches are connected
    to the PCINT interrupt vector which triggers on change, no matter the direction. The state of the switches are read on interrupt and checked to only trigger on rising.

    ```c
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
    ```

    A timer is used to debounce the switches, the timer and state are checked in a function called
    from main, polling the state of the buttons and queueing events.

    ```c
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
    ```
- Rotary Encoder
      
    The encoder is a voltage divider connected to a pin with a 10-bit ADC, mapping 0-5V to 0-1023.
    The potentiometer saves the last read value and compares it with the current, if the difference
    is higher than the set threshold the value is updated and a `POT_MOVED` event is pushed onto the queue, for the value to read later.

    ```c
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
    ```

## BOM
----