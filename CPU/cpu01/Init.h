/*
 * Hardware.h
 *
 *  Created on: 17.11.2017
 *      Author: Mr.Tea
 */

#ifndef INIT_H_
#define INIT_H_

class Init_class
{
public:
    void clear_alarms();
    void GPIO();
    void CPUS();
    void IPCBootCPU2_flash();
    void CLA();
    void Variables();
    void EMIF();
    void DMA();
    void PWMs();
    void ADC();
    void Fan_speed();
    void PWM_TZ_timestamp(volatile struct EPWM_REGS *EPwmReg);
    void EPwm_TZclear(volatile struct EPWM_REGS *EPwmReg);
    void CIC1_filter(struct CIC1_struct *CIC, float max_value, Uint16 OSR, Uint16 decimation_ratio);
    void CIC2_filter(struct CIC2_struct *CIC, float max_value, Uint16 OSR, Uint16 decimation_ratio);
    void CIC1_adaptive_filter(struct CIC1_adaptive_struct *CIC, float max_value, Uint16 OSR);
};

extern class Init_class Init;

#endif /* INIT_H_ */
