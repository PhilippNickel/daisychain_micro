#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU 1843200
#define FOSC 1843200 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#endif // CONFIG_H
