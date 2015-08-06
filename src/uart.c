#include <stdint.h>
#include <avr/io.h>

#include "uart.h"

void uart_init(unsigned int baudrate) {
    USARTC0_BAUDCTRLB = 0;
    USARTC0_BAUDCTRLA = F_CPU / (16 * (long)baudrate) - 1;

    // disable interrupts
    USARTC0_CTRLA = 0;

    // set format (8 bits, no parity, 1 stop bit)
    USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;

    // enable rx and tx
    USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm;

    PORTC.DIRSET = 1 << 3;
}

void uart_tx(char c) {
    while(!(USARTC0_STATUS & USART_DREIF_bm)); // wait until empty
    USARTC0_DATA = c;
}

void uart_tx_str(const char* str) {
    char c;
    while((c=*str)) {
        uart_tx(c);
        str++;
    }
}

void uart_tx_hex(uint8_t v) {
    const char* digits = "0123456789ABCDEF";
    uart_tx(digits[v >> 4]);
    uart_tx(digits[v & 0xf]);
}
