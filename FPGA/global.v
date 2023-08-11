`timescale 1ns/1ps

`include "version.v" 
 
`define A  0
`define B  20
`define C  40
`define D  60
`define E  80
`define F  100
`define G  120
`define H  140
`define J  160
`define K  180
`define L  200
`define M  220
`define N  240
`define P  260
`define R  280
`define T  300
`define U  320
`define V  340
`define W  360
`define Y  380

//CPU IO
`define EM1D0   85
`define EM1D1   83
`define EM1D2   82
`define EM1D3   81
`define EM1D4   80
`define EM1D5   79
`define EM1D6   78
`define EM1D7   77
`define EM1D8   76
`define EM1D9   75
`define EM1D10  74
`define EM1D11  73
`define EM1D12  72
`define EM1D13  71
`define EM1D14  70
`define EM1D15  69

`define EM1D16  68
`define EM1D17  67
`define EM1D18  66
`define EM1D19  65
`define EM1D20  64
`define EM1D21  63
`define EM1D22  62
`define EM1D23  61
`define EM1D24  60
`define EM1D25  59
`define EM1D26  58
`define EM1D27  57
`define EM1D28  56
`define EM1D29  55
`define EM1D30  54
`define EM1D31  53

`define EM1WE   31
`define EM1CS0  32
`define EM1RNW  33
`define EM1OE   37

`define EM1A0   38
`define EM1A1   39
`define EM1A2   40
`define EM1A3   41
`define EM1A4   44
`define EM1A5   45
`define EM1A6   46
`define EM1A7   47
`define EM1A8   48
`define EM1A9   49
`define EM1A10  50
`define EM1A11  51
`define EM1A12  52
`define EM1A13  86
`define EM1A14  87
`define EM1A15  88

//Comm defines

`define K_28_0 9'h11C
`define K_28_1 9'h13C //comma
`define K_28_2 9'h15C
`define K_28_3 9'h17C
`define K_28_4 9'h19C
`define K_28_5 9'h1BC //comma
`define K_28_6 9'h1DC
`define K_28_7 9'h1FC //comma, not allowed
`define K_23_7 9'h1F7
`define K_27_7 9'h1FB
`define K_29_7 9'h1FD
`define K_30_7 9'h1FE

`define K_Enum_nodes `K_28_0
`define K_Timestamp_master `K_28_2
`define K_Timestamp_slave `K_28_3
`define K_Start_Hipri_Packet `K_27_7
`define K_End_Hipri_Packet `K_29_7
`define K_Start_Lopri_Packet `K_23_7
`define K_End_Lopri_Packet `K_30_7

`define K_Idle `K_28_5
`define K_Error 9'h1EE

/////////////////////////////////////////////////////////////////////

`ifdef POWER_5
	`define CONV_FREQUENCY 62500
	`define CONTROL_RATE 8
`elsif POWER_10
	`define CONV_FREQUENCY 62500
	`define CONTROL_RATE 8
`elsif POWER_15
	`define CONV_FREQUENCY 40000
	`define CONTROL_RATE 4
`elsif POWER_25
	`define CONV_FREQUENCY 31250
	`define CONTROL_RATE 4
`elsif POWER_50
	`define CONV_FREQUENCY 31250
	`define CONTROL_RATE 4
`endif

`define PWM_CLOCK 125000000
`define SD_CLOCK 20000000
`define CYCLE_PERIOD (`PWM_CLOCK/`CONV_FREQUENCY/2)
`define DEF_OSR (`SD_CLOCK/`CONV_FREQUENCY/2)
`define CONTROL_RATE_WIDTH $clog2(`CONTROL_RATE)

/////////////////////////////////////////////////////////////////////

`define KALMAN_CYCLE_PERIOD 2000
`define KALMAN_TIME (8e-9*`KALMAN_CYCLE_PERIOD)
`define KALMAN_GAIN 0.005

`define HIPRI_MAILBOXES_NUMBER 8
`define HIPRI_MAILBOXES_WIDTH $clog2(`HIPRI_MAILBOXES_NUMBER)
`define LOPRI_MAILBOXES_NUMBER 8
`define LOPRI_MAILBOXES_WIDTH $clog2(`LOPRI_MAILBOXES_NUMBER)

`define LOPRI_MSG_LENGTH 128
`define LOPRI_MSG_WIDTH $clog2(`LOPRI_MSG_LENGTH)
`define HIPRI_MSG_LENGTH 128
`define HIPRI_MSG_WIDTH $clog2(`HIPRI_MSG_LENGTH)

`define POINTER_WIDTH $clog2(`HIPRI_MAILBOXES_NUMBER*`HIPRI_MSG_LENGTH+`LOPRI_MAILBOXES_NUMBER*`LOPRI_MSG_LENGTH)

`define COMM_MEMORY_EMIF_WIDTH (`POINTER_WIDTH-2)