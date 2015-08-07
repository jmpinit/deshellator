#include <stdint.h>
#include <avr/io.h>

#include "nrf24l01.h"

void rf_init() {
    PORT_RF.DIRSET = (1 << RF_SCK) | (1 << RF_MOSI) | (1 << RF_CE) | (1 << RF_CSN);
    PORT_RF.DIRCLR = (1 << RF_MISO) | (1 << RF_IRQ);
    PORT_RF.OUTSET = 1 << RF_CSN;
}

void rf_enable() {
    PORT_RF.OUTSET = 1 << RF_CE;
}

void rf_disable() {
    PORT_RF.OUTCLR = 1 << RF_CE;
}

// data buffer is filled with the reply
void rf_transfer(uint8_t* data, int len) {
    // data is little endian
    
    PORT_RF.OUTCLR = 1 << RF_SCK;
    PORT_RF.OUTCLR = 1 << RF_CSN;

    for (int byteIndex = 0; byteIndex < len; byteIndex++) {
        uint8_t inbyte = 0;

        for (int bit = 0; bit < 8; bit++) {
            // data out
            if (data[byteIndex] & (0x80 >> bit)) // MSBit first
                PORT_RF.OUTSET = 1 << RF_MOSI;
            else
                PORT_RF.OUTCLR = 1 << RF_MOSI;

            // clock
            PORT_RF.OUTSET = 1 << RF_SCK;

            // data in
            if (PORT_RF.IN & (1 << RF_MISO))
                inbyte |= 0x80 >> bit;

            // prep
            PORT_RF.OUTCLR = 1 << RF_SCK;
        }

        data[byteIndex] = inbyte;
    }

    PORT_RF.OUTSET = 1 << RF_CSN;
}
