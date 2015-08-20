#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "tilp.h"
#include "image.h"

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
    uart_init(0); // 0 is 2 megabaud

    tilp_init();

    //execute("hi", hello, sizeof(hello));
    //uart_tx_str("done programming\r\n");

    uart_tx_str("hello!\r\n");

    for (;;) {
        tilp_press(KEY_E);
        _delay_ms(1000);
    }
}
