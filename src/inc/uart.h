#ifndef H_UART
#define H_UART

void uart_init(unsigned int baudrate);
void uart_tx(char);
void uart_tx_str(const char*);
void uart_tx_hex(uint8_t);

#endif
