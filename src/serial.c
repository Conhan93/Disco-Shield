#include "serial.h"
#include "avr/io.h"


#define BAUD 9600
#define MYUBRR ((16000000/(16UL*BAUD))-1)

void USART_Init()  {
    // Set baud rate
    UBRR0H = (uint8_t)(MYUBRR >> 8);
    UBRR0L = (uint8_t) MYUBRR;
    
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    // Set frame format: 8data, 2stop bit
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
    
}


static void USART_Transmit(uint8_t data) {
    /* Wait for empty transmit buffer */

    while (!(UCSR0A & (1<<UDRE0)))  ;
    /* Put data into buffer, sends the data */
    UDR0 = data;
}
static uint8_t USART_Available(void) {
    return UCSR0A & (1<<RXC0);
}

static uint8_t USART_Receive(void) {
    /* Wait for data to be received */
    //while (!(UCSR0A & (1<<RXC0)))
    while (!USART_Available())
    ;
    /* Get and return received data from buffer */
    return UDR0;
}
uint8_t serial_available(void) {
    return USART_Available();
}

void serial_puts(char* string, char* delim) {
    uint8_t end_line = delim == NULL ? "\0" : (*delim);

    while(*string != end_line)
        USART_Transmit(*string++);
}
void serial_putc(char c) {
    USART_Transmit(c);
}
uint8_t serial_getc() {
    return USART_Receive();
}
uint32_t serial_gets(char* buf, uint32_t b_size) {
    char *temp = buf;
    char input;

    *temp = '\000';
    
    while ((input = serial_getc()) != '\n') {
        *temp = input;
        serial_putc(input);

        if ((temp - buf) < b_size) 
            temp++;
        
        // close string
        *temp = '\000';
    }

    return temp - buf;
}
