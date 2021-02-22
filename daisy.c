#include "config.h"
#include "daisy.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>

void daisy_send_cmd(enum command_t cmd)
{
	for (int i = COMMAND_LEN - 1; i >= 0; i--) {
		/* avr is little endian, send msb of command first */
		daisy_send_bit((uint8_t)cmd & (1 << i));
	}
}

void daisy_send_bit(uint8_t bit)
{
	if (bit) {
		DAISY_OUT_H;
	} else {
		DAISY_OUT_L;
	}
	_delay_us(250);
	_delay_us(250);
	DAISY_CLK_H;
	_delay_us(250); /* max. possible us_delay = 768us/F_CPU ca. 411us */
	_delay_us(250);
	DAISY_CLK_L;
	/* always return to zero */
	DAISY_OUT_L;
}

void daisy_wait_n_cycles(uint8_t n)
{
	for (int i = 0; i < n; i++) {
		_delay_us(250);
		_delay_us(250);
		DAISY_CLK_H;
		_delay_us(250);
		_delay_us(250);
		DAISY_CLK_L;
	}
}

/* sends content of "daisybuf" over UART with a following newline */
void daisy_uart(void)
{
	uint8_t i;
	for (i = 0; i < DAISY_LEN_BYTE; i++) {
		UART_transmit(daisybuf[i]);
	}
	UART_transmit('\n');
}

void daisy_reset()
{
	/* send startbit */
	daisy_send_bit(1);
	daisy_send_cmd(RESET_CMD);
	daisy_wait_n_cycles(4);
}

void daisy_write()
{
	/* send startbit */
	daisy_send_bit(1);
	daisy_send_cmd(START_RCV_CMD);
	daisy_wait_n_cycles(2);
	/* rx buf gets written to daisychain */
	for (int i = DAISY_LEN_BYTE; i > 0; i--) {
		for (int j = 0; j < 8; j++) {
			daisy_send_bit(rxbuf[i] & (1 << j));
		}
	}
	daisy_wait_n_cycles(2);
}

/* sends update command to daisychain */
void daisy_update(void)
{
	/* send startbit */
	daisy_send_bit(1);
	daisy_send_cmd(UPDATE_CMD);
	daisy_wait_n_cycles(4);
}

/* reads content of daisychain into  daisybuf */
void daisy_read(void)
{
	/* send startbit */
	daisy_send_bit(1);
	daisy_send_cmd(START_SND_CMD);
	/* switch bidirectional port to read from the daisychain */

	SET_DDR_DAISY_IN;
	daisy_wait_n_cycles(3);

	asm volatile("nop");
	for (int i = DAISY_LEN_BYTE - 1; i >= 0; i--) {
		for (int j = 0; j < 8; j++) {
			_delay_us(250);
			_delay_us(250);
			DAISY_CLK_H;
			_delay_us(250);
			DAISY_UPDATE_H;
			if (DAISY_IN_VAL) {
				daisybuf[i] |= (1 << j);
			}
			DAISY_UPDATE_L;
			_delay_us(250);
			DAISY_CLK_L;
		}
	}
	/* because of fsm structure all states take one extra clock cycle */

	SET_DDR_DAISY_OUT;
	daisy_wait_n_cycles(3);
	asm volatile("nop");
}

void daisy_send_daisybuf_uart(void)
{
	for (int i = DAISY_LEN_BYTE - 1; i >= 0; i--) {
		UART_transmit(daisybuf[i]);
		/* clear daisybuf after sending */
		daisybuf[i] = 0;
	}
}
