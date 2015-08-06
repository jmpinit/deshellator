#include <avr/io.h>
#include <util/delay.h>
#include <avr/xmega.h>

#include "uart.h"
#include "tilp.h"

#define BLINK_DELAY_MS 1000

#define KEY_E 0xA6

int main(void) {
    PORTC.DIRSET = 1 << 2;

    uart_init(9600);
    tilp_init();

    uart_tx_str("hello world!\r\n");

    for (;;) {
        PORTC.OUTSET = 1 << 2; // set the output high.
        _delay_ms(BLINK_DELAY_MS);

        PORTC.OUTCLR = 1 << 2; // set the output low.
        _delay_ms(BLINK_DELAY_MS);

        tilp_press(KEY_E);

        uart_tx_str("done tick\r\n");
    }
}
