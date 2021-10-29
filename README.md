# Disco-Shield

Arduino Uno with a shield extension attached, containing an RGB diode controlled by switches, rotary encoder and UART terminal.  

## Contents

1. [Scheduling & Task Management](https://github.com/Conhan93/Disco-Shield#scheduling--task-management)
    1. [Scheduler](https://github.com/Conhan93/Disco-Shield#scheduler)
    2. [Task Management & State Machine](https://github.com/Conhan93/Disco-Shield#task-queue--event-machine)
3. [Peripherals](https://github.com/Conhan93/Disco-Shield#peripherals)
    1. [RGB](https://github.com/Conhan93/Disco-Shield#rgb-diode)
    2. [Switches/Buttons](https://github.com/Conhan93/Disco-Shield#switches)
    3. [Encoder](https://github.com/Conhan93/Disco-Shield#rotary-encoder)
4. [Libs](https://github.com/Conhan93/Disco-Shield#libs)
5. [BOM](https://github.com/Conhan93/Disco-Shield#bom)


## Scheduling & Task Management
-----

- ### Scheduler
    Led events created via the shell interface are pushed onto the schedule queue, as an `event` struct.

    ```c
    // constructing event
    Event _event = construct_event(text_to_mode(argv[1]), text_to_colour(argv[2]),
      duration, rate);
    // pushing event onto schedule queue
    if(!schedule_add_event(_event)) {
      shell_fail(4); // error code
      return;
    }
    ```

    If no other event is running they're popped from the queue in the main loop, and used to update the state of the led, and the duration interval which is used to determine when to pop the next event in the queue. If there is one available.

    The schedule queue is just an array with functions for popping and pushing, making it act like a queue. There are also a few functions for CRUD operations from the shell.

- ### Task Queue & "Event Machine"
    
    Task events, known as "`STATE EVENT`"s in the program. Are generated whenever there's an action like a key press, a reading from the potentiometer/encoder or even when an event is popped from the schedule queue. Events are generated by pushing a value onto the state queue(should've called it task queue) which works much like the schedule queue.

    ```c
    void schedule_update() {

        uint64_t now = millis();

        if(!schedule_nr_events())
            return;

        if(now - time_last_pop > interval) {
            // pushing task onto the queue, to be executed/processed later.
            state_queue_push_back(SCHEDULE_POP);
        
            time_last_pop = now;
        }
    }
    ```

    This is done to promote looser coupling between an action or calling of a state change and the execution.

    The task queue is popped and processed at the end of the main loop in an event centric state machine, where instead the machine is event driven. With the task queue feeding events to the state machine, Which is executing those tasks.

    ```c
    void state_update() {
        // run mode - like blink or rainbow mode
        modes[mode]();

        // run queued events
        STATE_EVENT cur_event;
        while(state_queue_pop(&cur_event)) {
            events[cur_event](); // runs function associated with event
        }
    }
    ```
## Peripherals 
- ### RGB Diode
    
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
- ### Switches
    
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
- ### Rotary Encoder
      
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
## Libs
---
The small and lightweight shell library used to parse serial, call commands and pass arguments to them is taken from the [Memfault Repo](https://github.com/memfault/interrupt)

The timer implementation was proudly stolen from the Arduino source code.


## BOM
----
- 1 [Rotary Encoder](https://github.com/Conhan93/Disco-Shield/blob/main/Meta/Shield/Shield%20Components/COM-09806_Web.pdf)
- 2 [Switches](https://github.com/Conhan93/Disco-Shield/blob/main/Meta/Shield/Shield%20Components/en-b3f.pdf)
-  [Resistors](https://github.com/Conhan93/Disco-Shield/blob/main/Meta/Shield/Shield%20Components/PYu-RC_Group_51_RoHS_L_11.pdf)
    - 2 200 ohm
    - 1 300 ohm
    - 2 1k ohm
  - [RGB Led with common anode](https://github.com/Conhan93/Disco-Shield/blob/main/Meta/Shield/Shield%20Components/YSL-R596AR3G4B5C-C10.pdf)

## Schematic and Shield
-----
![Shield Image](https://github.com/Conhan93/Disco-Shield/blob/main/Meta/Shield/Labb3Kretskort.png)

A simple [Schematic](https://github.com/Conhan93/Disco-Shield/blob/main/Meta/Shield/Schematic.pdf) for the shield, drawn in Kicad.
