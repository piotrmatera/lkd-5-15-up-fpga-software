//`define DOUBLE_PULSE
//`define CALIBRATION

`ifdef TYPE_5_20
	////////////////////////////////////////////////////SELECT HERE
		
	//`define POWER_5
	//`define POWER_10
	`define POWER_15
	
	//`define VERSION_3_1
	`define VERSION_3_2
	
	////////////////////////////////////////////////////
	
	`define EXT_MILLER_BIT 1'b0
	`define TYPE_BIT 4'b0
	
	`ifdef VERSION_3_1
	`include "IO_5_20_v3_1.v"
	`define VERSION 16'd0
	`endif
	`ifdef VERSION_3_2
	`include "IO_5_20_v3_2.v"
	`define VERSION 16'd1
	`endif
	
	
	`ifdef POWER_5
		`define POWER 12'd5
		`define CONV_FREQUENCY 62500
		`define CONTROL_RATE 8
	`elsif POWER_10
		`define POWER 12'd10
		`define CONV_FREQUENCY 62500
		`define CONTROL_RATE 8
	`elsif POWER_15
		`define POWER 12'd15
		`define CONV_FREQUENCY 40000
		`define CONTROL_RATE 4
	`endif
`endif

`ifdef TYPE_25_50
	////////////////////////////////////////////////////SELECT HERE
	
	`define POWER_25
	//`define POWER_50
	
	`define VERSION_4_0

	////////////////////////////////////////////////////
	
	`define TYPE_BIT 4'b1
	`define EXT_MILLER_BIT 1'b1
	
	`ifdef VERSION_4_0
	`include "IO_25_50_v4_0.v"
	`define VERSION 16'd0
	`endif
	
	
	`ifdef POWER_25
		`define POWER 12'd25
		`define CONV_FREQUENCY 31250
		`define CONTROL_RATE 4
	`elsif POWER_50
		`define POWER 12'd50
		`define CONV_FREQUENCY 31250
		`define CONTROL_RATE 4
	`endif
`endif

`ifdef DOUBLE_PULSE
	`define DOUBLE_PULSE_BIT 1'b1
`else
	`define DOUBLE_PULSE_BIT 1'b0
`endif
`ifdef CALIBRATION
	`define CALIBRATION_BIT 1'b1
`else
	`define CALIBRATION_BIT 1'b0
`endif

`define ID_NUMBER {`TYPE_BIT, `POWER, `VERSION}
`define CONFIG_BITS {`CALIBRATION_BIT, `DOUBLE_PULSE_BIT, `EXT_MILLER_BIT}
