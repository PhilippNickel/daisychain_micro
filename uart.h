#ifndef UART_H_
#define UART_H_

#define Uart_EnableTransmitIT() UCSR0B |= (1 << UDRIE0)
#define Uart_DisableTransmitIT() UCSR0B &= ~(1 << UDRIE0)
#define BUF_LENGTH 8

void USART_init(uint32_t ubrr);
void UART_transmit(uint8_t data);
void UART_send_string(const char *str);
void decode_command();
void loopback();

uint8_t check_bit(unsigned char ddata, int n);

extern volatile uint8_t rxbuf[BUF_LENGTH];
extern volatile uint8_t rx_index;
extern volatile uint8_t rx_flag;
extern volatile uint8_t txbuf[BUF_LENGTH];
extern volatile uint8_t tx_index;

#endif /* UART_H_ */
