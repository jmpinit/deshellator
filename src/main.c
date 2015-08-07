#include <avr/io.h>
#include <util/delay.h>
#include <avr/xmega.h>

#include "uart.h"
#include "nrf24l01.h"
#include "tilp.h"
#include "image.h"

#define BLINK_DELAY_MS 1000

#define KEY_E 0xA6

void type_letter(char c) {
    if (c >= '0' && c <= '9') {
        tilp_press(0x8E + c - '0');
    } else if (c >= 'A' && c <= 'Z') {
        tilp_press(0x9A + c - 'A');
    } else if (c >= 'a' && c <= 'z') {
        tilp_press(0x9A + c - 'a');
    }
}

void type(const char* str) {
    char c;
    while((c=*str)) {
        type_letter(c);
        str++;
    }
}

void execute(const char* name, uint8_t* binary, int len) {
    // open new program
    tilp_press(0x2d); // prgm
    tilp_press(0x1); // right
    tilp_press(0x1); // right
    tilp_press(0x5); // enter
    type(name);
    tilp_press(0x5); // enter

    // type AsmPrgm header
    tilp_press(0x3e); // catalogue
    for (int i = 0; i < 8; i++)
        tilp_press(0x4); // down
    tilp_press(0x5);
    tilp_press(0x5);

    // type out the hex
    char* digits = "0123456789ABCDEF";
    int linelen = 0;
    for (int i = 0; i < len; i++) {
        type_letter(digits[binary[i] >> 4]);
        type_letter(digits[binary[i] & 0xf]);

        linelen++;
        if (linelen > 8) {
            tilp_press(0x5); // enter
            linelen = 0;
        }
    }
}

uint8_t hello[] = {
    0x21, 0x9f, 0x9d,
    0xef, 0x0a, 0x45,
    0xef, 0x2e, 0x45,
    0xc9,
    0x48, 0x69, 0x21, 0x00
};

int main(void) {
    PORTC.DIRSET = 1 << 2;

    uart_init(9600);
    rf_init();

    for (;;) {
        uint8_t data[1] = { RF_CMD_NOP };
        rf_transfer(data, 1);
        uint8_t status = data[0];
        uart_tx_hex(status);
        uart_tx_str("\r\n");
        _delay_ms(1000);
    }

    tilp_init();

    uart_tx_str("hello world!\r\n");
    //execute("hi", hello, sizeof(hello));
    //uart_tx_str("done programming\r\n");

    for (;;) {
        for (int i = 0; i < 768; i++) {
            tilp_spi(image_test2[i]);
        }
        tilp_init();

        PORTC.OUTSET = 1 << 2; // set the output high.
        _delay_ms(BLINK_DELAY_MS);

        PORTC.OUTCLR = 1 << 2; // set the output low.
        _delay_ms(BLINK_DELAY_MS);
    }
}
