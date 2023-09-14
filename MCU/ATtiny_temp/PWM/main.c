/*
 * Bridge.c
 *
 
 * Created: 26/09/2019 16:12:28
 * Author : FRA
 */ 

#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <string.h>

#include "Comm.h"
#include "Temp_meas.h"

int main(void)
{
	/*setting clock*/
	CPU_CCP = 0xD8; // CCP: Configuration Change Protection Un-protect protected I/O registers
	CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc; // 16/20 MHz internal oscillator
	CPU_CCP = 0xD8;
	CLKCTRL.MCLKCTRLB = 3;
	/*END setting clock*/
		
	Comm_init();

	Temp_meas_init();

	sei();

    while (1)
    {
		Temp_meas();
		
		while(USART0.CTRLA & (USART_TXCIE_bm | USART_DREIE_bm));
		_delay_ms(5);
		
		cli();
		
		memcpy(data_tx, &Temps, DATA_TX_LENGTH);
		uint16_t crc = MdbCrc(data_tx, DATA_TX_LENGTH);
		data_tx[DATA_TX_LENGTH] = crc;
		data_tx[DATA_TX_LENGTH+1] = crc >> 8;
		data_count_tx = 0;
		
		USART0.CTRLA |= USART_DREIE_bm;
		sei();
    }
}
