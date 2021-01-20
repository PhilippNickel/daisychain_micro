.PHONY:
all: prog.hex

prog.elf: *.c *.h
	avr-gcc -mmcu=atmega48pb -Wall -Wextra -O2 *.c -o prog.elf

prog.hex: prog.elf
	avr-objcopy -j .text -j .data -O ihex prog.elf prog.hex

.PHONY:
debug: *.c *.h
	avr-gcc -mmcu=atmega48pb -fverbose-asm -save-temps -Wall -Wextra -O2 *.c -o prog.elf

.PHONY:
flash:
	avrdude -v -c usbasp -p m48pb -P usb -U flash:w:prog.hex

.PHONY:
clean:
	rm -f *.elf *.hex *.s *.i *.o 
