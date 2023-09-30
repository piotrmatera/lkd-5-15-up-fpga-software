//FPGA IO MASTER 

`define LED1_FM  `C+16
`define LED2_FM  `C+15 
`define LED3_FM  `D+14 
`define LED4_FM  `C+14
`define LED5_FM  `E+14 
`define ON_OFF_FM  `C+13
 
`define TX1_FM  `D+1 
`define TX2_FM  `C+1

`define RX1_FM  `F+2 
`define RX2_FM  `D+2

`define PWM_L_N_FM  `B+8
`define PWM_H_N_FM  `A+6
`define FLT_L_N_FM  `E+5
`define FLT_H_N_FM  `F+5
`define RDY_L_N_FM  `A+7
`define RDY_H_N_FM  `B+6
 
`define PWM_L_L1_FM  `A+12
`define PWM_H_L1_FM  `A+11
`define FLT_L_L1_FM  `C+8
`define FLT_H_L1_FM  `D+7
`define RDY_L_L1_FM  `B+11
`define RDY_H_L1_FM  `D+9
 
`define PWM_L_L2_FM  `A+17
`define PWM_H_L2_FM  `A+16
`define FLT_L_L2_FM  `C+10
`define FLT_H_L2_FM  `E+8
`define RDY_L_L2_FM  `B+16
`define RDY_H_L2_FM  `B+15
 
`define PWM_L_L3_FM  `A+19
`define PWM_H_L3_FM  `A+18
`define FLT_L_L3_FM  `D+11
`define FLT_H_L3_FM  `E+12
`define RDY_L_L3_FM  `B+18
`define RDY_H_L3_FM  `B+17
 
`define PWM_DCDC_UDC_FM  `D+5
`define PWM_DCDC_TRDS_FM  `F+4 //CLK_Spp_Trds
`define PWM_DCDC_ISO_FM  `E+3 //CLK_Spp_Uac
`define PWM_DCDC_DRV_FM  `C+4 //PWM_DCDC_N

`define PWM_DCDC_DRV_L1_FM  `E+7 //NEW
`define PWM_DCDC_DRV_L2_FM  `E+11 //NEW
`define PWM_DCDC_DRV_L3_FM  `C+11 //NEW 

`define SD_CLK_UDC_FM  `A+9
`define SD_UDC_1_FM  `B+9
`define SD_UDC_05_FM  `A+8

`define SD_CLK_I_FM  `C+2
`define SD_ITR_L1_FM  `A+2
`define SD_ITR_L2_FM  `B+2
`define SD_ITR_L3_FM  `B+1
 
`define SD_U_L1_FM  `A+3
`define SD_U_L2_FM  `B+4
`define SD_U_L3_FM  `A+4
`define SD_CLK_UAC_FM  `B+3 //NEW 

`define SD_I_L1_FM  `B+10
`define SD_I_L2_FM  `B+20
`define SD_I_L3_FM  `B+19
`define SD_I_N_FM  `B+5 //NEW
`define SD_CLK_L1_FM  `A+10
`define SD_CLK_L2_FM  `C+20
`define SD_CLK_L3_FM  `C+18
`define SD_CLK_N_FM  `A+5 //NEW 
 
`define C_SS_RLY_L1_FM  `E+6
`define C_SS_RLY_L2_FM  `D+15
`define C_SS_RLY_L3_FM  `C+12
`define GR_RLY_N_FM  `E+4
`define GR_RLY_L1_FM  `C+6
`define GR_RLY_L2_FM  `C+17
`define GR_RLY_L3_FM  `D+13

`define SS_DClink_FM  `D+6
//`define DClink_DSCH_FM  `C+5

`define FAN_FM  `E+13
`define FAN2_FM  `D+3 //NEW

`define TEMP_N_FM  `C+3 //NEW 
`define TEMP_L1_FM  `D+8 //NEW 
`define TEMP_L2_FM  `E+9 //NEW 
`define TEMP_L3_FM  `D+17 //NEW 

`define IN1_ISO_FM  `F+18
`define IN2_ISO_FM  `F+17
`define IN3_ISO_FM  `E+18
`define OUT1_ISO_FM  `D+16
`define OUT2_ISO_FM  `E+16
`define OUT3_ISO_FM  `D+18 

//F16 -> F3 
//H18 -> D12 
//D12 -> D17

`define IO1_FM  `G+5 
`define IO2_FM  `F+3 
`define IO3_FM  `E+2

`define XTAL_20MHz_FM  `G+3
`define XTAL_25MHz_FM  `G+2