/*
 * Temp_meas.c
 *
 * Created: 25.04.2021 09:46:28
 *  Author: MrTea
 */ 

#include "Temp_meas.h"

struct Thermistor_struct
{
	float Divider_supply;
	float R_divider;
	float B;
	float T_0;
	float R25;
	float DIV_Rinf;
}Therm;

float Temps;

void Temp_meas_init(void)
{
	ADC0.CTRLC = ADC_PRESC_DIV8_gc | ADC_REFSEL_VDDREF_gc;
	ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc;
	ADC0.CTRLA = ADC_ENABLE_bm;
	
	Therm.Divider_supply = 0.0f;
	Therm.R_divider = 1000.0f;
	Therm.B = 3550.0f;
	Therm.T_0 = 273.15f;
	Therm.R25 = 493.0f;
	Therm.DIV_Rinf = expf(Therm.B/(Therm.T_0+100.0f))/Therm.R25;
}

void Temp_meas(void)
{			
	while (ADC0.COMMAND);
	uint16_t adcVal = ADC0.RES;
			
	ADC0.MUXPOS  = ADC_MUXPOS_AIN1_gc;
	ADC0.COMMAND = ADC_STCONV_bm;
	
	static volatile float Resistance;
	Resistance = Therm.R_divider * (float)adcVal / (float)(65536 - 64 - adcVal);
	Temps = Therm.B/logf(Resistance * Therm.DIV_Rinf) - Therm.T_0;
}