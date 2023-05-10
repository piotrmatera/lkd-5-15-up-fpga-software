`include "global.v" 

 //CPU IO MASTER
`define RST_CM  2
`define SD_NEW_CM  3
`define SYNC_PWM_CM  4
`define ON_OFF_CM  5

`define TRIGGER0_CM  0
`define TRIGGER1_CM  1

`define LED1_CM  6
`define LED2_CM  7
`define LED3_CM  8
`define LED4_CM  9
`define LED5_CM  10

`define TX_Mod_CM  12
`define RX_Mod_CM  13
`define EN_Mod_CM  14

`define SS_DClink_CM  11
`define DClink_DSCH_CM  12

`define C_SS_RLY_L1_CM  15
`define GR_RLY_L1_CM  16
`define C_SS_RLY_L2_CM  17
`define GR_RLY_L2_CM  18
`define C_SS_RLY_L3_CM  19
`define GR_RLY_L3_CM  20
`define C_SS_RLY_N_CM  21
`define GR_RLY_N_CM  22

`define TZ_EN_CM  23
`define PWM_EN_CM  84
`define FAN_CM  133

//FPGA IO MASTER
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

`define FAN_AC_FM  `B+16

`define IN1_ISO_FM  `C+14
`define IN2_ISO_FM  `D+14
`define IN3_ISO_FM  `C+15
`define OUT1_ISO_FM  `C+16
`define OUT2_ISO_FM  `D+16
`define OUT3_ISO_FM  `B+20

//F16 -> F3
//H18 -> D12
//C17 -> D17

module SerDes_master(CPU_io, FPGA_io);
	inout[168:0] CPU_io/*synthesis IO_TYPE="LVCMOS33" */; 
	inout[400:1] FPGA_io/*synthesis IO_TYPE="LVCMOS33" */;
 
	integer i; 

	BB BB_XTAL_25MHz(.I(1'b0), .T(1'b1), .O(XTAL_25MHz_i), .B(FPGA_io[`G+2]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;
	BB BB_XTAL_20MHz(.I(1'b0), .T(1'b1), .O(XTAL_20MHz_i), .B(FPGA_io[`G+3]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;
	BB BB_CLK_CPU(.I(XTAL_20MHz_i), .T(1'b0), .O(), .B(FPGA_io[`K+20]))/*synthesis IO_TYPE="LVCMOS33"*/; 
	
	GSR GSR_INST (.GSR (1'b1)); 
	//PUR PUR_INST (.PUR (1'b1)); 
///////////////////////////////////////////////////////////////////// 
	localparam EMIF_MEMORY_WIDTH = `COMM_MEMORY_EMIF_WIDTH+3;//$clog2(2x COMM_MEMORY + MUX) = $clog2(3) = 2 
	wire EMIF_oe_i; 
	wire EMIF_we_i; 
	wire EMIF_cs_i; 
	wire[31:0] EMIF_data_i; 
	reg[31:0] EMIF_data_o; 
	wire[EMIF_MEMORY_WIDTH-1:0] EMIF_address_i; 
 
	localparam EMIF_MUX_NUMBER = 26;
	localparam EMIF_REG_NUMBER = 9;
	localparam EMIF_MUX_WIDTH = $clog2(EMIF_MUX_NUMBER);
	localparam EMIF_REG_WIDTH = $clog2(EMIF_REG_NUMBER);
	wire[31:0] EMIF_TX_mux[EMIF_MUX_NUMBER-1:0];
	wire[31:0] EMIF_RX_reg[EMIF_REG_NUMBER-1:0]; 

///////////////////////////////////////////////////////////////////// 
 
	wire[1:0] rx_i; 
	wire[1:0] tx_o; 
 
	wire tx_clk_5x; 
	wire rx1_clk_10x; 
	wire rx2_clk_10x; 
	wire rx1_clk_phasestep_lag; 
	wire rx2_clk_phasestep_lag; 
	wire clk_1x; 
	wire clk_5x; 
	assign clk_1x = XTAL_25MHz_i; 
 
	Clocking_block Clocking_block(.XTAL_i(clk_1x), .rx1_phase_i(rx1_clk_phasestep_lag), .rx2_phase_i(rx2_clk_phasestep_lag), .LOCK_o(LOCK), 
	.local_5x_clk_o(clk_5x), .tx_clk_5x_o(tx_clk_5x), .rx1_clk_10x_o(rx1_clk_10x), .rx2_clk_10x_o(rx2_clk_10x)); 
 
 	wire timestamp_code_rx1;
	wire[8:0] fifo_rx1_port; 
	wire[1:0] fifo_rx1_port_we; 
	wire fifo_rx1_clk_5x; 
	wire rx1_port_rdy; 
	RX_port RX1_port(.rx_clk_i(rx1_clk_10x), .rx_i(rx_i[0]), .rx_clk_phasestep_lag_o(rx1_clk_phasestep_lag), .timestamp_code_o(timestamp_code_rx1), .rx_rdy_o(rx1_port_rdy),
	.fifo_clk_o(fifo_rx1_clk_5x), .fifo_we_o(fifo_rx1_port_we), .fifo_o(fifo_rx1_port),
	.clk_i(clk_1x), .rst_i(!LOCK)); 

	wire timestamp_code_rx2;
	wire[8:0] fifo_rx2_port; 
	wire[1:0] fifo_rx2_port_we; 
	wire fifo_rx2_clk_5x; 
	wire rx2_port_rdy; 
	RX_port RX2_port(.rx_clk_i(rx2_clk_10x), .rx_i(rx_i[1]), .rx_clk_phasestep_lag_o(rx2_clk_phasestep_lag), .timestamp_code_o(timestamp_code_rx2), .rx_rdy_o(rx2_port_rdy),
	.fifo_clk_o(fifo_rx2_clk_5x), .fifo_we_o(fifo_rx2_port_we), .fifo_o(fifo_rx2_port),
	.clk_i(clk_1x), .rst_i(!LOCK)); 

	wire timestamp_code_tx1;
	wire[8:0] fifo_tx1_core; 
	wire[1:0] fifo_tx1_core_we; 
	wire fifo_tx1_clk_1x; 
	TX_port TX1_port(.tx_clk_i(tx_clk_5x), .tx_o(tx_o[0]), .timestamp_code_o(timestamp_code_tx1), 
	.fifo1_clk_i(1'b0), .fifo1_we_i(2'b0), .fifo1_i(9'b0), 
	.fifo2_clk_i(fifo_tx1_clk_1x), .fifo2_we_i(fifo_tx1_core_we), .fifo2_i(fifo_tx1_core));

	wire timestamp_code_tx2;
	wire[8:0] fifo_tx2_core; 
	wire[1:0] fifo_tx2_core_we; 
	wire fifo_tx2_clk_1x; 
	TX_port TX2_port(.tx_clk_i(tx_clk_5x), .tx_o(tx_o[1]), .timestamp_code_o(timestamp_code_tx2), 
	.fifo1_clk_i(1'b0), .fifo1_we_i(2'b0), .fifo1_i(9'b0), 
	.fifo2_clk_i(fifo_tx2_clk_1x), .fifo2_we_i(fifo_tx2_core_we), .fifo2_i(fifo_tx2_core)); 
 	 
///////////////////////////////////////////////////////////////////// 
 
	wire[`POINTER_WIDTH-1:0] Comm_rx1_addrw; 
	wire[7:0] Comm_rx1_dataw; 
	wire Comm_rx1_we; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_ack; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_rdy; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_wip; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_ack; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_rdy; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_wip; 
	wire[8:0] rx1_fifo_rx_o; 
	wire rx1_fifo_rx_dv; 
	wire[2:0] rx1_state_reg;

	wire rx1_crc_error; 
	wire rx1_overrun_error; 
	wire rx1_frame_error;  
	RX_core RX1_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b1),
	.fifo_clk_i(fifo_rx1_clk_5x), .fifo_we_i(fifo_rx1_port_we), .fifo_i(fifo_rx1_port), 
	.mem_addr_o(Comm_rx1_addrw), .mem_we_o(Comm_rx1_we), .mem_data_o(Comm_rx1_dataw), 
	.rx_hipri_msg_ack_i(rx1_hipri_msg_ack), .rx_hipri_msg_wip_o(rx1_hipri_msg_wip), .rx_hipri_msg_rdy_o(rx1_hipri_msg_rdy),
	.rx_lopri_msg_ack_i(rx1_lopri_msg_ack), .rx_lopri_msg_wip_o(rx1_lopri_msg_wip), .rx_lopri_msg_rdy_o(rx1_lopri_msg_rdy),
	.rx_crc_error_o(rx1_crc_error), .rx_overrun_error_o(rx1_overrun_error), .rx_frame_error_o(rx1_frame_error),
	.fifo_rx_o(rx1_fifo_rx_o), .fifo_rx_dv(rx1_fifo_rx_dv), .state_reg(rx1_state_reg)); 
 
  
	wire[`POINTER_WIDTH-1:0] Comm_rx2_addrw; 
	wire[7:0] Comm_rx2_dataw; 
	wire Comm_rx2_we; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_ack; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_rdy; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_wip; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_ack; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_rdy; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_wip;

	wire rx2_crc_error; 
	wire rx2_overrun_error; 
	wire rx2_frame_error; 
	RX_core RX2_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b1),
	.fifo_clk_i(fifo_rx2_clk_5x), .fifo_we_i(fifo_rx2_port_we), .fifo_i(fifo_rx2_port), 
	.mem_addr_o(Comm_rx2_addrw), .mem_we_o(Comm_rx2_we), .mem_data_o(Comm_rx2_dataw), 
	.rx_hipri_msg_ack_i(rx2_hipri_msg_ack), .rx_hipri_msg_wip_o(rx2_hipri_msg_wip), .rx_hipri_msg_rdy_o(rx2_hipri_msg_rdy),
	.rx_lopri_msg_ack_i(rx2_lopri_msg_ack), .rx_lopri_msg_wip_o(rx2_lopri_msg_wip), .rx_lopri_msg_rdy_o(rx2_lopri_msg_rdy),
	.rx_crc_error_o(rx2_crc_error), .rx_overrun_error_o(rx2_overrun_error), .rx_frame_error_o(rx2_frame_error)); 
 
 
	wire[`POINTER_WIDTH-1:0] Comm_tx1_addrr;
	wire[7:0] Comm_tx1_datar;
	
	wire[7:0] Comm_tx1_mem_datar;
	reg[7:0] Comm_tx1_mux_datar_r;
	reg tx1_select_memory;
	assign Comm_tx1_datar = tx1_select_memory ? Comm_tx1_mux_datar_r : Comm_tx1_mem_datar; 
	
	wire tx1_code_start; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] tx1_hipri_msg_start;
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] tx1_hipri_msg_wip; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] tx1_lopri_msg_start;
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] tx1_lopri_msg_wip; 
	TX_core TX1_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b1),
	.fifo_clk_o(fifo_tx1_clk_1x), .fifo_we_o(fifo_tx1_core_we), .fifo_o(fifo_tx1_core), 
	.mem_addr_o(Comm_tx1_addrr), .mem_data_i(Comm_tx1_datar), 
	.tx_hipri_msg_start_i(tx1_hipri_msg_start), .tx_hipri_msg_wip_o(tx1_hipri_msg_wip),
	.tx_lopri_msg_start_i(tx1_lopri_msg_start), .tx_lopri_msg_wip_o(tx1_lopri_msg_wip),
	.tx_code_start_i(tx1_code_start)); 

 
	wire[`POINTER_WIDTH-1:0] Comm_tx2_addrr; 
	wire[7:0] Comm_tx2_datar; 
		
	wire[7:0] Comm_tx2_mem_datar; 
	reg[7:0] Comm_tx2_mux_datar_r; 
	reg tx2_select_memory; 
	assign Comm_tx2_datar = tx2_select_memory ? Comm_tx2_mux_datar_r : Comm_tx2_mem_datar; 
	
	wire tx2_code_start; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] tx2_hipri_msg_start;
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] tx2_hipri_msg_wip; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] tx2_lopri_msg_start;
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] tx2_lopri_msg_wip; 
	TX_core TX2_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b1),
	.fifo_clk_o(fifo_tx2_clk_1x), .fifo_we_o(fifo_tx2_core_we), .fifo_o(fifo_tx2_core), 
	.mem_addr_o(Comm_tx2_addrr), .mem_data_i(Comm_tx2_datar), 
	.tx_hipri_msg_start_i(tx2_hipri_msg_start), .tx_hipri_msg_wip_o(tx2_hipri_msg_wip),
	.tx_lopri_msg_start_i(tx2_lopri_msg_start), .tx_lopri_msg_wip_o(tx2_lopri_msg_wip),
	.tx_code_start_i(tx2_code_start)); 
	
	wire[31:0] COMM_RX_MEM1_data_o; 
	wire[31:0] COMM_RX_MEM2_data_o;  
	
 	pmi_ram_dp
	#(.pmi_wr_addr_depth(2048),
	.pmi_wr_addr_width(11),
	.pmi_wr_data_width(8),
	.pmi_rd_addr_depth(512),
	.pmi_rd_addr_width(9),
	.pmi_rd_data_width(32),
	.pmi_regmode("noreg"),	    //"reg", "noreg"
	.pmi_gsr("enable"),		//"enable", "disable"
	.pmi_resetmode("sync"),	//"async", "sync"
	.pmi_optimization("speed"),//"speed", "area"
	.pmi_family("ECP5U")
	) COMM_RX_MEM1 (.WrAddress(Comm_rx1_addrw), .WrClock(clk_1x), .WrClockEn(1'b1), .WE(Comm_rx1_we), .Data(Comm_rx1_dataw), 
    .RdAddress(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .RdClock(!EMIF_oe_i), .RdClockEn(1'b1), .Reset(1'b0), .Q(COMM_RX_MEM1_data_o)); 

 	pmi_ram_dp
	#(.pmi_wr_addr_depth(2048),
	.pmi_wr_addr_width(11),
	.pmi_wr_data_width(8),
	.pmi_rd_addr_depth(512),
	.pmi_rd_addr_width(9),
	.pmi_rd_data_width(32),
	.pmi_regmode("noreg"),	   //"reg", "noreg"
	.pmi_gsr("enable"),		   //"enable", "disable"
	.pmi_resetmode("sync"),	   //"async", "sync"
	.pmi_optimization("speed"),//"speed", "area"
	.pmi_family("ECP5U")
	) COMM_RX_MEM2 (.WrAddress(Comm_rx2_addrw), .WrClock(clk_1x), .WrClockEn(1'b1), .WE(Comm_rx2_we), .Data(Comm_rx2_dataw),
    .RdAddress(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .RdClock(!EMIF_oe_i), .RdClockEn(1'b1), .Reset(1'b0), .Q(COMM_RX_MEM2_data_o));
	
	parameter COMM_TX_MEM1_key = 3'd2;
	pmi_ram_dp
	#(.pmi_wr_addr_depth(512),
	.pmi_wr_addr_width(9),
	.pmi_wr_data_width(32),
	.pmi_rd_addr_depth(2048),
	.pmi_rd_addr_width(11),
	.pmi_rd_data_width(8),
	.pmi_regmode("noreg"),	    //"reg", "noreg"
	.pmi_gsr("enable"),		//"enable", "disable"
	.pmi_resetmode("sync"),	//"async", "sync"
	.pmi_optimization("speed"),//"speed", "area"
	.pmi_family("ECP5U")
	) COMM_TX_MEM1 (.WrAddress(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .WrClock(EMIF_we_i),
	.WrClockEn(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == COMM_TX_MEM1_key[1 +: 2]), .WE(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == COMM_TX_MEM1_key[0]), .Data(EMIF_data_i), 
    .RdAddress(Comm_tx1_addrr), .RdClock(clk_1x), .RdClockEn(1'b1), .Reset(1'b0), .Q(Comm_tx1_mem_datar));  

	parameter COMM_TX_MEM2_key = 3'd3;
	pmi_ram_dp
	#(.pmi_wr_addr_depth(512),
	.pmi_wr_addr_width(9),
	.pmi_wr_data_width(32),
	.pmi_rd_addr_depth(2048),
	.pmi_rd_addr_width(11),
	.pmi_rd_data_width(8),
	.pmi_regmode("noreg"),	   //"reg", "noreg"
	.pmi_gsr("enable"),		   //"enable", "disable"
	.pmi_resetmode("sync"),	   //"async", "sync"
	.pmi_optimization("speed"),//"speed", "area"
	.pmi_family("ECP5U")
	) COMM_TX_MEM2 (.WrAddress(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .WrClock(EMIF_we_i),
	.WrClockEn(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == COMM_TX_MEM2_key[1 +: 2]), .WE(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == COMM_TX_MEM2_key[0]), .Data(EMIF_data_i),
    .RdAddress(Comm_tx2_addrr), .RdClock(clk_1x), .RdClockEn(1'b1), .Reset(1'b0), .Q(Comm_tx2_mem_datar));


////////////////////////////////////////////////////////////////

	wire clk_DSP;
	assign clk_DSP = clk_5x;
	
	wire Resonant1_WIP;
	wire Resonant1_START;
	
	wire Resonant1_Mem2_we;
	wire[8:0] Resonant1_Mem2_addrw;
	wire[35:0] Resonant1_Mem2_data;
	wire[31:0] Resonant1_data_o;

	parameter Resonant1_Mem2_key = 3'd4;
	Res_ctrl Res_ctrl1(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant1_Mem2_key[1 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == Resonant1_Mem2_key[0]),
	.enable_i(Resonant1_START), .Mem0_addrw_o(Resonant1_Mem2_addrw), .Mem0_we_o(Resonant1_Mem2_we), .Mem0_data_io(Resonant1_Mem2_data), .WIP_flag_o(Resonant1_WIP));

	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("disable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), 
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/), .pmi_family()
	)
	Resonant1_Mem2(.Data(Resonant1_Mem2_data[35:4]), .WrAddress(Resonant1_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant1_Mem2_we), .Reset(1'b0), .Q(Resonant1_data_o));


	wire Resonant2_WIP;
	wire Resonant2_START;
	
	wire Resonant2_Mem2_we;
	wire[8:0] Resonant2_Mem2_addrw;
	wire[35:0] Resonant2_Mem2_data;
	wire[31:0] Resonant2_data_o;

	parameter Resonant2_Mem2_key = 3'd5;
	Res_ctrl Res_ctrl2(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant2_Mem2_key[1 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == Resonant2_Mem2_key[0]),
	.enable_i(Resonant2_START), .Mem0_addrw_o(Resonant2_Mem2_addrw), .Mem0_we_o(Resonant2_Mem2_we), .Mem0_data_io(Resonant2_Mem2_data), .WIP_flag_o(Resonant2_WIP));

	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("disable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), 
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/), .pmi_family()
	)
	Resonant2_Mem2(.Data(Resonant2_Mem2_data[35:4]), .WrAddress(Resonant2_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant2_Mem2_we), .Reset(1'b0), .Q(Resonant2_data_o));
	
	wire Kalman1_WIP;
	wire Kalman1_START;
	
	wire Kalman1_Mem2_we;
	wire[8:0] Kalman1_Mem2_addrw;
	wire[35:0] Kalman1_Mem2_data;
	wire[31:0] Kalman1_data_o;

	parameter Kalman1_Mem2_key = 3'd6;
	Kalman Kalman1(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Kalman1_Mem2_key[1 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == Kalman1_Mem2_key[0]),
	.enable_i(Kalman1_START), .Mem0_addrw_o(Kalman1_Mem2_addrw), .Mem0_we_o(Kalman1_Mem2_we), .Mem0_data_io(Kalman1_Mem2_data), .WIP_flag_o(Kalman1_WIP));

	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("disable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), 
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/), .pmi_family()
	)
	Kalman1_Mem2(.Data(Kalman1_Mem2_data[35:4]), .WrAddress(Kalman1_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Kalman1_Mem2_we), .Reset(1'b0), .Q(Kalman1_data_o));
	
	wire Kalman2_WIP;
	wire Kalman2_START;
	
	wire Kalman2_Mem2_we;
	wire[8:0] Kalman2_Mem2_addrw;
	wire[35:0] Kalman2_Mem2_data;
	wire[31:0] Kalman2_data_o;

	parameter Kalman2_Mem2_key = 3'd7;
	Kalman Kalman2(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Kalman2_Mem2_key[1 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == Kalman2_Mem2_key[0]),
	.enable_i(Kalman2_START), .Mem0_addrw_o(Kalman2_Mem2_addrw), .Mem0_we_o(Kalman2_Mem2_we), .Mem0_data_io(Kalman2_Mem2_data), .WIP_flag_o(Kalman2_WIP));

	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("disable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), 
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/), .pmi_family()
	)
	Kalman2_Mem2(.Data(Kalman2_Mem2_data[35:4]), .WrAddress(Kalman2_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Kalman2_Mem2_we), .Reset(1'b0), .Q(Kalman2_data_o));

///////////////////////////////////////////////////////////////////// 
 
	localparam OSR = 320; 
	localparam SD_WIDTH = 16; 
	localparam SD_NUMBER = 11; 
	wire[1:0] decimator_pulse; 
	wire new_value; 
	wire new_value_early; 
	reg SD_sync_pulse; 
	wire select_SD; 
	SD_filter_sync_gen #(.OSR(OSR)) SD_filter_sync_gen1(.clk_i(XTAL_20MHz_i), .sync_pulse_i(SD_sync_pulse), 
	.decimator_pulse_o(decimator_pulse), .select_o(select_SD), .new_value_o(new_value), .new_value_early_o(new_value_early)); 
 
	wire[SD_NUMBER-1:0] SD_DAT; 
	wire[SD_NUMBER-1:0] SD_DAT_IDDR; 
	wire[5-1:0] SD_CLK; 
 	ODDRX1F SD_CLK_gen[5-1:0] (.D0(1'b0), .D1(1'b1), .SCLK(XTAL_20MHz_i), .RST(1'b0), .Q(SD_CLK)); 
	
    IDDRX1F SD_IDDR[SD_NUMBER-1:0](.D(SD_DAT), .RST(1'b0), .SCLK(XTAL_20MHz_i), .Q0(SD_DAT_IDDR), .Q1());
 
	wire[SD_WIDTH*SD_NUMBER-1:0] SD_dat; 
	SD_filter_sync #(.ORDER(2), .OSR(OSR), .OUTPUT_WIDTH(SD_WIDTH)) SD_filter_sync[SD_NUMBER-1:0](.data_i(SD_DAT_IDDR), 
	.clk_i(XTAL_20MHz_i), .decimator_pulse_i(decimator_pulse), .select_i(select_SD), .data_o(SD_dat)); 
 
///////////////////////////////////////////////////////////////////// 
	
	wire SED_enable;
	SED_machine SED_machine(.clk_i(XTAL_20MHz_i), .enable_i(SED_enable), .err_o(sed_err));

	localparam FAULT_NUMBER = 25; 
 
	wire[FAULT_NUMBER-1:0] FLT_REG_O; 
	wire[FAULT_NUMBER-1:0] FLT_bus; 
 
	assign FLT_bus[16] = !rx1_crc_error; 
	assign FLT_bus[17] = !rx1_overrun_error; 
	assign FLT_bus[18] = !rx1_frame_error; 
	assign FLT_bus[19] = !rx2_crc_error; 
	assign FLT_bus[20] = !rx2_overrun_error; 
	assign FLT_bus[21] = !rx2_frame_error; 
	assign FLT_bus[22] = rx1_port_rdy; 
	assign FLT_bus[23] = rx2_port_rdy; 
	assign FLT_bus[24] = !sed_err;
 
 	wire rst_faults;
	FD1P3BX FLT_ff[FAULT_NUMBER-1:0](.D(1'b0), .SP(rst_faults), .CK(clk_5x), .PD(~FLT_bus[FAULT_NUMBER-1:0]), .Q(FLT_REG_O[FAULT_NUMBER-1:0])); 
 
  	wire TZ_FPGA;
	assign TZ_FPGA = !(FLT_REG_O[24] | |FLT_REG_O[15:0]);
	
/////////////////////////////////////////////////////////////////////  
	 	
	defparam TX1_port.TIMESTAMP_CODE = `K_Timestamp_master;
	defparam TX2_port.TIMESTAMP_CODE = `K_Timestamp_master;
	defparam RX1_port.TIMESTAMP_CODE = `K_Timestamp_slave;
	defparam RX2_port.TIMESTAMP_CODE = `K_Timestamp_slave;	
	
	wire[63:0] snapshot_value;
	wire[15:0] local_counter; 
	wire[15:0] current_period;
	wire[15:0] next_period;
	wire sync_phase;
	Local_counter Local_counter(.clk_i(clk_5x), .next_period_i(next_period), .current_period_o(current_period), .local_counter_o(local_counter), .sync_phase_o(sync_phase), 
	.snapshot_start_i({timestamp_code_rx2, timestamp_code_tx2, timestamp_code_rx1, timestamp_code_tx1}), .snapshot_value_o(snapshot_value)); 
	assign next_period = `CYCLE_PERIOD - 16'd1;
	
	always @(posedge clk_5x)
		SD_sync_pulse <= local_counter >= EMIF_RX_reg[2][15:0];
	 
	reg pulse_cycle_r;
	always @(posedge clk_5x) 
		pulse_cycle_r = local_counter == `CYCLE_PERIOD - (`CYCLE_PERIOD>>2); 
		
	wire[`HIPRI_MAILBOXES_NUMBER*16-1:0] clock_offsets;
	wire[`HIPRI_MAILBOXES_NUMBER*16-1:0] comm_delays; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx_ok; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] sync_ok; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] slave_rdy;
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] scope_trigger_request;
	Master_sync Master_sync(.clk_i(clk_1x), .pulse_cycle_i(pulse_cycle_r),
	.rx_addrw_i(Comm_rx1_addrw), .rx_dataw_i(Comm_rx1_dataw), .rx_we_i(Comm_rx1_we),
	.snapshot_value_i(snapshot_value), .clock_offsets_o(clock_offsets), .comm_delays_o(comm_delays),
	.rx_ok_o(rx_ok), .sync_ok_o(sync_ok), .slave_rdy_o(slave_rdy), .scope_trigger_request_o(scope_trigger_request));
	
	assign rx1_hipri_msg_ack = EMIF_RX_reg[1][1*8 +: `HIPRI_MAILBOXES_NUMBER]; 
	assign rx2_hipri_msg_ack = EMIF_RX_reg[1][3*8 +: `HIPRI_MAILBOXES_NUMBER]; 

	assign rx1_lopri_msg_ack = EMIF_RX_reg[1][0*8 +: `LOPRI_MAILBOXES_NUMBER]; 
	assign rx2_lopri_msg_ack = EMIF_RX_reg[1][2*8 +: `LOPRI_MAILBOXES_NUMBER]; 
	
	assign tx1_hipri_msg_start = {EMIF_RX_reg[0][1*8+2 +: `HIPRI_MAILBOXES_NUMBER-2], new_value, new_value_early}; 
	assign tx2_hipri_msg_start = EMIF_RX_reg[0][3*8 +: `HIPRI_MAILBOXES_NUMBER]; 
	
	assign tx1_lopri_msg_start = EMIF_RX_reg[0][0*8 +: `LOPRI_MAILBOXES_NUMBER]; 
	assign tx2_lopri_msg_start = EMIF_RX_reg[0][2*8 +: `LOPRI_MAILBOXES_NUMBER]; 

 	defparam TX1_core.TX_CODE = `K_Timestamp_master;
	defparam TX2_core.TX_CODE = `K_Timestamp_master;
	assign tx1_code_start = pulse_cycle_r; 
	assign tx2_code_start = pulse_cycle_r;
 
	always @(posedge clk_1x) begin
		tx1_select_memory <= 1'b1;
		tx2_select_memory <= 1'b1;
		case(Comm_tx1_addrr)
			11'h400 : Comm_tx1_mux_datar_r <= {4'd0, 4'd0};
			11'h401 : Comm_tx1_mux_datar_r <= 8'd8 - 8'd1;
			11'h402 : Comm_tx1_mux_datar_r <= SD_dat[0*8 +: 8];
			11'h403 : Comm_tx1_mux_datar_r <= SD_dat[1*8 +: 8];
			11'h404 : Comm_tx1_mux_datar_r <= SD_dat[2*8 +: 8];
			11'h405 : Comm_tx1_mux_datar_r <= SD_dat[3*8 +: 8];
			11'h406 : Comm_tx1_mux_datar_r <= SD_dat[4*8 +: 8];
			11'h407 : Comm_tx1_mux_datar_r <= SD_dat[5*8 +: 8]; 
			11'h408 : Comm_tx1_mux_datar_r <= SD_dat[6*8 +: 8];
			11'h409 : Comm_tx1_mux_datar_r <= SD_dat[7*8 +: 8];
			11'h40A : Comm_tx1_mux_datar_r <= SD_dat[8*8 +: 8];
			11'h40B : Comm_tx1_mux_datar_r <= SD_dat[9*8 +: 8];
			11'h40C : Comm_tx1_mux_datar_r <= SD_dat[10*8 +: 8];
			11'h40D : Comm_tx1_mux_datar_r <= SD_dat[11*8 +: 8]; 
			 
			11'h480 : Comm_tx1_mux_datar_r <= {4'd0, 4'd1};
			11'h481 : Comm_tx1_mux_datar_r <= 8'd7 - 8'd1;
			11'h482 : Comm_tx1_mux_datar_r <= {{8-`HIPRI_MAILBOXES_NUMBER{1'b0}}, rx_ok};
			11'h483 : Comm_tx1_mux_datar_r <= {7'b0, sync_phase};
			11'h484 : Comm_tx1_mux_datar_r <= clock_offsets[0*8 +: 8];
			11'h485 : Comm_tx1_mux_datar_r <= clock_offsets[1*8 +: 8];
			11'h486 : Comm_tx1_mux_datar_r <= clock_offsets[2*8 +: 8];
			11'h487 : Comm_tx1_mux_datar_r <= clock_offsets[3*8 +: 8];
			11'h488 : Comm_tx1_mux_datar_r <= clock_offsets[4*8 +: 8];
			11'h489 : Comm_tx1_mux_datar_r <= clock_offsets[5*8 +: 8]; 
			11'h48A : Comm_tx1_mux_datar_r <= clock_offsets[6*8 +: 8]; 
			11'h48B : Comm_tx1_mux_datar_r <= clock_offsets[7*8 +: 8];
			default : begin
				tx1_select_memory <= 1'b0;
				Comm_tx1_mux_datar_r <= 8'd0;
			end
		endcase
		case(Comm_tx2_addrr)
			default : begin
				tx2_select_memory <= 1'b0;
				Comm_tx2_mux_datar_r <= 8'd0;
			end
		endcase
	end 
	
///////////////////////////////////////////////////////////////////// 

	localparam DEB_WRITE_WIDTH_MULTIPLY = 2;
	localparam DEB_WADDR_WIDTH = 8;
		localparam DEB_READ_WIDTH = 36;
	localparam DEB_WRITE_WIDTH = DEB_WRITE_WIDTH_MULTIPLY*DEB_READ_WIDTH;
	localparam DEB_WADDR_DEPTH = 2**DEB_WADDR_WIDTH;
	localparam DEB_RADDR_DEPTH = DEB_WRITE_WIDTH_MULTIPLY*DEB_WADDR_DEPTH;
	localparam DEB_RADDR_WIDTH = $clog2(DEB_RADDR_DEPTH);
	
	wire Scope_enable;
	wire Scope_enable_extended;
	reg[5:0] Scope_enable_counter;
	wire Scope_trigger;
	reg Scope_trigger_r;
	wire Scope_WE;
	wire[DEB_READ_WIDTH-1:0] Scope_data_out;
	wire[DEB_WRITE_WIDTH-1:0] Scope_data_in;
	reg[DEB_WADDR_WIDTH-1:0] Scope_index;
	reg[DEB_WADDR_WIDTH-1:0] Scope_index_last;
	reg[DEB_WADDR_WIDTH-1:0] Scope_acquire_counter;
	reg[DEB_WADDR_WIDTH-1:0] Scope_acquire_before_counter;
	wire[DEB_WADDR_WIDTH-1:0] Scope_acquire_before_trigger;
	
	localparam STATES_WIDTH = 2;
	localparam [STATES_WIDTH-1:0]
    Scope_0 = 0,
    Scope_1 = 1,
    Scope_2 = 2,
	Scope_3 = 3;
	reg[STATES_WIDTH-1:0] Scope_state;
	
	always @(posedge clk_1x) begin
		if(Scope_enable)
			Scope_enable_counter <= -1;
		else if (Scope_enable_counter)
			Scope_enable_counter <= Scope_enable_counter - 1'b1;
			
		Scope_trigger_r <= Scope_trigger;
		if(Scope_enable_extended) begin
			if(Scope_acquire_before_counter != Scope_acquire_before_trigger)
				Scope_acquire_before_counter <= Scope_acquire_before_counter + 1'b1;
			Scope_acquire_counter <= Scope_acquire_counter - 1'b1;
			Scope_index <= Scope_index + 1'b1;
			case(Scope_state)
				Scope_0 : begin
					Scope_acquire_counter <= DEB_WADDR_DEPTH - Scope_acquire_before_trigger;
					if(Scope_trigger_r)
						Scope_state <= Scope_1;
				end
				Scope_1 : begin
					Scope_index_last <= Scope_index;
					if(Scope_acquire_before_counter != Scope_acquire_before_trigger)
						Scope_acquire_counter <= Scope_acquire_counter;
					if(!Scope_acquire_counter)
						Scope_state <= Scope_2;
				end
				Scope_2 : begin
					Scope_acquire_before_counter <= 0;
					Scope_index <= 0;
					if(!Scope_trigger_r)
						Scope_state <= Scope_0;
				end
				Scope_3 : begin
					Scope_acquire_before_counter <= 0;
					Scope_index <= 0;
					if(!Scope_trigger_r)
						Scope_state <= Scope_0;
				end
			endcase
		end
	end
	assign Scope_enable_extended = Scope_enable || Scope_enable_counter;
	assign Scope_WE = Scope_enable_extended && (Scope_state == Scope_0 || Scope_state == Scope_1);
	
	pmi_ram_dp
	#(.pmi_wr_addr_depth(DEB_WADDR_DEPTH),
	.pmi_wr_addr_width(DEB_WADDR_WIDTH),
	.pmi_wr_data_width(DEB_WRITE_WIDTH),
	.pmi_rd_addr_depth(DEB_RADDR_DEPTH),
	.pmi_rd_addr_width(DEB_RADDR_WIDTH),
	.pmi_rd_data_width(DEB_READ_WIDTH),
	.pmi_regmode("noreg"),	    //"reg", "noreg"
	.pmi_gsr("enable"),		//"enable", "disable"
	.pmi_resetmode("sync"),	//"async", "sync"
	.pmi_optimization("speed"),//"speed", "area"
	.pmi_family("ECP5U")
	) Debugger_memory (.WrAddress(Scope_index), .WrClock(clk_1x), .WrClockEn(1'b1), .WE(Scope_WE), .Data(Scope_data_in), 
    .RdAddress(EMIF_RX_reg[3][DEB_RADDR_WIDTH-1:0]), .RdClock(!EMIF_oe_i), .RdClockEn(1'b1), .Reset(1'b0), .Q(Scope_data_out)); 

	reg[31:0] timestamp_counter;
	reg[31:0] timestamp_mem;
	wire[31:0] timestamp_diff;
	
	assign timestamp_diff = timestamp_counter - timestamp_mem;
	always @(posedge clk_1x) begin
		timestamp_counter <= timestamp_counter + 1'b1;
		if(Scope_enable_extended) timestamp_mem <= timestamp_counter;
	end
	
	assign Scope_acquire_before_trigger = EMIF_RX_reg[4][DEB_WADDR_WIDTH-1:0];
	assign Scope_enable = rx1_lopri_msg_rdy[0] | (|rx1_lopri_msg_wip) | fifo_tx1_core_we[1] | (rx1_fifo_rx_o == `K_Start_Lopri_Packet);
	assign Scope_data_in = {rx1_state_reg, rx1_lopri_msg_rdy[3:0], rx1_lopri_msg_wip[3:0], rx1_fifo_rx_o, rx1_fifo_rx_dv, tx1_lopri_msg_wip[3:0], fifo_tx1_core, fifo_tx1_core_we, Scope_trigger_r, timestamp_diff};
	assign Scope_trigger = EMIF_RX_reg[5][0];
	
///////////////////////////////////////////////////////////////////// 
 
	always @(*) begin
		case(EMIF_address_i[EMIF_MEMORY_WIDTH-3 +: 3])
			0 : EMIF_data_o = EMIF_TX_mux[EMIF_address_i[EMIF_MUX_WIDTH-1:0]];
			1 : EMIF_data_o = 0; 
			2 : EMIF_data_o = COMM_RX_MEM1_data_o; 
			3 : EMIF_data_o = COMM_RX_MEM2_data_o;
			4 : EMIF_data_o = Resonant1_data_o;
			5 : EMIF_data_o = Resonant2_data_o;
			6 : EMIF_data_o = Kalman1_data_o;
			7 : EMIF_data_o = Kalman2_data_o;
		endcase
	end
	//assign EMIF_data_o = EMIF_address_i[EMIF_MEMORY_WIDTH-1] ? (EMIF_address_i[EMIF_MEMORY_WIDTH-2] ? COMM_RX_MEM2_data_o : COMM_RX_MEM1_data_o) : EMIF_TX_mux[EMIF_address_i[EMIF_MUX_WIDTH-1:0]]; 
 
	assign EMIF_TX_mux[0] = {tx2_hipri_msg_wip, tx2_lopri_msg_wip, tx1_hipri_msg_wip, tx1_lopri_msg_wip}; 
	assign EMIF_TX_mux[1] = {rx2_hipri_msg_rdy, rx2_lopri_msg_rdy, rx1_hipri_msg_rdy, rx1_lopri_msg_rdy}; 
	assign EMIF_TX_mux[2] = SD_dat[0*32 +: 32]; 
	assign EMIF_TX_mux[3] = SD_dat[1*32 +: 32]; 
	assign EMIF_TX_mux[4] = SD_dat[2*32 +: 32]; 
	assign EMIF_TX_mux[5] = SD_dat[3*32 +: 32]; 
	assign EMIF_TX_mux[6] = SD_dat[4*32 +: 32]; 
	assign EMIF_TX_mux[7] = SD_dat[5*32 +: 16]; 
	assign EMIF_TX_mux[8] = {{32-FAULT_NUMBER{1'b0}}, FLT_REG_O}; 
	assign EMIF_TX_mux[9] = {8'b0, slave_rdy, sync_ok, rx_ok}; 
	assign EMIF_TX_mux[10] = clock_offsets[0*32 +: 32]; 
	assign EMIF_TX_mux[11] = clock_offsets[1*32 +: 32]; 
	assign EMIF_TX_mux[12] = clock_offsets[2*32 +: 32];
	assign EMIF_TX_mux[13] = clock_offsets[3*32 +: 32];
	assign EMIF_TX_mux[14] = comm_delays[0*32 +: 32];
	assign EMIF_TX_mux[15] = comm_delays[1*32 +: 32];
	assign EMIF_TX_mux[16] = comm_delays[2*32 +: 32];
	assign EMIF_TX_mux[17] = comm_delays[3*32 +: 32];
	assign EMIF_TX_mux[18] = EMIF_RX_reg[2];
	assign EMIF_TX_mux[19] = `VERSION;
	assign EMIF_TX_mux[20] = Scope_data_out[31:0];
	assign EMIF_TX_mux[21] = {28'b0, Scope_data_out[DEB_READ_WIDTH-1:32]};
	assign EMIF_TX_mux[22] = DEB_RADDR_DEPTH;
	assign EMIF_TX_mux[23] = DEB_WRITE_WIDTH_MULTIPLY;
	assign EMIF_TX_mux[24] = Scope_state[1];
	assign EMIF_TX_mux[25] = Scope_index_last;
	
 	FD1P3DX EMIF_RX_reg_0[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 0), .CK(EMIF_we_i), .CD({tx2_hipri_msg_wip, tx2_lopri_msg_wip, tx1_hipri_msg_wip, tx1_lopri_msg_wip}), .Q(EMIF_RX_reg[0]));
 	FD1P3DX EMIF_RX_reg_1[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 1), .CK(EMIF_we_i), .CD(~{rx2_hipri_msg_rdy, rx2_lopri_msg_rdy, rx1_hipri_msg_rdy, rx1_lopri_msg_rdy}), .Q(EMIF_RX_reg[1]));
	EMIF_RX_reg #(.INIT_VAL(32'd1625)) EMIF_RX_reg_2(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 2), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[2]));
	EMIF_RX_reg #(.INIT_VAL(32'd0)) EMIF_RX_reg_3(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 3), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[3]));
	EMIF_RX_reg #(.INIT_VAL(DEB_WADDR_DEPTH>>1)) EMIF_RX_reg_4(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 4), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[4]));
	EMIF_RX_reg #(.INIT_VAL(32'd0)) EMIF_RX_reg_5(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 5), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[5]));
 	FD1P3DX EMIF_RX_reg_6[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 6), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg[6]));
 	FD1P3DX EMIF_RX_reg_7[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 7), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg[7]));
 	FD1P3DX EMIF_RX_reg_8[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 8), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg[8]));

///////////////////////////////////////////////////////////////////// 

	BB BB_EMIF_OE(.I(1'b0), .T(1'b1), .O(EMIF_oe_i), .B(CPU_io[`EM1OE]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/; 
	BB BB_EMIF_WE(.I(1'b0), .T(1'b1), .O(EMIF_we_i), .B(CPU_io[`EM1WE]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/; 
	BB BB_EMIF_CS(.I(1'b0), .T(1'b1), .O(EMIF_cs_i), .B(CPU_io[`EM1CS0]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/; 
 
	`define EMIF_data_options /*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" SLEWRATE="FAST" DRIVE="8"*/
	BB BB_EMIF_data0(.I(EMIF_data_o[0]), .T(EMIF_oe_i), .O(EMIF_data_i[0]), .B(CPU_io[`EM1D0])) `EMIF_data_options; 
	BB BB_EMIF_data1(.I(EMIF_data_o[1]), .T(EMIF_oe_i), .O(EMIF_data_i[1]), .B(CPU_io[`EM1D1])) `EMIF_data_options; 
	BB BB_EMIF_data2(.I(EMIF_data_o[2]), .T(EMIF_oe_i), .O(EMIF_data_i[2]), .B(CPU_io[`EM1D2])) `EMIF_data_options; 
	BB BB_EMIF_data3(.I(EMIF_data_o[3]), .T(EMIF_oe_i), .O(EMIF_data_i[3]), .B(CPU_io[`EM1D3])) `EMIF_data_options; 
	BB BB_EMIF_data4(.I(EMIF_data_o[4]), .T(EMIF_oe_i), .O(EMIF_data_i[4]), .B(CPU_io[`EM1D4])) `EMIF_data_options; 
	BB BB_EMIF_data5(.I(EMIF_data_o[5]), .T(EMIF_oe_i), .O(EMIF_data_i[5]), .B(CPU_io[`EM1D5])) `EMIF_data_options; 
	BB BB_EMIF_data6(.I(EMIF_data_o[6]), .T(EMIF_oe_i), .O(EMIF_data_i[6]), .B(CPU_io[`EM1D6])) `EMIF_data_options; 
	BB BB_EMIF_data7(.I(EMIF_data_o[7]), .T(EMIF_oe_i), .O(EMIF_data_i[7]), .B(CPU_io[`EM1D7])) `EMIF_data_options; 
	BB BB_EMIF_data8(.I(EMIF_data_o[8]), .T(EMIF_oe_i), .O(EMIF_data_i[8]), .B(CPU_io[`EM1D8])) `EMIF_data_options; 
	BB BB_EMIF_data9(.I(EMIF_data_o[9]), .T(EMIF_oe_i), .O(EMIF_data_i[9]), .B(CPU_io[`EM1D9])) `EMIF_data_options; 
	BB BB_EMIF_data10(.I(EMIF_data_o[10]), .T(EMIF_oe_i), .O(EMIF_data_i[10]), .B(CPU_io[`EM1D10])) `EMIF_data_options; 
	BB BB_EMIF_data11(.I(EMIF_data_o[11]), .T(EMIF_oe_i), .O(EMIF_data_i[11]), .B(CPU_io[`EM1D11])) `EMIF_data_options; 
	BB BB_EMIF_data12(.I(EMIF_data_o[12]), .T(EMIF_oe_i), .O(EMIF_data_i[12]), .B(CPU_io[`EM1D12])) /*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" SLEWRATE="FAST" DRIVE="8"*/;
	BB BB_EMIF_data13(.I(EMIF_data_o[13]), .T(EMIF_oe_i), .O(EMIF_data_i[13]), .B(CPU_io[`EM1D13])) `EMIF_data_options; 
	BB BB_EMIF_data14(.I(EMIF_data_o[14]), .T(EMIF_oe_i), .O(EMIF_data_i[14]), .B(CPU_io[`EM1D14])) `EMIF_data_options; 
	BB BB_EMIF_data15(.I(EMIF_data_o[15]), .T(EMIF_oe_i), .O(EMIF_data_i[15]), .B(CPU_io[`EM1D15])) `EMIF_data_options; 
	BB BB_EMIF_data16(.I(EMIF_data_o[16]), .T(EMIF_oe_i), .O(EMIF_data_i[16]), .B(CPU_io[`EM1D16])) `EMIF_data_options; 
	BB BB_EMIF_data17(.I(EMIF_data_o[17]), .T(EMIF_oe_i), .O(EMIF_data_i[17]), .B(CPU_io[`EM1D17])) `EMIF_data_options; 
	BB BB_EMIF_data18(.I(EMIF_data_o[18]), .T(EMIF_oe_i), .O(EMIF_data_i[18]), .B(CPU_io[`EM1D18])) `EMIF_data_options; 
	BB BB_EMIF_data19(.I(EMIF_data_o[19]), .T(EMIF_oe_i), .O(EMIF_data_i[19]), .B(CPU_io[`EM1D19])) `EMIF_data_options; 
	BB BB_EMIF_data20(.I(EMIF_data_o[20]), .T(EMIF_oe_i), .O(EMIF_data_i[20]), .B(CPU_io[`EM1D20])) `EMIF_data_options; 
	BB BB_EMIF_data21(.I(EMIF_data_o[21]), .T(EMIF_oe_i), .O(EMIF_data_i[21]), .B(CPU_io[`EM1D21])) `EMIF_data_options; 
	BB BB_EMIF_data22(.I(EMIF_data_o[22]), .T(EMIF_oe_i), .O(EMIF_data_i[22]), .B(CPU_io[`EM1D22])) `EMIF_data_options; 
	BB BB_EMIF_data23(.I(EMIF_data_o[23]), .T(EMIF_oe_i), .O(EMIF_data_i[23]), .B(CPU_io[`EM1D23])) `EMIF_data_options; 
	BB BB_EMIF_data24(.I(EMIF_data_o[24]), .T(EMIF_oe_i), .O(EMIF_data_i[24]), .B(CPU_io[`EM1D24])) `EMIF_data_options; 
	BB BB_EMIF_data25(.I(EMIF_data_o[25]), .T(EMIF_oe_i), .O(EMIF_data_i[25]), .B(CPU_io[`EM1D25])) `EMIF_data_options; 
	BB BB_EMIF_data26(.I(EMIF_data_o[26]), .T(EMIF_oe_i), .O(EMIF_data_i[26]), .B(CPU_io[`EM1D26])) `EMIF_data_options; 
	BB BB_EMIF_data27(.I(EMIF_data_o[27]), .T(EMIF_oe_i), .O(EMIF_data_i[27]), .B(CPU_io[`EM1D27])) `EMIF_data_options; 
	BB BB_EMIF_data28(.I(EMIF_data_o[28]), .T(EMIF_oe_i), .O(EMIF_data_i[28]), .B(CPU_io[`EM1D28])) `EMIF_data_options; 
	BB BB_EMIF_data29(.I(EMIF_data_o[29]), .T(EMIF_oe_i), .O(EMIF_data_i[29]), .B(CPU_io[`EM1D29])) `EMIF_data_options; 
	BB BB_EMIF_data30(.I(EMIF_data_o[30]), .T(EMIF_oe_i), .O(EMIF_data_i[30]), .B(CPU_io[`EM1D30])) `EMIF_data_options; 
	BB BB_EMIF_data31(.I(EMIF_data_o[31]), .T(EMIF_oe_i), .O(EMIF_data_i[31]), .B(CPU_io[`EM1D31])) `EMIF_data_options; 
 
	`define EMIF_address_options /*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */ 
	BB BB_EMIF_address0(.I(1'b0), .T(1'b1), .O(EMIF_address_i[0]), .B(CPU_io[`EM1A0])) `EMIF_address_options; 
	BB BB_EMIF_address1(.I(1'b0), .T(1'b1), .O(EMIF_address_i[1]), .B(CPU_io[`EM1A1])) `EMIF_address_options; 
	BB BB_EMIF_address2(.I(1'b0), .T(1'b1), .O(EMIF_address_i[2]), .B(CPU_io[`EM1A2])) `EMIF_address_options; 
	BB BB_EMIF_address3(.I(1'b0), .T(1'b1), .O(EMIF_address_i[3]), .B(CPU_io[`EM1A3])) `EMIF_address_options; 
	BB BB_EMIF_address4(.I(1'b0), .T(1'b1), .O(EMIF_address_i[4]), .B(CPU_io[`EM1A4])) `EMIF_address_options; 
	BB BB_EMIF_address5(.I(1'b0), .T(1'b1), .O(EMIF_address_i[5]), .B(CPU_io[`EM1A5])) `EMIF_address_options; 
	BB BB_EMIF_address6(.I(1'b0), .T(1'b1), .O(EMIF_address_i[6]), .B(CPU_io[`EM1A6])) `EMIF_address_options; 
	BB BB_EMIF_address7(.I(1'b0), .T(1'b1), .O(EMIF_address_i[7]), .B(CPU_io[`EM1A7])) `EMIF_address_options; 
	BB BB_EMIF_address8(.I(1'b0), .T(1'b1), .O(EMIF_address_i[8]), .B(CPU_io[`EM1A8])) `EMIF_address_options; 
	BB BB_EMIF_address9(.I(1'b0), .T(1'b1), .O(EMIF_address_i[9]), .B(CPU_io[`EM1A9])) `EMIF_address_options; 
	BB BB_EMIF_address10(.I(1'b0), .T(1'b1), .O(EMIF_address_i[10]), .B(CPU_io[`EM1A10])) `EMIF_address_options; 
	BB BB_EMIF_address11(.I(1'b0), .T(1'b1), .O(EMIF_address_i[11]), .B(CPU_io[`EM1A11])) `EMIF_address_options; 


	BB BB_RST(.I(1'b0), .T(1'b1), .O(rst_faults), .B(CPU_io[`RST_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_NEW(.I(new_value), .T(1'b0), .O(), .B(CPU_io[`SD_NEW_CM]))/*synthesis IO_TYPE="LVCMOS33"*/; 
	BB BB_SYNC_PWM(.I(sync_phase), .T(1'b0), .O(), .B(CPU_io[`SYNC_PWM_CM]))/*synthesis IO_TYPE="LVCMOS33"*/; 

	BB BB_SD_CLK0(.I(SD_CLK[0]), .T(1'b0), .O(), .B(FPGA_io[`SD_CLK_UDC_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK1(.I(SD_CLK[1]), .T(1'b0), .O(), .B(FPGA_io[`SD_CLK_I_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK2(.I(SD_CLK[2]), .T(1'b0), .O(), .B(FPGA_io[`SD_CLK_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK3(.I(SD_CLK[3]), .T(1'b0), .O(), .B(FPGA_io[`SD_CLK_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK4(.I(SD_CLK[4]), .T(1'b0), .O(), .B(FPGA_io[`SD_CLK_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */;
 
	BB BB_SD_DAT0(.I(1'b0), .T(1'b1), .O(SD_DAT[0]), .B(FPGA_io[`SD_U_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT1(.I(1'b0), .T(1'b1), .O(SD_DAT[1]), .B(FPGA_io[`SD_U_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT2(.I(1'b0), .T(1'b1), .O(SD_DAT[2]), .B(FPGA_io[`SD_U_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT3(.I(1'b0), .T(1'b1), .O(SD_DAT[3]), .B(FPGA_io[`SD_ITR_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT4(.I(1'b0), .T(1'b1), .O(SD_DAT[4]), .B(FPGA_io[`SD_ITR_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT5(.I(1'b0), .T(1'b1), .O(SD_DAT[5]), .B(FPGA_io[`SD_ITR_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 	BB BB_SD_DAT6(.I(1'b0), .T(1'b1), .O(SD_DAT[6]), .B(FPGA_io[`SD_I_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT7(.I(1'b0), .T(1'b1), .O(SD_DAT[7]), .B(FPGA_io[`SD_I_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT8(.I(1'b0), .T(1'b1), .O(SD_DAT[8]), .B(FPGA_io[`SD_I_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT9(.I(1'b0), .T(1'b1), .O(SD_DAT[9]), .B(FPGA_io[`SD_UDC_1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT10(.I(1'b0), .T(1'b1), .O(SD_DAT[10]), .B(FPGA_io[`SD_UDC_05_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;
  
 	localparam[63:0] SUPPLY_PERIOD = 20e6/50e3;
	localparam SUPPLY_WIDTH = $clog2(SUPPLY_PERIOD);
	localparam SUPPLY_RATE = 4;
	reg CLK_supply;
	reg[SUPPLY_WIDTH-1:0] supply_counter;
	reg[SUPPLY_WIDTH+SUPPLY_RATE-1:0] supply_duty;
	always @(posedge XTAL_20MHz_i) begin
		supply_counter <= supply_counter + 1'b1;
		CLK_supply <= supply_duty[SUPPLY_RATE +: SUPPLY_WIDTH] > supply_counter;

		if(supply_counter >= SUPPLY_PERIOD - 1'b1) begin
			supply_counter <= {SUPPLY_WIDTH{1'b0}};

		if(supply_duty[SUPPLY_RATE +: SUPPLY_WIDTH] < (SUPPLY_PERIOD>>1))
			supply_duty <= supply_duty + 1'b1;
		end
	end

	BB BB_PWM_DCDC_UDC(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_UDC_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_PWM_DCDC_TRDS(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_TRDS_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_PWM_DCDC_ISO(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_ISO_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_PWM_DCDC_DRV(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_DRV_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 

	BB BB_FLT_FPGA0(.I(1'b0), .T(1'b1), .O(FLT_bus[0]), .B(FPGA_io[`FLT_H_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA1(.I(1'b0), .T(1'b1), .O(FLT_bus[1]), .B(FPGA_io[`FLT_L_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA2(.I(1'b0), .T(1'b1), .O(FLT_bus[2]), .B(FPGA_io[`FLT_H_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA3(.I(1'b0), .T(1'b1), .O(FLT_bus[3]), .B(FPGA_io[`FLT_L_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA4(.I(1'b0), .T(1'b1), .O(FLT_bus[4]), .B(FPGA_io[`FLT_H_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA5(.I(1'b0), .T(1'b1), .O(FLT_bus[5]), .B(FPGA_io[`FLT_L_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA6(.I(1'b0), .T(1'b1), .O(FLT_bus[6]), .B(FPGA_io[`FLT_H_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA7(.I(1'b0), .T(1'b1), .O(FLT_bus[7]), .B(FPGA_io[`FLT_L_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	BB BB_FLT_FPGA8(.I(!rst_faults), .T(1'b1), .O(FLT_bus[8]), .B(FPGA_io[`RDY_H_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA9(.I(!rst_faults), .T(1'b1), .O(FLT_bus[9]), .B(FPGA_io[`RDY_L_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA10(.I(!rst_faults), .T(1'b1), .O(FLT_bus[10]), .B(FPGA_io[`RDY_H_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA11(.I(!rst_faults), .T(1'b1), .O(FLT_bus[11]), .B(FPGA_io[`RDY_L_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA12(.I(!rst_faults), .T(1'b1), .O(FLT_bus[12]), .B(FPGA_io[`RDY_H_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA13(.I(!rst_faults), .T(1'b1), .O(FLT_bus[13]), .B(FPGA_io[`RDY_L_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA14(.I(!rst_faults), .T(1'b1), .O(FLT_bus[14]), .B(FPGA_io[`RDY_H_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA15(.I(!rst_faults), .T(1'b1), .O(FLT_bus[15]), .B(FPGA_io[`RDY_L_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" OPENDRAIN="ON" */; 
 
	wire[7:0] PWM_o; 
	BB BB_PWM_FPGA0(.I(PWM_o[0]), .T(1'b0), .O(), .B(FPGA_io[`PWM_H_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA1(.I(PWM_o[1]), .T(1'b0), .O(), .B(FPGA_io[`PWM_L_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA2(.I(PWM_o[2]), .T(1'b0), .O(), .B(FPGA_io[`PWM_H_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA3(.I(PWM_o[3]), .T(1'b0), .O(), .B(FPGA_io[`PWM_L_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA4(.I(PWM_o[4]), .T(1'b0), .O(), .B(FPGA_io[`PWM_H_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA5(.I(PWM_o[5]), .T(1'b0), .O(), .B(FPGA_io[`PWM_L_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA6(.I(PWM_o[6]), .T(1'b0), .O(), .B(FPGA_io[`PWM_H_N_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA7(.I(PWM_o[7]), .T(1'b0), .O(), .B(FPGA_io[`PWM_L_N_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire[1:0] TRIGGER; 
	reg[1:0] TRIGGER_r; 
	BB BB_TRIGGER0(.I(1'b0), .T(1'b1), .O(TRIGGER[0]), .B(CPU_io[`TRIGGER0_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_TRIGGER1(.I(1'b0), .T(1'b1), .O(TRIGGER[1]), .B(CPU_io[`TRIGGER1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	always @(posedge clk_1x) 
		TRIGGER_r <= TRIGGER; 
 
	wire PWM_EN; 
	wire TZ_EN; 
	BB BB_PWM_EN(.I(1'b0), .T(1'b1), .O(PWM_EN), .B(CPU_io[`PWM_EN_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_TZ_EN(.I(1'b0), .T(1'b1), .O(TZ_EN), .B(CPU_io[`TZ_EN_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	reg[1:0] sync_reg; 
	always @(posedge clk_5x) 
		sync_reg <= {sync_phase, sync_reg[1]}; 
 
 	assign TZ_CLR = !(TZ_EN & PWM_EN & TZ_FPGA);
	FD1P3DX PWM_EN_ff(.D(PWM_EN), .SP(sync_reg[1] ^ sync_reg[0]), .CK(clk_5x), .CD(TZ_CLR), .Q(PWM_EN_r)); 
 
	Symmetrical_PWM #(.DEADTIME(65)) Symmetrical_PWM[3:0](.clk_i(clk_5x), .enable_output_i(PWM_EN_r), .duty_i({EMIF_RX_reg[7], EMIF_RX_reg[6]}), .next_period_i(next_period), .current_period_i(current_period), .local_counter_i(local_counter), .sync_phase_i(sync_phase), .PWM_o(PWM_o));
  
/*
	wire PWM_dp; 
	wire[7:0] PWM_dp_dt; 
	double_pulse double_pulse(.clk_i(XTAL_20MHz_i), .start_i(PWM_EN_r), .length0_i(EMIF_RX_reg[8][15:0]), .length1_i(EMIF_RX_reg[8][31:16]), .PWM_o(PWM_dp)); 
	deadtime deadtime(.clk_i(XTAL_20MHz_i), .deadtime_i(5'd5), .PWM_i({4{PWM_dp}}), .PWM_o(PWM_dp_dt)); 
	assign PWM_o = PWM_EN_r ? {PWM_dp_dt[0], PWM_dp_dt[1], 6'b0} : 8'b0;  
*/

	wire[8:0] REL_i; 
	wire[8:0] REL_o; 
	assign REL_o = TZ_EN ? REL_i : 9'b0; 
	BB BB_REL_CPU0(.I(1'b0), .T(1'b1), .O(REL_i[0]), .B(CPU_io[`C_SS_RLY_L1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU1(.I(1'b0), .T(1'b1), .O(REL_i[1]), .B(CPU_io[`GR_RLY_L1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU2(.I(1'b0), .T(1'b1), .O(REL_i[2]), .B(CPU_io[`C_SS_RLY_L2_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU3(.I(1'b0), .T(1'b1), .O(REL_i[3]), .B(CPU_io[`GR_RLY_L2_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU4(.I(1'b0), .T(1'b1), .O(REL_i[4]), .B(CPU_io[`C_SS_RLY_L3_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU5(.I(1'b0), .T(1'b1), .O(REL_i[5]), .B(CPU_io[`GR_RLY_L3_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	//BB BB_REL_CPU6(.I(1'b0), .T(1'b1), .O(REL_i[6]), .B(CPU_io[`DClink_DSCH_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU7(.I(1'b0), .T(1'b1), .O(REL_i[7]), .B(CPU_io[`GR_RLY_N_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU8(.I(1'b0), .T(1'b1), .O(REL_i[8]), .B(CPU_io[`SS_DClink_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	BB BB_REL_FPGA0(.I(REL_o[0]), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA1(.I(REL_o[1]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA2(.I(REL_o[2]), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA3(.I(REL_o[3]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA4(.I(REL_o[4]), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA5(.I(REL_o[5]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_REL_FPGA6(.I(REL_o[6]), .T(1'b0), .O(), .B(FPGA_io[`DClink_DSCH_CM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA7(.I(REL_o[7]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_N_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA8(.I(REL_o[8]), .T(1'b0), .O(), .B(FPGA_io[`SS_DClink_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire[5:1] LED_BUS; 
	BB BB_LED_CPU1(.I(1'b0), .T(1'b1), .O(LED_BUS[1]), .B(CPU_io[`LED1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU2(.I(1'b0), .T(1'b1), .O(LED_BUS[2]), .B(CPU_io[`LED2_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU3(.I(1'b0), .T(1'b1), .O(LED_BUS[3]), .B(CPU_io[`LED3_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU4(.I(1'b0), .T(1'b1), .O(LED_BUS[4]), .B(CPU_io[`LED4_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU5(.I(1'b0), .T(1'b1), .O(LED_BUS[5]), .B(CPU_io[`LED5_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	BB BB_LED_FPGA1(.I(LED_BUS[1]), .T(1'b0), .O(), .B(FPGA_io[`LED1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA2(.I(LED_BUS[2]), .T(1'b0), .O(), .B(FPGA_io[`LED2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA3(.I(LED_BUS[3]), .T(1'b0), .O(), .B(FPGA_io[`LED3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA4(.I(sync_ok[0]), .T(1'b0), .O(), .B(FPGA_io[`LED4_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA5(.I(LED_BUS[5]), .T(1'b0), .O(), .B(FPGA_io[`LED5_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 

	BB BB_SED_ENABLE_FPGA(.I(1'b0), .T(1'b1), .O(SED_enable), .B(CPU_io[`FPGA_SED]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 

	wire on_off; 
	BB BB_ON_OFF_FPGA(.I(1'b0), .T(1'b1), .O(on_off), .B(FPGA_io[`ON_OFF_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_ON_OFF_CPU(.I(on_off), .T(1'b0), .O(), .B(CPU_io[`ON_OFF_CM]))/*synthesis IO_TYPE="LVCMOS33" */; 

	wire Modbus_EN; 
	wire Modbus_TX; 
	wire Modbus_RX; 

	BB BB_Modbus_EN_CPU(.I(1'b0), .T(1'b1), .O(Modbus_EN), .B(CPU_io[`EN_Mod_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_Modbus_EN_FPGA(.I(Modbus_EN), .T(1'b0), .O(), .B(FPGA_io[`EN_Mod_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 

	BB BB_Modbus_TX_CPU(.I(1'b0), .T(1'b1), .O(Modbus_TX), .B(CPU_io[`TX_Mod_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_Modbus_TX_FPGA(.I(Modbus_TX), .T(1'b0), .O(), .B(FPGA_io[`TX_Mod_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_Modbus_RX_CPU(.I(Modbus_RX), .T(1'b0), .O(), .B(CPU_io[`RX_Mod_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_Modbus_RX_FPGA(.I(1'b0), .T(1'b1), .O(Modbus_RX), .B(FPGA_io[`RX_Mod_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
 
	BB BB_COMM_TX0(.I(tx_o[0]), .T(1'b0), .O(), .B(FPGA_io[`TX1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_COMM_TX1(.I(tx_o[1]), .T(1'b0), .O(), .B(FPGA_io[`TX2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_COMM_RX0(.I(1'b0), .T(1'b1), .O(rx_i[0]), .B(FPGA_io[`RX1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_COMM_RX1(.I(1'b0), .T(1'b1), .O(rx_i[1]), .B(FPGA_io[`RX2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
endmodule 
