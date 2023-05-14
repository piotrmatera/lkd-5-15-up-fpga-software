/*
 * chip1ed389mu12m.h
 *
 *  Created on: 24 lut 2022
 *      Author: Piotr Romaniuk
 */

#ifndef SOFTWARE_DRIVER_MOSFET_CHIP1ED389MU12M_H_
#define SOFTWARE_DRIVER_MOSFET_CHIP1ED389MU12M_H_


#define CHIP1ED389_ADDRESS_INIT  (0x1a>>1)
#define CHIP1ED389_ADDRESS_CHIP  (0x1c>>1)
#define CHIP1ED389_ADDRESS_GROUP (0x2c>>1)


#define CHIP1ED389_I2CADD   0x00 // 44 I2C address of gate driver
#define CHIP1ED389_I2CGADD  0x01 // 44 I2C group address of gate driver
#define CHIP1ED389_I2CCFGOK 0x02 // 45 I2C address configuration access lock
// Configuration registers
#define CHIP1ED389_PSUPR    0x03 // 45 Input pin filter times for IN, RDYC, FLT_N, and I2C
#define CHIP1ED389_FCLR     0x04 // 46 FLT_N clear behavior by RDYC or timer
#define CHIP1ED389_RECOVER  0x05 // 47 Input and output configuration recovery modes
#define CHIP1ED389_RECOVER_RESTORE (1<<1)
#define CHIP1ED389_RECOVER_RECOVER (1<<0)

#define CHIP1ED389_UVTLVL   0x06 // 48 UVLO threshold level for VCC2 and VEE2
#define CHIP1ED389_UVSVCC2C 0x07 // 49 VCC2 soft UVLO enable and threshold level
#define CHIP1ED389_UVSVEE2C 0x08 // 50 VEE2 soft UVLO enable and threshold level
#define CHIP1ED389_ADCCFG   0x09 // 51 ADC enable and compare polarity
#define CHIP1ED389_VEXTCFG  0x0A // 53 CLAMP pin voltage compare limit (ADC)
#define CHIP1ED389_OTWCFG   0x0B // 53 Over-temperature warning level and action
#define CHIP1ED389_D1LVL    0x0C // 54 DESAT disable and DESAT1 threshold voltage level
#define CHIP1ED389_D1FILT   0x0D // 55 DESAT1 filter time and type
#define CHIP1ED389_D2LVL    0x0E // 56 DESAT2 enable during TLTOff, influence on faultoff, and threshold level
#define CHIP1ED389_D2FILT   0x0F // 57 DESAT2 filter time and type
#define CHIP1ED389_D2CNTLIM 0x10 // 59 DESAT2 event counter limit to trigger FLTEVT.D2_EVT
#define CHIP1ED389_D2CNTDEC 0x11 // 60 DESAT2 event count down
#define CHIP1ED389_DLEBT    0x12 // 60 DESAT leading edge blanking time
#define CHIP1ED389_F2ODLY   0x13 // 61 Delay from fault event to gate driver off
#define CHIP1ED389_DTECOR   0x14 // 62 DESAT temperature compensation
#define CHIP1ED389_DRVFOFF  0x15 // 63 Type of fault switch-off
#define CHIP1ED389_DRVCFG   0x16 // 64 Type of normal switch-off and TLTOff gate charge range
#define CHIP1ED389_TLTOC1   0x17 // 65 TLTOff level and ramp A
#define CHIP1ED389_TLTOC2   0x18 // 66 TLTOff duration and ramp B
#define CHIP1ED389_CSSOFCFG 0x19 // 67 Soft-off current
#define CHIP1ED389_CLCFG    0x1A // 67 CLAMP and pin monitoring filter time and type, CLAMP output types and disable
#define CHIP1ED389_SOTOUT   0x1B // 69 Switch-off timeout time and fault signaling
#define CHIP1ED389_CFGOK    0x1C // 70 Register configuration access lock
#define CHIP1ED389_CLEARREG 0x1D // 70 Clear event counter registers for DESAT2, VCC1 VCC2 UVLO, event flags, and soft-reset UVLO,

#define CHIP1ED389_CLEARREG__D2E_CL   (1<<4)
#define CHIP1ED389_CLEARREG__UV2F_CL  (1<<3)
#define CHIP1ED389_CLEARREG__UV1F_CL  (1<<2)
#define CHIP1ED389_CLEARREG__EVTSI_CL (1<<1)
#define CHIP1ED389_CLEARREG__SOFT_RST (1<<0)


//#define CHIP1ED389_res1         0x1E-0x25 //  reserved registers are read as 0H



//Status registers
#define CHIP1ED389_RDYSTAT  0x26 // 72 Status of input side, output side, and gate driver IC
//#define CHIP1ED389_res2 0x27 //  reserved registers are read as 0H
#define CHIP1ED389_SECUVEVT 0x28 // 73 Output side UVLO events causing a not ready state (sticky bits)
#define CHIP1ED389_GFLTEVT  0x29 // 74 Indicator of active fault handling
#define CHIP1ED389_FLTEVT   0x2A // 75 Fault status and events of input side and output side
#define CHIP1ED389_PINSTAT  0x2B // 77 Status of pins
#define CHIP1ED389_COMERRST 0x2C // 78 Status of input to output communication
#define CHIP1ED389_CHIPSTAT 0x2D // 79 Logic status of gate driver IC
#define CHIP1ED389_EVTSTICK 0x2E // 79 Event indicator (sticky bits)
#define CHIP1ED389_UV1FCNT  0x2F // 81 Counter of unfiltered VCC1 UVLO events
#define CHIP1ED389_UV2FCNT  0x30 // 81 Counter of unfiltered VCC2 UVLO events
#define CHIP1ED389_D2ECNT   0x31 // 82 Counter of DESAT2 events
#define CHIP1ED389_ADCMVDIF 0x32 // 82 Filtered ADC calculation result of VCC2-GND2
#define CHIP1ED389_ADCMGND2 0x33 // 83 Filtered ADC result of GND2-VEE2
#define CHIP1ED389_ADCMVCC2 0x34 // 83 Filtered ADC result of VCC2-VEE2
#define CHIP1ED389_ADCMTEMP 0x35 // 84 Filtered ADC result of gate driver temperature
#define CHIP1ED389_ADCMVEXT 0x36 // 84 Filtered ADC result of CLAMP-VEE2

#define CHIP1ED389_CONFIG_REGS_COUNT (CHIP1ED389_CFGOK+1-CHIP1ED389_I2CADD)
#define CHIP1ED389_STATUS_REGS_COUNT (CHIP1ED389_ADCMVEXT+1-CHIP1ED389_RDYSTAT)
#define CHIP1ED389_LASTREG  0x36

//Uwaga! i2c nie umozliwia odczytania tak duzego bloku (max 14)
typedef union chip1ed389_status_regs{
    uint16_t raw_data[CHIP1ED389_STATUS_REGS_COUNT];
    struct named_s{
        uint16_t rdystat;
        uint16_t secuvevt;
        uint16_t glftevt;
        uint16_t fltevt;
        uint16_t pinstat;
        uint16_t comerrst;
        uint16_t chipstat;
        uint16_t evtstick;
        uint16_t uv1fcnt;
        uint16_t uv2fcnt;
        uint16_t d2ecnt;
        uint16_t adcmvdif;
        uint16_t adcmgnd2;
        uint16_t adcmvcc2;
        uint16_t adcmtemp;
        uint16_t adcmvext;

    }named;
} chip1ed389_status_regs_t;

#endif /* SOFTWARE_DRIVER_MOSFET_CHIP1ED389MU12M_H_ */
