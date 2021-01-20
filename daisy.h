#ifndef DAISY_H_
#define DAISY_H_

#include <avr/io.h>

#define COMMAND_LEN 2 /* command length in bits */
#define ACK_PHASE_CLK_CYCLES                                                   \
	1 /* clock cycles for the acknowledgement phase to take */

enum command_t {
	RESET_CMD,
	START_SND_CMD,
	START_RCV_CMD,
	UPDATE_CMD
}; /* commands to send to the daisychain controller */

#define DAISY_LEN_BYTE 1
#define DAISY_LEN_BIT (DAISY_LEN_BYTE * 8)

#define SET_DDR_DAISY_OUT DDRD |= (1 << PORTD4)
#define DAISY_OUT_H PORTD |= (1 << PORTD4)
#define DAISY_OUT_L PORTD &= ~(1 << PORTD4)
#define SET_DDR_DAISY_IN DDRD &= ~(1 << PORTD4)
#define DAISY_IN_VAL ((PIND & (1 << PORTD4)) ? 1 : 0)

#define SET_DDR_DAISY_CLK DDRE |= (1 << PORTE1)
#define DAISY_CLK_H PORTE |= (1 << PORTE1)
#define DAISY_CLK_L PORTE &= ~(1 << PORTE1)

extern volatile uint8_t daisybuf[DAISY_LEN_BYTE];
void daisy_send_bit(uint8_t bit);
void daisy_update();
void write_daisy();
void daisy_reset();
void daisy_read();
void daisy_write();
void daisy_wait_n_cycles(uint8_t n);
void daisy_send_cmd(enum command_t cmd);

#endif // DAISY_H
