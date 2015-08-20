#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "tilp.h"
//#include "image.h"

#define FRAME_WIDTH     96
#define FRAME_HEIGHT    64
#define FRAME_BYTES     (FRAME_WIDTH * FRAME_HEIGHT / 8)

#define KEY_E 0xA6

uint8_t framebuffer[768];

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

volatile int frameIndex = 0;

ISR(USART_RX_vect) {
    cli();
    framebuffer[frameIndex] = UDR0;
    frameIndex++;
    frameIndex %= FRAME_BYTES;
    sei();
}

int main(void) {
    uart_init(3); // 0 is 2 megabaud

    tilp_init();

    //execute("hi", hello, sizeof(hello));
    //uart_tx_str("done programming\r\n");

    uart_tx_str("hello!\r\n");

    for (int i = 0; i < 768; i++)
        framebuffer[i] = 0xAA;

    for (;;) {
        for (int i = 0; i < 768; i++) {
            tilp_spi(framebuffer[i]);
        }
        tilp_init();
    }
}
