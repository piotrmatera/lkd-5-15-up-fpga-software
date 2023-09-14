/*
 * Comm.h
 *
 * Created: 25.04.2021 00:16:27
 *  Author: MrTea
 */ 


#ifndef COMM_H_
#define COMM_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#define F_CPU 5000000
#include <util/delay.h>

#define GPIO_WRITE(pin, val) *(&PORTA.OUTCLR + ((pin) / 8)*sizeof(PORTA) - (val)) = 1 << ((pin) % 8)
#define GPIO_DIR(pin, val) *(&PORTA.DIRCLR + ((pin) / 8)*sizeof(PORTA) - (val)) = 1 << ((pin) % 8)
#define GPIO_READ(pin) ((*(&PORTA.IN + ((pin) / 8)*sizeof(PORTA)) >> ((pin) % 8) & 1))

#define DATA_TX_LENGTH 4

extern char data_tx[DATA_TX_LENGTH+2];
extern uint8_t data_count_tx;

void Comm_init(void);
uint16_t MdbCrc (char *nData, uint8_t wLength);

#endif /* COMM_H_ */