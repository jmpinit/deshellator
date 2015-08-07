#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "nrf24l01.h"

void rf_init_io() {
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

uint8_t rf_read_reg(uint8_t addr) {
    addr &= 0x1F;
    uint8_t data[2] = { RF_CMD_R_REGISTER | addr, 0 };
    rf_transfer(data, sizeof(data));
    return data[1];
}

void rf_write_reg(uint8_t addr, uint8_t value) {
    addr &= 0x1F;
    uint8_t data[2] = { RF_CMD_W_REGISTER | addr, value };
    rf_transfer(data, sizeof(data));
}

void rf_mode_rx() {
    uint8_t config = rf_read_reg(RF_REG_CONFIG);
    rf_write_reg(RF_REG_CONFIG, config | (1 << PWR_UP) | (1 << PRIM_RX));
}

uint8_t rf_rx() {
    uint8_t data[2] = { RF_CMD_R_RX_PAYLOAD, 0 };
    rf_transfer(data, sizeof(data));
    return data[1];
}

uint8_t rf_available() {
    uint8_t data[2] = { RF_CMD_R_RX_PL_WID, 0 };
    rf_transfer(data, sizeof(data));
    return data[1];
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

void rf_init_rx() {
    rf_init_io();
    _delay_ms(1);
    uint8_t config = rf_read_reg(RF_REG_CONFIG);
    rf_write_reg(RF_REG_CONFIG, config | (1 << PWR_UP));
    _delay_ms(1);
    config = rf_read_reg(RF_REG_CONFIG);
    rf_write_reg(RF_REG_CONFIG, config | (1 << PRIM_RX));
    _delay_ms(1);

    // payload size for pipe 0
    rf_write_reg(RF_REG_RX_PW_P0, 1);
    
    // auto ack on pipe 0
    //rf_write_reg(0x01, 0x01); // EN_AA reg

    // enable pipe 0
    rf_write_reg(0x02, 0x01); // EN_RXADDR reg

    rf_enable();
    _delay_ms(1);
}

void rf_init_tx() {
    rf_init_io();
    _delay_ms(1);
    uint8_t config = rf_read_reg(RF_REG_CONFIG);
    rf_write_reg(RF_REG_CONFIG, config | (1 << PWR_UP));
    _delay_ms(1);
}
