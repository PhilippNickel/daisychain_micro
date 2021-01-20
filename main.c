#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/io.h>

#include "uart.h"
#include "daisy.h"

uint8_t poti_vtune EEMEM = 127;
uint8_t poti_itune EEMEM = 127;
uint8_t poti_vbiasn EEMEM = 127;
uint8_t poti_vbiasp EEMEM = 127;

void util_init(void)
{
	SET_DDR_DAISY_OUT;
	SET_DDR_DAISY_CLK;
	_delay_us(250);
	DAISY_OUT_L;
	DAISY_CLK_L;
	_delay_us(250);
	daisy_reset();
}

int main(void)
{
	util_init();
	USART_init(11); /* 9600 BAUD @ 1.8 MHz crystal */
	sei();

	while (1) {
		if (rx_flag == 1) {
			decode_command();
		}
	}
}
