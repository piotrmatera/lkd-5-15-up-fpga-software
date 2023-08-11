 //CPU IO MASTER 
`define FAN2_CM  0 
`define RST_CM  1 
`define SD_NEW_CM  2
`define SD_AVG_CM  3 
`define SYNC_PWM_CM  4
`define FAN_CM  5 
`define TZ_EN_CPU1_CM  6
`define PWM_EN_CM  7
`define TZ_EN_CPU2_CM  8 
 
`define LED1_CM  9 
`define LED2_CM  10 
`define LED3_CM  11 
`define LED4_CM  12 
`define LED5_CM  13 

`define C_SS_RLY_L1_CM  14
`define GR_RLY_L1_CM  15 

`define FPGA_FLASH_SPI_IO0 16
`define FPGA_FLASH_SPI_IO1 17
`define FPGA_FLASH_SPI_SCK 18
`define FPGA_FLASH_SPI_CS  19

`define C_SS_RLY_L2_CM  20
`define GR_RLY_L2_CM  21
`define C_SS_RLY_L3_CM  22
`define GR_RLY_L3_CM  23 
`define C_SS_RLY_N_CM  32
`define GR_RLY_N_CM  33 

`define SS_DClink_CM  36
`define DClink_DSCH_CM  x 

`define ON_OFF_CM  84 

`define FPGA_SED  133
 
//FPGA IO MASTER 

`ifdef POWER_5
	`define TYPE_5_20
`endif
`ifdef POWER_10
	`define TYPE_5_20
`endif
`ifdef POWER_15
	`define TYPE_5_20
`endif
`ifdef POWER_25
	`define TYPE_25_50
`endif
`ifdef POWER_50
	`define TYPE_25_50
`endif

`ifdef TYPE_5_20

	`define LED1_FM  `E+18 
	`define LED2_FM  `F+17 
	`define LED3_FM  `E+17 
	`define LED4_FM  `D+18 
	`define LED5_FM  `E+16 
	`define ON_OFF_FM  `F+18 
	 
	`define EN_Mod_FM  `C+1 
	`define TX_Mod_FM  `C+2 
	`define RX_Mod_FM  `D+2 
	  
	`define TX1_FM  `C+20 
	`define TX2_FM  `B+19 

	`define RX1_FM  `D+17 
	`define RX2_FM  `C+18

	`define PWM_L_N_FM  `B+2
	`define PWM_H_N_FM  `B+3
	`define FLT_L_N_FM  `B+1
	`define FLT_H_N_FM  `A+2
	`define RDY_L_N_FM  `D+8
	`define RDY_H_N_FM  `E+7
	 
	`define PWM_L_L1_FM  `B+4
	`define PWM_H_L1_FM  `B+5
	`define FLT_L_L1_FM  `A+3
	`define FLT_H_L1_FM  `A+4
	`define RDY_L_L1_FM  `E+8
	`define RDY_H_L1_FM  `C+10 
	 
	`define PWM_L_L2_FM  `B+6
	`define PWM_H_L2_FM  `A+7
	`define FLT_L_L2_FM  `A+5
	`define FLT_H_L2_FM  `A+6
	`define RDY_L_L2_FM  `E+9
	`define RDY_H_L2_FM  `E+11 
	 
	`define PWM_L_L3_FM  `A+8
	`define PWM_H_L3_FM  `A+9
	`define FLT_L_L3_FM  `B+8
	`define FLT_H_L3_FM  `B+9
	`define RDY_L_L3_FM  `E+12
	`define RDY_H_L3_FM  `D+11 
	 
	`define PWM_DCDC_UDC_FM  `F+2 
	`define PWM_DCDC_TRDS_FM  `A+17 
	`define PWM_DCDC_ISO_FM  `E+14 
	`define PWM_DCDC_DRV_FM  `C+8 

	`define SD_CLK_UDC_FM  `D+1
	`define SD_UDC_1_FM  `E+2
	`define SD_UDC_05_FM  `E+1

	`define SD_CLK_I_FM  `A+19
	`define SD_ITR_L1_FM  `B+18
	`define SD_ITR_L2_FM  `A+18
	`define SD_ITR_L3_FM  `B+17 
	 
	`define SD_U_L1_FM  `B+12
	`define SD_U_L2_FM  `A+11
	`define SD_U_L3_FM  `B+10
	`define SD_I_L1_FM  `A+13
	`define SD_I_L2_FM  `B+11
	`define SD_I_L3_FM  `A+10
	`define SD_CLK_L1_FM  `B+13
	`define SD_CLK_L2_FM  `A+12
	`define SD_CLK_L3_FM  `D+9 
	 
	`define C_SS_RLY_L1_FM  `A+14
	`define C_SS_RLY_L2_FM  `B+15
	`define C_SS_RLY_L3_FM  `A+16
	`define GR_RLY_N_FM  `E+13
	`define GR_RLY_L1_FM  `C+12
	`define GR_RLY_L2_FM  `D+13
	`define GR_RLY_L3_FM  `C+13

	`define SS_DClink_FM  `C+11
	//`define DClink_DSCH_FM  `D+12

	`define FAN_FM  `B+16

	`define IN1_ISO_FM  `C+14
	`define IN2_ISO_FM  `D+14
	`define IN3_ISO_FM  `C+15
	`define OUT1_ISO_FM  `C+16
	`define OUT2_ISO_FM  `D+16
	`define OUT3_ISO_FM  `B+20 

	//F16 -> F3 
	//H18 -> D12 
	//C17 -> D17 
`endif

`ifdef TYPE_25_50

	`define LED1_FM  `C+16
	`define LED2_FM  `C+15 
	`define LED3_FM  `D+14 
	`define LED4_FM  `C+14
	`define LED5_FM  `E+14 
	`define ON_OFF_FM  `C+13
	 
	`define EN_Mod_FM  `A+13
	`define TX_Mod_FM  `B+13
	`define RX_Mod_FM  `A+14
	 
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
`endif