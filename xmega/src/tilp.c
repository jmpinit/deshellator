#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "tilp.h"
#include "uart.h"

unsigned char info[768];
unsigned char packet[4];
unsigned int len = 0;

void tilp_tip_high();
void tilp_tip_low();
void tilp_ring_high();
void tilp_ring_low();

void tilp_init(void);
int tilp_tx(uint8_t data);
int tilp_rx(void);

int tilp_send_packet_raw(unsigned char* data, unsigned int len);

void tilp_init() {
    tilp_ring_high();
    tilp_tip_high();
}

void tilp_press(uint16_t button) {
    unsigned char p[] = { MACHINE_ID, CMD_CMD, 0x00, 0x00 };

    p[2] = button & 0xFF;
    p[3] = button >> 8;

    tilp_send_packet_raw(p, 4);

    uart_tx_str("press\r\n");

    uart_tx_str("r1: ");
    for (unsigned char i = 0; i < 4; i++) {
        uart_tx_hex(tilp_rx());
        uart_tx(',');
    }
    uart_tx_str("\r\n");

    uart_tx_str("r2: ");
    for (unsigned char i = 0; i < 4; i++) {
        uart_tx_hex(tilp_rx());
        uart_tx(',');
    }
    uart_tx_str("\r\n");
}

void tilp_spi(uint8_t v) {
    for (int bit = 0; bit < 8; bit++) {
        tilp_wait_ring_low();

        if (v & (0x80 >> bit))
            tilp_tip_high();
        else
            tilp_tip_low();

        tilp_wait_ring_high();
    }
}

void tilp_receive_packet() {
    for (unsigned char i = 0; i < 4; i++) {
        packet[i] = tilp_rx();
    }

    len = (packet[3] << 8) | packet[2];

    for (unsigned char i = 0; i < len; i++) {
        info[i] = tilp_rx();
    }

    /*if (len > 0) {
        unsigned char CRCL = rx();
        unsigned char CRCH = rx();
    }*/
}

int tilp_send_packet_raw(unsigned char* data, unsigned int len) {
    int res;
    for (unsigned int i = 0; i < len; i++) {
        res = tilp_tx(data[i]);
        if (res < 0) {
            // FIXME
            uart_tx_str("failed after ");
            uart_tx_hex(i);
            uart_tx_str("\r\n");
            return res;
        }
    }

    uart_tx_str("packet raw\r\n");
    return 0;
}

void tilp_send_packet(unsigned char cmd, unsigned int len, unsigned char* data) {
    tilp_tx(MACHINE_ID);
    tilp_tx(cmd);

    //tell calc how much data to expect
    tilp_tx(len & 0x0F);  //LSB
    tilp_tx(len >> 8);    //MSB

    //send data and calc checksum
    unsigned int checksum = 0;
    for (unsigned int i = 0; i < len; i++) {
        checksum += data[i];
        tilp_tx(data[i]);
    }

    //send checksum
    if (len > 0) {
        tilp_tx(checksum & 0x0F);  //LSB
        tilp_tx(checksum >> 8);    //MSB
    }
}

void tilp_ring_high() {
    PORT_TI.OUTSET = 1 << TI_RING;
    PORT_TI.DIRCLR = 1 << TI_RING;
}

void tilp_tip_high() {
    PORT_TI.OUTSET = 1 << TI_TIP;
    PORT_TI.DIRCLR = 1 << TI_TIP;
}

void tilp_ring_low() {
    PORT_TI.OUTCLR = 1 << TI_RING;
    PORT_TI.DIRSET = 1 << TI_RING;
}

void tilp_tip_low() {
    PORT_TI.OUTCLR = 1 << TI_TIP;
    PORT_TI.DIRSET = 1 << TI_TIP;
}

#define TIMEOUT 250000

int tilp_wait_ring_high() {
    long t;
    for (t = 0; t < TIMEOUT && !(PORT_TI.IN & (1 << TI_RING)); t++)
        __asm__("nop\n\t");
    
    if (t == TIMEOUT)
        return -1;
    else
        return 1;
}

int tilp_wait_tip_high() {
    long t;
    for (t = 0; t < TIMEOUT && !(PORT_TI.IN & (1 << TI_TIP)); t++)
        __asm__("nop\n\t");
    
    if (t == TIMEOUT)
        return -1;
    else
        return 1;
}

int tilp_wait_ring_low() {
    long t;
    for (t = 0; t < TIMEOUT && (PORT_TI.IN & (1 << TI_RING)); t++)
        __asm__("nop\n\t");
    
    if (t == TIMEOUT)
        return -1;
    else
        return 1;
}

int tilp_wait_tip_low() {
    long t;
    for (t = 0; t < TIMEOUT && (PORT_TI.IN & (1 << TI_TIP)); t++)
        __asm__("nop\n\t");
    
    if (t == TIMEOUT)
        return -1;
    else
        return 1;
}

int tilp_rx() {
    int res;
    unsigned char val = 0;

    for (int b = 0; b < 8; b++) {
        // wait while both are high
        while ((PORT_TI.IN & (1 << TI_RING)) && (PORT_TI.IN & (1 << TI_TIP)))
            { __asm__("nop\n\t"); }

        val >>= 1;

        if (PORT_TI.IN & (1 << TI_TIP)) {
            //received 1
            val |= 0x80;

            _delay_us(TILP_DELAY);

            tilp_tip_low();
            res = tilp_wait_ring_high();
            if (res < 0) {
                tilp_init();
                return res;
            }
            tilp_tip_high();
        } else {
            //received 0
            _delay_us(TILP_DELAY);

            tilp_ring_low();
            res = tilp_wait_tip_high();
            if (res < 0) {
                tilp_init();
                return res;
            }
            tilp_ring_high();
        }
    }

    tilp_init();

    return val;
}

int tilp_tx(unsigned char msg) {
    int res;

    for (int bit = 0; bit < 8; bit++) {
        if (msg & (1 << bit)) {
            uart_tx('1'); // FIXME

            tilp_ring_low();
            res = tilp_wait_tip_low();
            if (res < 0) {
                tilp_init();
                uart_tx('a'); // FIXME
                return res;
            }
            tilp_ring_high();
            res = tilp_wait_tip_high();
            if (res < 0) {
                tilp_init();
                uart_tx('b'); // FIXME
                return res;
            }
        } else {
            uart_tx('0'); // FIXME

            tilp_tip_low();
            res = tilp_wait_ring_low();
            if (res < 0) {
                tilp_init();
                uart_tx('c'); // FIXME
                return res;
            }
            tilp_tip_high();
            res = tilp_wait_ring_high();
            if (res < 0) {
                tilp_init();
                uart_tx('d'); // FIXME
                return res;
            }
        }
    }

    tilp_init();

    uart_tx_str(" tx\r\n");

    return 0;
}
