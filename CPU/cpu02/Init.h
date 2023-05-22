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
    void PWM_timestamp(volatile struct EPWM_REGS *EPwmReg);
    void PWMs();
    void CLA();
    void CIC1_filter(struct CIC1_struct *CIC, float max_value, float OSR, float decimation_ratio);
    void CIC2_filter(struct CIC2_struct *CIC, float max_value, Uint16 OSR, Uint16 decimation_ratio);
    void CIC1_adaptive_filter(struct CIC1_adaptive_struct *CIC, float max_value, float OSR);
    void Variables();
};

extern class Init_class Init;

#endif /* INIT_H_ */
