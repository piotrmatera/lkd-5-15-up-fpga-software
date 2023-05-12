/*
 * IO.h
 *
 *  Created on: 22 mar 2020
 *      Author: MrTea
 */

#ifndef IO_H_
#define IO_H_

#ifndef __ASM_HEADER__

enum GPIO_state_enum
{
    LOW,
    HIGH,
};

enum GPIO_mux_enum
{
    MUX0,
    MUX1,
    MUX2,
    MUX3,
    MUX4,
    MUX5,
    MUX6,
    MUX7,
    MUX8,
    MUX9,
    MUX11,
    MUX12,
    MUX13,
    MUX14,
    MUX15
};

enum GPIO_cpucla_enum
{
    CPU1_IO,
    CPU1CLA_IO,
    CPU2_IO,
    CPU2CLA_IO
};

enum GPIO_dir_enum
{
    INPUT,
    OUTPUT
};

enum GPIO_options_enum
{
    PUSHPULL = (0 << 0),
    PULLUP = (1 << 0),
    INVERT = (1 << 1),
    OPENDRAIN = (1 << 2),
    SYNC  = (0 << 4),
    QUAL3 = (1 << 4),
    QUAL6 = (2 << 4),
    ASYNC = (3 << 4),
};

struct GPIO_struct
{
    enum GPIO_state_enum defval;
    enum GPIO_mux_enum mux;
    enum GPIO_cpucla_enum cpucla;
    enum GPIO_dir_enum dir;
    Uint16 options;
};

extern const struct GPIO_struct GPIOreg[169];

#define GPIO_Setup(i)                                  \
GPIO_WritePin(i, GPIOreg[i].defval);                      \
GPIO_SetupPinMux(i, GPIOreg[i].cpucla, GPIOreg[i].mux);      \
GPIO_SetupPinOptions(i, GPIOreg[i].dir, GPIOreg[i].options)

#define GPIO_SET(pin) *(&GpioDataRegs.GPASET.all + (pin / 32)*GPY_DATA_OFFSET) = 1UL << (pin % 32)
#define GPIO_CLEAR(pin) *(&GpioDataRegs.GPACLEAR.all + (pin / 32)*GPY_DATA_OFFSET) = 1UL << (pin % 32)
#define GPIO_TOGGLE(pin) *(&GpioDataRegs.GPATOGGLE.all + (pin / 32)*GPY_DATA_OFFSET) = 1UL << (pin % 32)
#define GPIO_READ(pin) ((*(&GpioDataRegs.GPADAT.all + (pin / 32)*GPY_DATA_OFFSET) >> (pin % 32) & 1UL))
#define GPIO_WRITE(pin, val) *(&GpioDataRegs.GPACLEAR.all + (pin / 32)*GPY_DATA_OFFSET - val) = 1UL << (pin % 32)
#define GPIO_SET2(pin, val) *(&GpioDataRegs.GPASET.all + (pin / 32)*GPY_DATA_OFFSET) = (Uint32)val << (pin % 32)
#define GPIO_CLEAR2(pin, val) *(&GpioDataRegs.GPACLEAR.all + (pin / 32)*GPY_DATA_OFFSET) = (Uint32)val << (pin % 32)
#define GPIO_TOGGLE2(pin, val) *(&GpioDataRegs.GPATOGGLE.all + (pin / 32)*GPY_DATA_OFFSET) = (Uint32)val << (pin % 32)

#define SD_SPISIMO_PIN 24
#define SD_SPISOMI_PIN 25
#define SD_SPICLK_PIN 26
#define SD_SPISTE_PIN 27

#define I2CA_SDA_PIN 42
#define I2CA_SCL_PIN 43

#define EM1D0   85
#define EM1D1   83
#define EM1D2   82
#define EM1D3   81
#define EM1D4   80
#define EM1D5   79
#define EM1D6   78
#define EM1D7   77
#define EM1D8   76
#define EM1D9   75
#define EM1D10  74
#define EM1D11  73
#define EM1D12  72
#define EM1D13  71
#define EM1D14  70
#define EM1D15  69

#define EM1D16  68
#define EM1D17  67
#define EM1D18  66
#define EM1D19  65
#define EM1D20  64
#define EM1D21  63
#define EM1D22  62
#define EM1D23  61
#define EM1D24  60
#define EM1D25  59
#define EM1D26  58
#define EM1D27  57
#define EM1D28  56
#define EM1D29  55
#define EM1D30  54
#define EM1D31  53

#define EM1CS0  32
#define EM1WE   31
#define EM1OE   37

#define EM1A0   38
#define EM1A1   39
#define EM1A2   40
#define EM1A3   41
#define EM1A4   44
#define EM1A5   45
#define EM1A6   46
#define EM1A7   47
#define EM1A8   48
#define EM1A9   49
#define EM1A10  50
#define EM1A11  51

#define FPGA_CS 92
#define FPGA_PROGRAMN 90
#define FPGA_INITN 89
#define FPGA_DONE 91

//CPU IO MASTER
#define TRIGGER0_CM  0
#define TRIGGER1_CM  1

#define RST_CM  2
#define SD_NEW_CM  3
#define SYNC_PWM_CM  4
#define ON_OFF_CM  5
#define TZ_EN_CM  6
#define PWM_EN_CM  36
#define FAN_CM  84

#define LED1_CM  7
#define LED2_CM  8
#define LED3_CM  9
#define LED4_CM  10
#define LED5_CM  11

#define SS_DClink_CM  15
#define DClink_DSCH_CM  x

#define C_SS_RLY_L1_CM  16
#define GR_RLY_L1_CM  17
#define C_SS_RLY_L2_CM  18
#define GR_RLY_L2_CM  19
#define C_SS_RLY_L3_CM  20
#define GR_RLY_L3_CM  21
#define C_SS_RLY_N_CM  22
#define GR_RLY_N_CM  23

#define FPGA_SED  133


#define EN_Mod_1_CM  30
#define TX_Mod_1_CM  29
#define RX_Mod_1_CM  28

#define EN_Mod_2_CM  99
#define TX_Mod_2_CM  93
#define RX_Mod_2_CM  94

#define EN_Mod_3_CM  14
#define TX_Mod_3_CM  12
#define RX_Mod_3_CM  13

//ECAP1 Mod1
//ECAP2 Mod2
//ECAP3 Mod3
//ECAP4
//ECAP5
//ECAP6

#endif

#endif /* IO_H_ */
