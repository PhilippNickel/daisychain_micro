#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "daisy.h"

#include "uart.h"

volatile uint8_t rxbuf[BUF_LENGTH];
volatile uint8_t rx_index;
volatile uint8_t rx_flag;
volatile uint8_t txbuf[BUF_LENGTH];
volatile uint8_t tx_index;
volatile uint8_t daisybuf[DAISY_LEN_BYTE];

void USART_init(uint32_t ubrr)
{
	uint8_t i = 0;
	rx_index = 0;
	rx_flag = 0;
	/* empty buffers */
	for (i = 0; i < BUF_LENGTH; i++) {
		rxbuf[i] = 0;
	}
	for (i = 0; i < DAISY_LEN_BYTE; i++) {
		daisybuf[i] = 0;
	}
	tx_index = 0;
	for (i = 0; i < BUF_LENGTH; i++) {
		txbuf[i] = 0;
	}
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0B |= (1 << RXCIE0);
}

inline void UART_transmit(uint8_t data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)))
		;
	/* Put data into buffer, sends the  data */
	UDR0 = data;
}

/* sends a null terminated string over uart */
void UART_send_string(const char *str)
{
	while (*str != '\0') {
		UART_transmit((uint8_t)*str);
		str++;
	}
}

/* interrupt service routine for arrival of new uart data */
ISR(USART_RX_vect)
{
	rxbuf[rx_index] = UDR0;
	if (rxbuf[rx_index] == '\n') {
		rx_flag = 1;
	}
	rx_index++;
}

/* loops back received data over uart */
void loopback(void)
{
	uint8_t i;
	for (i = 1; i < BUF_LENGTH; i++) {
		txbuf[i] = rxbuf[i];
		if (txbuf[i] != '\n') {
			UART_transmit(txbuf[i]);
		} else {
			break;
		}
	}
	UART_transmit('\n');
	tx_index = 0;
	rx_index = 0;
	rx_flag = 0;
}

/* checks if nth bit of ddata is a 1 */
uint8_t check_bit(unsigned char ddata, int n)
{
	return (ddata >> n) & 1;
}

void decode_command(void)
{
	switch (rxbuf[0]) {
	case '1': // Loopback UART RX
		loopback();
		break;
	case '2': // Write Daisy Chain
		daisy_write();
		loopback();
		break;
	case '3': // Update daisy chain
		daisy_update();
		loopback();
		break;
	case '4': // write and update daisy chain
		daisy_write();
		daisy_update();
		loopback();
		break;
	case '5': // read daisy chain
		//daisy_read_content();
		break;
	case '6': // reset daisy chain
		daisy_send_bit(rxbuf[1]);
		break;
	case '7': // reset daisy chain
		daisy_reset();
		break;
	}

	tx_index = 0;
	rx_index = 0;
	rx_flag = 0;
}
