`include "global.v" 
 
 //CPU IO SLAVE
`define TZ_EN_CS  151
`define PWM_EN_CS  154
`define FAN_PWM1_CS  12
`define FAN_PWM2_CS  13

`define SD_NEW_CS  156
`define SYNC_PWM_CS  158

`define TRIGGER0_CS  0
`define TRIGGER1_CS  1

`define RST_CS  159

`define RX1_MSG0_CS  16
`define RX1_MSG1_CS  17
`define RX1_MSG2_CS  18

`define RX2_MSG0_CS  21
`define RX2_MSG1_CS  22
`define SYNC_RDY_CS  23

`define FLT_DRV_CS  11

`define SS_DClink_CS  160
`define DClink_DSCH_CS  15

`define C_SS_RLY_L1_CS  161
`define GR_RLY_L1_CS  162
`define C_SS_RLY_L2_CS  163
`define GR_RLY_L2_CS  164
`define C_SS_RLY_L3_CS  165
`define GR_RLY_L3_CS  166
`define C_SS_RLY_N_CS  167
`define GR_RLY_N_CS  168

`define ON_OFF_CS  5

`define LED1_CS  6
`define LED2_CS  7
`define LED3_CS  8
`define LED4_CS  9
`define LED5_CS  14

//FPGA IO SLAVE
`define PWM_DCDC_UDC_FS  77
`define SD_UDC_FS  87
`define SD_CLK_UDC_FS  83
`define SD_UDC_05_FS  90
`define SD_CLK_UDC_05_FS  89

`define RST_L_N_FS  23
`define RST_H_N_FS  5
`define SD_I_AC_N_FS  63
`define SD_CLK_N_FS  65
`define C_SS_RLY_N_FS  75
`define GR_RLY_N_FS  69
`define PWM_DCDC_N_FS  41
`define PWM_L_N_FS  39
`define PWM_H_N_FS  21
`define FLT_L_N_FS  33
`define FLT_H_N_FS  15
`define RDY_L_N_FS  29
`define RDY_H_N_FS  11
`define Temp_N_FS  3

`define RST_L_L1_FS  18
`define RST_H_L1_FS  40
`define SD_I_AC_L1_FS  45
`define SD_CLK_L1_FS  51
`define C_SS_RLY_L1_FS  57
`define GR_RLY_L1_FS  53
`define PWM_DCDC_L1_FS  4
`define PWM_L_L1_FS  6
`define PWM_H_L1_FS  28
`define FLT_L_L1_FS  12
`define FLT_H_L1_FS  30
`define RDY_L_L1_FS  16
`define RDY_H_L1_FS  34
`define Temp_L1_FS  42
`define ExTemp_L1_FS  24

`define RST_L_L2_FS  99
`define RST_H_L2_FS  100
`define SD_I_AC_L2_FS  137
`define SD_CLK_L2_FS  119
`define C_SS_RLY_L2_FS  131
`define GR_RLY_L2_FS  129
`define PWM_DCDC_L2_FS  117
`define PWM_L_L2_FS  111
`define PWM_H_L2_FS  93
`define FLT_L_L2_FS  107
`define FLT_H_L2_FS  94
`define RDY_L_L2_FS  101
`define RDY_H_L2_FS  96
`define Temp_L2_FS  102
`define ExTemp_L2_FS  95

`define RST_L_L3_FS  136
`define RST_H_L3_FS  156
`define SD_I_AC_L3_FS  112
`define SD_CLK_L3_FS  106
`define C_SS_RLY_L3_FS  114
`define GR_RLY_L3_FS  118
`define PWM_DCDC_L3_FS  120
`define PWM_L_L3_FS  124
`define PWM_H_L3_FS  144
`define FLT_L_L3_FS  130
`define FLT_H_L3_FS  148
`define RDY_L_L3_FS  132
`define RDY_H_L3_FS  154
`define Temp_L3_FS  162
`define ExTemp_L3_FS  142

`define SS_DClink_FS  174
`define DClink_DSCH_FS  178

`define FAN_PWM1_FS  168
`define FAN_PWM2_FS  166

`define DIP_1_FS  46
`define DIP_2_FS  52
`define DIP_3_FS  54
`define DIP_4_FS  60
`define DIP_5_FS  66
`define DIP_6_FS  72
`define DIP_7_FS  78
`define DIP_8_FS  84

`define LED1_FS  88
`define LED2_FS  123
`define LED3_FS  179
`define LED4_FS  149
`define LED5_FS  171

`define ON_OFF_FS  161

`define FLTSPLY_FS  180

`define RX1_FS 153
`define TX1_FS 141
`define RX2_FS 177
`define TX2_FS 167

module SerDes_slave(XTAL_20MHz_i, XTAL_25MHz_i, CPU_io, FPGA_conn_io, clk_cpu_o);
	input XTAL_25MHz_i/*synthesis IO_TYPE="LVCMOS33" */; 
	input XTAL_20MHz_i/*synthesis IO_TYPE="LVCMOS33" */; 
	inout[168:0] CPU_io/*synthesis IO_TYPE="LVCMOS33" */; 
	inout[180:1] FPGA_conn_io/*synthesis IO_TYPE="LVCMOS33" */;
	output clk_cpu_o/*synthesis IO_TYPE="LVCMOS33" */; 
 
	integer i; 
 
	assign clk_cpu_o = XTAL_20MHz_i;
 
	GSR GSR_INST (.GSR (1'b1)); 
	//PUR PUR_INST (.PUR (1'b1)); 
///////////////////////////////////////////////////////////////////// 
	localparam EMIF_MEMORY_WIDTH = `COMM_MEMORY_EMIF_WIDTH+2;//$clog2(2x COMM_MEMORY + MUX) = $clog2(3) = 2 
	wire EMIF_oe_i; 
	wire EMIF_we_i; 
	wire[31:0] EMIF_data_i; 
	wire[31:0] EMIF_data_o; 
	wire[EMIF_MEMORY_WIDTH-1:0] EMIF_address_i; 
 
	localparam EMIF_MUX_NUMBER = 23;
	localparam EMIF_REG_NUMBER = 11;
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
	RX_port	RX1_port(.rx_clk_i(rx1_clk_10x), .rx_i(rx_i[0]), .rx_clk_phasestep_lag_o(rx1_clk_phasestep_lag), .timestamp_code_o(timestamp_code_rx1), .rx_rdy_o(rx1_port_rdy),
	.fifo_clk_o(fifo_rx1_clk_5x), .fifo_we_o(fifo_rx1_port_we), .fifo_o(fifo_rx1_port),
	.clk_i(clk_1x), .rst_i(!LOCK)); 

	wire timestamp_code_rx2;
	wire[8:0] fifo_rx2_port; 
	wire[1:0] fifo_rx2_port_we; 
	wire fifo_rx2_clk_5x; 
	wire rx2_port_rdy; 
	RX_port	RX2_port(.rx_clk_i(rx2_clk_10x), .rx_i(rx_i[1]), .rx_clk_phasestep_lag_o(rx2_clk_phasestep_lag), .timestamp_code_o(timestamp_code_rx2), .rx_rdy_o(rx2_port_rdy),
	.fifo_clk_o(fifo_rx2_clk_5x), .fifo_we_o(fifo_rx2_port_we), .fifo_o(fifo_rx2_port),
	.clk_i(clk_1x), .rst_i(!LOCK)); 

	wire timestamp_code_tx1;
	wire[8:0] fifo_tx1_core; 
	wire[1:0] fifo_tx1_core_we; 
	wire fifo_tx1_clk_1x; 
	TX_port	TX1_port(.tx_clk_i(tx_clk_5x), .tx_o(tx_o[0]), .timestamp_code_o(timestamp_code_tx1), 
	.fifo1_clk_i(fifo_rx2_clk_5x), .fifo1_we_i(fifo_rx2_port_we), .fifo1_i(fifo_rx2_port), 
	.fifo2_clk_i(fifo_tx1_clk_1x), .fifo2_we_i(fifo_tx1_core_we), .fifo2_i(fifo_tx1_core)); 

	wire timestamp_code_tx2;
	wire[8:0] fifo_tx2_core; 
	wire[1:0] fifo_tx2_core_we; 
	wire fifo_tx2_clk_1x; 
	TX_port	TX2_port(.tx_clk_i(tx_clk_5x), .tx_o(tx_o[1]), .timestamp_code_o(timestamp_code_tx2), 
	.fifo1_clk_i(fifo_rx1_clk_5x), .fifo1_we_i(fifo_rx1_port_we), .fifo1_i(fifo_rx1_port), 
	.fifo2_clk_i(fifo_tx2_clk_1x), .fifo2_we_i(fifo_tx2_core_we), .fifo2_i(fifo_tx2_core)); 

///////////////////////////////////////////////////////////////////// 
  
	wire hipri_comm_enable;
	wire lopri_comm_enable;
	
	wire[`POINTER_WIDTH-1:0] Comm_rx1_addrw; 
	wire[7:0] Comm_rx1_dataw; 
	wire Comm_rx1_we; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_ack; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_wip; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_rdy;
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_ack; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_rdy; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_wip; 
	wire[8:0] rx1_fifo_rx_o; 
	wire rx1_fifo_rx_dv; 
	wire[2:0] rx1_state_reg;

	wire rx1_crc_error; 
	wire rx1_overrun_error; 
	wire rx1_frame_error;  
	RX_core RX1_core(.core_clk_i(clk_1x), .hipri_msg_en_i(hipri_comm_enable), .lopri_msg_en_i(lopri_comm_enable),
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
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_wip; 
	wire[`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_rdy; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_ack; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_rdy; 
	wire[`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_wip;
	
	wire rx2_crc_error; 
	wire rx2_overrun_error; 
	wire rx2_frame_error; 
	RX_core RX2_core(.core_clk_i(clk_1x), .hipri_msg_en_i(hipri_comm_enable), .lopri_msg_en_i(lopri_comm_enable),
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
	TX_core TX1_core(.core_clk_i(clk_1x), .hipri_msg_en_i(hipri_comm_enable), .lopri_msg_en_i(lopri_comm_enable),
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
	TX_core TX2_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b0),
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
	) COMM_TX_MEM1 (.WrAddress(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .WrClock(EMIF_we_i), .WrClockEn(EMIF_address_i[EMIF_MEMORY_WIDTH-1]), .WE(!EMIF_address_i[EMIF_MEMORY_WIDTH-2]), .Data(EMIF_data_i), 
    .RdAddress(Comm_tx1_addrr), .RdClock(clk_1x), .RdClockEn(1'b1), .Reset(1'b0), .Q(Comm_tx1_mem_datar));  
 
	assign COMM_RX_MEM2_data_o = 32'b0;
	assign Comm_tx2_mem_datar = 8'b0; 
/*
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

	pmi_ram_dp
	#(.pmi_wr_addr_depth(512),
	.pmi_wr_addr_width(9),
	.pmi_wr_data_width(32),
	.pmi_rd_addr_depth(2048),
	.pmi_rd_addr_width(11),
	.pmi_rd_data_width(8),
	.pmi_regmode("noreg"),     //"reg", "noreg"
	.pmi_gsr("enable"),		   //"enable", "disable"
	.pmi_resetmode("sync"),    //"async", "sync"
	.pmi_optimization("speed"),//"speed", "area"
	.pmi_family("ECP5U")
	) COMM_TX_MEM2 (.WrAddress(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .WrClock(EMIF_we_i), .WrClockEn(EMIF_address_i[EMIF_MEMORY_WIDTH-1]), .WE(EMIF_address_i[EMIF_MEMORY_WIDTH-2]), .Data(EMIF_data_i),
    .RdAddress(Comm_tx2_addrr), .RdClock(clk_1x), .RdClockEn(1'b1), .Reset(1'b0), .Q(Comm_tx2_mem_datar));
*/ 

///////////////////////////////////////////////////////////////////// 
 
	localparam OSR = 320; 
	localparam SD_WIDTH = 16; 
	wire[1:0] decimator_pulse; 
	wire new_value; 
	wire new_value_early; 
	reg SD_sync_pulse; 
	wire select_SD; 
	SD_filter_sync_gen #(.OSR(OSR)) SD_filter_sync_gen1(.clk_i(XTAL_20MHz_i), .sync_pulse_i(SD_sync_pulse), 
	.decimator_pulse_o(decimator_pulse), .select_o(select_SD), .new_value_o(new_value), .new_value_early_o(new_value_early)); 
 
	wire[5:0] SD_DAT; 
	wire[5:0] SD_DAT_IDDR; 
	wire[5:0] SD_CLK; 
	ODDRX1F SD_CLK_gen[5:0] (.D0(1'b0), .D1(1'b1), .SCLK(XTAL_20MHz_i), .RST(1'b0), .Q(SD_CLK));
    
	IDDRX1F SD_IDDR[5:0](.D(SD_DAT), .RST(1'b0), .SCLK(XTAL_20MHz_i), .Q0(SD_DAT_IDDR), .Q1());
 
	wire[SD_WIDTH*6-1:0] SD_dat; 
	SD_filter_sync #(.ORDER(2), .OSR(OSR), .OUTPUT_WIDTH(SD_WIDTH)) SD_filter_sync[5:0](.data_i(SD_DAT_IDDR), 
	.clk_i(XTAL_20MHz_i), .decimator_pulse_i(decimator_pulse), .select_i(select_SD), .data_o(SD_dat)); 
 
	wire[6:0] DRV_temp; 
	wire[6:0] DRV_temp_IDDR; 
    IDDRX1F SD_temp_IDDR[6:0](.D(DRV_temp), .RST(1'b0), .SCLK(XTAL_20MHz_i), .Q0(DRV_temp_IDDR), .Q1());
 
	wire[SD_WIDTH*7-1:0] SD_DRV_temp; 
	reg decimator_pulse_DRV; 
	SD_filter #(.ORDER(1), .OSR(62500), .OUTPUT_WIDTH(SD_WIDTH), .OUTPUT_NUMBER(1)) SD_filter[6:0](.data_i(DRV_temp_IDDR), 
	.clk_i(XTAL_20MHz_i), .decimator_pulse_i(decimator_pulse_DRV), .data_o(SD_DRV_temp)); 
 
	reg[SD_WIDTH*7-1:0] SD_DRV_temp_reg; 
	reg[15:0] OSR_counter; 
	always @(posedge XTAL_20MHz_i) begin 
		if(new_value) 
			SD_DRV_temp_reg <= SD_DRV_temp; 
 
		if(OSR_counter >= 16'd62499) begin 
			OSR_counter <= 0; 
			decimator_pulse_DRV <= 1'b1; 
		end 
		else begin 
			OSR_counter <= OSR_counter + 1'b1; 
			decimator_pulse_DRV <= 1'b0; 
		end 
	end 
 
	initial begin 
		decimator_pulse_DRV = 0; 
		SD_DRV_temp_reg = 0; 
		OSR_counter = 0; 
	end 
 
///////////////////////////////////////////////////////////////////// 
 	
	wire SED_enable;
	SED_machine SED_machine(.clk_i(XTAL_20MHz_i), .enable_i(SED_enable), .err_o(sed_err));
	
	wire fault_supply;
	reg fault_supply_r;
	reg fault_supply_f;
	reg[7:0] fault_supply_counter;
	always @(posedge XTAL_20MHz_i) begin
		fault_supply_r <= fault_supply;
		if(!fault_supply_r)
			fault_supply_counter <= fault_supply_counter + 1'b1;
		else
			fault_supply_counter <= 0;
		fault_supply_f <= fault_supply_counter == 8'hFF;
	end
	
	localparam FAULT_NUMBER = 26; 
 
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
	assign FLT_bus[25] = !fault_supply_f; 
 
 	wire rst_faults;
 	FD1P3BX FLT_ff[FAULT_NUMBER-1:0](.D(1'b0), .SP(rst_faults), .CK(clk_5x), .PD(~FLT_bus[FAULT_NUMBER-1:0]), .Q(FLT_REG_O[FAULT_NUMBER-1:0])); 
 
/////////////////////////////////////////////////////////////////////  
 	
	defparam TX1_port.TIMESTAMP_CODE = `K_Timestamp_slave;
	defparam TX2_port.TIMESTAMP_CODE = `K_Timestamp_master;
	defparam RX1_port.TIMESTAMP_CODE = `K_Timestamp_master;
	defparam RX2_port.TIMESTAMP_CODE = `K_Timestamp_slave;	
	
	wire[63:0] snapshot_value; 
	wire[15:0] next_period; 
	wire[15:0] current_period; 
	wire[15:0] local_counter;
	wire sync_phase;
	Local_counter #(.INITIAL_PHASE(0)) Local_counter(.clk_i(clk_5x), .next_period_i(next_period), .current_period_o(current_period), .local_counter_o(local_counter), .sync_phase_o(sync_phase),
	.snapshot_start_i({timestamp_code_rx2, timestamp_code_tx2, timestamp_code_rx1, timestamp_code_tx1}), .snapshot_value_o(snapshot_value));  
	
	always @(posedge clk_5x)
		SD_sync_pulse <= local_counter >= EMIF_RX_reg[7][15:0];
	 
	wire[`HIPRI_MAILBOXES_WIDTH-1:0] node_number;
	wire node_number_rdy; 
	Node_number_eval Node_number_eval(.clk_1x_i(clk_1x), .fifo_clk_i(fifo_rx1_clk_5x), .fifo_we_i(fifo_rx1_port_we), .fifo_i(fifo_rx1_port), .node_number_o(node_number), .node_number_rdy_o(node_number_rdy)); 
 
	wire[31:0] Kalman_offset; 
	wire[31:0] Kalman_rate;  
	wire[15:0] offset_memory;  
	wire sync_rdy; 
	Slave_sync Slave_sync(.clk_i(clk_1x), .sync_phase_i(sync_phase), .msg_rdy_i(rx1_hipri_msg_rdy[1]),
	.rx_addrw_i(Comm_rx1_addrw), .rx_dataw_i(Comm_rx1_dataw), .rx_we_i(Comm_rx1_we), 
	.node_number_i(node_number), .node_number_rdy_i(node_number_rdy),
	.Kalman_offset_o(Kalman_offset), .Kalman_rate_o(Kalman_rate), .offset_memory_o(offset_memory),
	.next_period_o(next_period), .sync_rdy_o(sync_rdy)); 
	
	assign rx1_hipri_msg_ack = {EMIF_RX_reg[1][1*8+2 +: `HIPRI_MAILBOXES_NUMBER-2], rx1_hipri_msg_rdy[1], EMIF_RX_reg[1][8]}; 
	assign rx2_hipri_msg_ack = EMIF_RX_reg[1][3*8 +: `HIPRI_MAILBOXES_NUMBER]; 

	assign rx1_lopri_msg_ack = EMIF_RX_reg[1][0*8 +: `LOPRI_MAILBOXES_NUMBER]; 
	assign rx2_lopri_msg_ack = EMIF_RX_reg[1][2*8 +: `LOPRI_MAILBOXES_NUMBER]; 
	
	assign tx1_hipri_msg_start = {EMIF_RX_reg[0][1*8+1 +: `HIPRI_MAILBOXES_NUMBER-1], rx1_hipri_msg_wip[0]}; 
	assign tx2_hipri_msg_start = EMIF_RX_reg[0][3*8 +: `HIPRI_MAILBOXES_NUMBER]; 

	assign tx1_lopri_msg_start = EMIF_RX_reg[0][0*8 +: `LOPRI_MAILBOXES_NUMBER]; 
	assign tx2_lopri_msg_start = EMIF_RX_reg[0][2*8 +: `LOPRI_MAILBOXES_NUMBER]; 
	
 	defparam TX1_core.TX_CODE = `K_Timestamp_slave;
	defparam TX2_core.TX_CODE = `K_Enum_nodes;
	assign tx1_code_start = timestamp_code_rx1; 
	assign tx2_code_start = timestamp_code_rx1; 

	assign hipri_comm_enable = EMIF_RX_reg[5][0];//register is cleared if node_number_rdy goes low
	assign lopri_comm_enable = node_number_rdy;
	
	wire scope_trigger_request;
	assign scope_trigger_request = EMIF_RX_reg[6][0];
	
	reg tx1_hipri_msg_wip0_last; 
	reg tx1_hipri0_end;
	reg tx1_hipri0_start;
	wire PWM_EN_r;  
	always @(posedge clk_1x) begin
		tx1_hipri_msg_wip0_last <= tx1_hipri_msg_wip[0];
		if(tx1_hipri_msg_wip[0] && !tx1_hipri_msg_wip0_last && EMIF_RX_reg[6][0]) tx1_hipri0_start <= 1'b1;
		if(!tx1_hipri_msg_wip[0] && tx1_hipri_msg_wip0_last) begin
			tx1_hipri0_start <= 1'b0;
			if(tx1_hipri0_start) tx1_hipri0_end <= 1'b1;
		end
		else tx1_hipri0_end <= 1'b0;
		
			
		tx1_select_memory <= 1'b1;
		tx2_select_memory <= 1'b1;
		case(Comm_tx1_addrr)
			11'h400 : Comm_tx1_mux_datar_r <= {4'h0, {4-`HIPRI_MAILBOXES_WIDTH{1'b0}}, node_number};
			11'h401 : Comm_tx1_mux_datar_r <= 8'd13 - 8'd1;
			11'h402 : Comm_tx1_mux_datar_r <= {5'b0, scope_trigger_request, PWM_EN_r, sync_rdy}; 
			11'h403 : Comm_tx1_mux_datar_r <= 8'b0; 
			11'h404 : Comm_tx1_mux_datar_r <= snapshot_value[0*8 +: 8];
			11'h405 : Comm_tx1_mux_datar_r <= snapshot_value[1*8 +: 8];
			11'h406 : Comm_tx1_mux_datar_r <= snapshot_value[2*8 +: 8];
			11'h407 : Comm_tx1_mux_datar_r <= snapshot_value[3*8 +: 8];
			11'h408 : Comm_tx1_mux_datar_r <= snapshot_value[4*8 +: 8];
			11'h409 : Comm_tx1_mux_datar_r <= snapshot_value[5*8 +: 8];
			11'h40A : Comm_tx1_mux_datar_r <= snapshot_value[6*8 +: 8];
			11'h40B : Comm_tx1_mux_datar_r <= snapshot_value[7*8 +: 8];
			11'h40C : Comm_tx1_mux_datar_r <= SD_dat[0*8 +: 8];
			11'h40D : Comm_tx1_mux_datar_r <= SD_dat[1*8 +: 8];
			11'h40E : Comm_tx1_mux_datar_r <= SD_dat[2*8 +: 8];
			11'h40F : Comm_tx1_mux_datar_r <= SD_dat[3*8 +: 8];
			11'h410 : Comm_tx1_mux_datar_r <= SD_dat[4*8 +: 8];
			11'h411 : Comm_tx1_mux_datar_r <= SD_dat[5*8 +: 8]; 
			11'h412 : Comm_tx1_mux_datar_r <= SD_dat[6*8 +: 8];
			11'h413 : Comm_tx1_mux_datar_r <= SD_dat[7*8 +: 8];
			11'h414 : Comm_tx1_mux_datar_r <= SD_dat[8*8 +: 8];
			11'h415 : Comm_tx1_mux_datar_r <= SD_dat[9*8 +: 8];
			11'h416 : Comm_tx1_mux_datar_r <= SD_dat[10*8 +: 8];
			11'h417 : Comm_tx1_mux_datar_r <= SD_dat[11*8 +: 8]; 
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
	localparam DEB_WADDR_WIDTH = 13;
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
    .RdAddress(EMIF_RX_reg[8][DEB_RADDR_WIDTH-1:0]), .RdClock(!EMIF_oe_i), .RdClockEn(1'b1), .Reset(1'b0), .Q(Scope_data_out)); 

	reg[31:0] timestamp_counter;
	reg[31:0] timestamp_mem;
	wire[31:0] timestamp_diff;
	
	assign timestamp_diff = timestamp_counter - timestamp_mem;
	always @(posedge clk_1x) begin
		timestamp_counter <= timestamp_counter + 1'b1;
		if(Scope_enable_extended) timestamp_mem <= timestamp_counter;
	end
	
	assign Scope_acquire_before_trigger = EMIF_RX_reg[9][DEB_WADDR_WIDTH-1:0];
	assign Scope_enable = rx1_lopri_msg_rdy[0] | (|rx1_lopri_msg_wip) | fifo_tx1_core_we[1] | (rx1_fifo_rx_o == `K_Start_Lopri_Packet);
	assign Scope_data_in = {rx1_state_reg, rx1_lopri_msg_rdy[3:0], rx1_lopri_msg_wip[3:0], rx1_fifo_rx_o, rx1_fifo_rx_dv, tx1_lopri_msg_wip[3:0], fifo_tx1_core, fifo_tx1_core_we, Scope_trigger_r, timestamp_diff};
	assign Scope_trigger = EMIF_RX_reg[10][0];
	
///////////////////////////////////////////////////////////////////// 

	assign EMIF_data_o = EMIF_address_i[EMIF_MEMORY_WIDTH-1] ? (EMIF_address_i[EMIF_MEMORY_WIDTH-2] ? COMM_RX_MEM2_data_o : COMM_RX_MEM1_data_o) : EMIF_TX_mux[EMIF_address_i[EMIF_MUX_WIDTH-1:0]]; 
 	 
	wire[8:1] DIP_SWITCH; 
	reg[15:0] next_period_r; 
	always @(posedge XTAL_20MHz_i)
		next_period_r <= next_period; 

	assign EMIF_TX_mux[0] = {tx2_hipri_msg_wip, tx2_lopri_msg_wip, tx1_hipri_msg_wip, tx1_lopri_msg_wip}; 
	assign EMIF_TX_mux[1] = {rx2_hipri_msg_rdy, rx2_lopri_msg_rdy, rx1_hipri_msg_rdy, rx1_lopri_msg_rdy};
 	assign EMIF_TX_mux[2] = SD_dat[0*32 +: 32]; 
	assign EMIF_TX_mux[3] = SD_dat[1*32 +: 32]; 
	assign EMIF_TX_mux[4] = SD_dat[2*32 +: 32];
	assign EMIF_TX_mux[5] = {16'b0, next_period_r};
	assign EMIF_TX_mux[6] = SD_DRV_temp_reg[0*32 +: 32]; 
	assign EMIF_TX_mux[7] = SD_DRV_temp_reg[1*32 +: 32]; 	
	assign EMIF_TX_mux[8] = SD_DRV_temp_reg[2*32 +: 32]; 	
	assign EMIF_TX_mux[9] = {16'b0, SD_DRV_temp_reg[3*32 +: 16]}; 	
	assign EMIF_TX_mux[10] = {{32-2-`HIPRI_MAILBOXES_WIDTH-FAULT_NUMBER{1'b0}}, node_number, node_number_rdy, hipri_comm_enable, FLT_REG_O}; 
	assign EMIF_TX_mux[11] = {24'b0, DIP_SWITCH}; 
	assign EMIF_TX_mux[12] = Kalman_offset;
	assign EMIF_TX_mux[13] = Kalman_rate; 
	assign EMIF_TX_mux[14] = offset_memory;
	assign EMIF_TX_mux[15] = EMIF_RX_reg[7];
	assign EMIF_TX_mux[16] = `VERSION;
	assign EMIF_TX_mux[17] = Scope_data_out[31:0];
	assign EMIF_TX_mux[18] = {28'b0, Scope_data_out[DEB_READ_WIDTH-1:32]};
	assign EMIF_TX_mux[19] = DEB_RADDR_DEPTH;
	assign EMIF_TX_mux[20] = DEB_WRITE_WIDTH_MULTIPLY;
	assign EMIF_TX_mux[21] = Scope_state[1];
	assign EMIF_TX_mux[22] = Scope_index_last;

	wire[31:0] tx_start_clear;
	wire[31:0] rx_ack_clear;
	assign tx_start_clear = {tx2_hipri_msg_wip, tx2_lopri_msg_wip, tx1_hipri_msg_wip, tx1_lopri_msg_wip} | {{8{!hipri_comm_enable}}, {8{!lopri_comm_enable}}, {8{!hipri_comm_enable}}, {8{!lopri_comm_enable}}};
	assign rx_ack_clear = ~{{rx2_hipri_msg_rdy, rx2_lopri_msg_rdy, rx1_hipri_msg_rdy, rx1_lopri_msg_rdy} & {{8{hipri_comm_enable}}, {8{lopri_comm_enable}}, {8{hipri_comm_enable}}, {8{lopri_comm_enable}}}};
 	FD1P3DX EMIF_RX_reg_0[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 0), .CK(EMIF_we_i), .CD(tx_start_clear), .Q(EMIF_RX_reg[0]));
 	FD1P3DX EMIF_RX_reg_1[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 1), .CK(EMIF_we_i), .CD(rx_ack_clear), .Q(EMIF_RX_reg[1]));
  	FD1P3DX EMIF_RX_reg_2[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 2), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg[2])); 
 	FD1P3DX EMIF_RX_reg_3[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 3), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg[3])); 
 	FD1P3DX EMIF_RX_reg_4[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 4), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg[4]));
 	FD1P3DX EMIF_RX_reg_5[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 5), .CK(EMIF_we_i), .CD({31'b0, !node_number_rdy}), .Q(EMIF_RX_reg[5]));
 	FD1P3DX EMIF_RX_reg_6[31:0](.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 6), .CK(EMIF_we_i), .CD({31'b0, tx1_hipri0_end}), .Q(EMIF_RX_reg[6]));
	EMIF_RX_reg #(.INIT_VAL(32'd1625)) EMIF_RX_reg_7(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 7), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[7]));
	EMIF_RX_reg #(.INIT_VAL(32'd0)) EMIF_RX_reg_8(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 8), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[8]));
	EMIF_RX_reg #(.INIT_VAL(DEB_WADDR_DEPTH>>1)) EMIF_RX_reg_9(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 9), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[9]));
	EMIF_RX_reg #(.INIT_VAL(32'd0)) EMIF_RX_reg_10(.D(EMIF_data_i), .SP(!EMIF_address_i[EMIF_MEMORY_WIDTH-1] && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 10), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg[10]));

///////////////////////////////////////////////////////////////////// 

	BB BB_EMIF_OE(.I(1'b0), .T(1'b1), .O(EMIF_oe_i), .B(CPU_io[`EM1OE]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/; 
	BB BB_EMIF_WE(.I(1'b0), .T(1'b1), .O(EMIF_we_i), .B(CPU_io[`EM1WE]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/; 
	BB BB_EMIF_CS(.I(1'b0), .T(1'b1), .O(EMIF_cs_i), .B(CPU_io[`EM1CS2]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/; 
 
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
	//BB BB_EMIF_address11(.I(1'b0), .T(1'b1), .O(EMIF_address_i[11]), .B(CPU_io[`EM1A11])) `EMIF_address_options; 
 
	BB BB_RX1_MSG0(.I(rx1_hipri_msg_rdy[0]), .T(1'b0), .O(), .B(CPU_io[`RX1_MSG0_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RX1_MSG1(.I(rx1_hipri_msg_rdy[1]), .T(1'b0), .O(), .B(CPU_io[`RX1_MSG1_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RX1_MSG3(.I(rx1_hipri_msg_rdy[2]), .T(1'b0), .O(), .B(CPU_io[`RX1_MSG2_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_RX2_MSG0(.I(rx2_hipri_msg_rdy[0]), .T(1'b0), .O(), .B(CPU_io[`RX2_MSG0_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RX2_MSG1(.I(rx2_hipri_msg_rdy[1]), .T(1'b0), .O(), .B(CPU_io[`RX2_MSG1_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_SYNC_RDY(.I(sync_rdy), .T(1'b0), .O(), .B(CPU_io[`SYNC_RDY_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_RST(.I(1'b0), .T(1'b1), .O(rst_faults), .B(CPU_io[`RST_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_NEW(.I(new_value), .T(1'b0), .O(), .B(CPU_io[`SD_NEW_CS]))/*synthesis IO_TYPE="LVCMOS33"*/; 
	BB BB_SYNC_PWM(.I(sync_phase), .T(1'b0), .O(), .B(CPU_io[`SYNC_PWM_CS]))/*synthesis IO_TYPE="LVCMOS33"*/;
 
	BB BB_SD_CLK0(.I(SD_CLK[0]), .T(1'b0), .O(), .B(FPGA_conn_io[`SD_CLK_UDC_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK1(.I(SD_CLK[1]), .T(1'b0), .O(), .B(FPGA_conn_io[`SD_CLK_UDC_05_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK2(.I(SD_CLK[2]), .T(1'b0), .O(), .B(FPGA_conn_io[`SD_CLK_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK3(.I(SD_CLK[3]), .T(1'b0), .O(), .B(FPGA_conn_io[`SD_CLK_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK4(.I(SD_CLK[4]), .T(1'b0), .O(), .B(FPGA_conn_io[`SD_CLK_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_SD_CLK5(.I(SD_CLK[5]), .T(1'b0), .O(), .B(FPGA_conn_io[`SD_CLK_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_SD_DAT0(.I(1'b0), .T(1'b1), .O(SD_DAT[0]), .B(FPGA_conn_io[`SD_UDC_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT1(.I(1'b0), .T(1'b1), .O(SD_DAT[1]), .B(FPGA_conn_io[`SD_UDC_05_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT2(.I(1'b0), .T(1'b1), .O(SD_DAT[2]), .B(FPGA_conn_io[`SD_I_AC_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT3(.I(1'b0), .T(1'b1), .O(SD_DAT[3]), .B(FPGA_conn_io[`SD_I_AC_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT4(.I(1'b0), .T(1'b1), .O(SD_DAT[4]), .B(FPGA_conn_io[`SD_I_AC_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT5(.I(1'b0), .T(1'b1), .O(SD_DAT[5]), .B(FPGA_conn_io[`SD_I_AC_N_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	BB BB_SD_DRV_temp0(.I(1'b0), .T(1'b1), .O(DRV_temp[0]), .B(FPGA_conn_io[`Temp_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DRV_temp1(.I(1'b0), .T(1'b1), .O(DRV_temp[1]), .B(FPGA_conn_io[`Temp_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DRV_temp2(.I(1'b0), .T(1'b1), .O(DRV_temp[2]), .B(FPGA_conn_io[`Temp_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DRV_temp3(.I(1'b0), .T(1'b1), .O(DRV_temp[3]), .B(FPGA_conn_io[`Temp_N_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DRV_temp4(.I(1'b0), .T(1'b1), .O(DRV_temp[4]), .B(FPGA_conn_io[`ExTemp_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DRV_temp5(.I(1'b0), .T(1'b1), .O(DRV_temp[5]), .B(FPGA_conn_io[`ExTemp_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DRV_temp6(.I(1'b0), .T(1'b1), .O(DRV_temp[6]), .B(FPGA_conn_io[`ExTemp_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
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
	
	BB BB_PWM_DCDC_UDC(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_DCDC_UDC_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_DCDC_L1(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_DCDC_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_DCDC_L2(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_DCDC_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_DCDC_L3(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_DCDC_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_DCDC_L4(.I(CLK_supply), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_DCDC_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_RST_FPGA0(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_H_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RST_FPGA1(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_L_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RST_FPGA2(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_H_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RST_FPGA3(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_L_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RST_FPGA4(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_H_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RST_FPGA5(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_L_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RST_FPGA6(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_H_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_RST_FPGA7(.I(!rst_faults), .T(1'b0), .O(), .B(FPGA_conn_io[`RST_L_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_FLT_FPGA0(.I(1'b0), .T(1'b1), .O(FLT_bus[0]), .B(FPGA_conn_io[`FLT_H_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA1(.I(1'b0), .T(1'b1), .O(FLT_bus[1]), .B(FPGA_conn_io[`FLT_L_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA2(.I(1'b0), .T(1'b1), .O(FLT_bus[2]), .B(FPGA_conn_io[`FLT_H_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA3(.I(1'b0), .T(1'b1), .O(FLT_bus[3]), .B(FPGA_conn_io[`FLT_L_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA4(.I(1'b0), .T(1'b1), .O(FLT_bus[4]), .B(FPGA_conn_io[`FLT_H_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA5(.I(1'b0), .T(1'b1), .O(FLT_bus[5]), .B(FPGA_conn_io[`FLT_L_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA6(.I(1'b0), .T(1'b1), .O(FLT_bus[6]), .B(FPGA_conn_io[`FLT_H_N_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA7(.I(1'b0), .T(1'b1), .O(FLT_bus[7]), .B(FPGA_conn_io[`FLT_L_N_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	BB BB_FLT_FPGA8(.I(1'b0), .T(1'b1), .O(FLT_bus[8]), .B(FPGA_conn_io[`RDY_H_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA9(.I(1'b0), .T(1'b1), .O(FLT_bus[9]), .B(FPGA_conn_io[`RDY_L_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA10(.I(1'b0), .T(1'b1), .O(FLT_bus[10]), .B(FPGA_conn_io[`RDY_H_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA11(.I(1'b0), .T(1'b1), .O(FLT_bus[11]), .B(FPGA_conn_io[`RDY_L_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA12(.I(1'b0), .T(1'b1), .O(FLT_bus[12]), .B(FPGA_conn_io[`RDY_H_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA13(.I(1'b0), .T(1'b1), .O(FLT_bus[13]), .B(FPGA_conn_io[`RDY_L_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA14(.I(1'b0), .T(1'b1), .O(FLT_bus[14]), .B(FPGA_conn_io[`RDY_H_N_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_FLT_FPGA15(.I(1'b0), .T(1'b1), .O(FLT_bus[15]), .B(FPGA_conn_io[`RDY_L_N_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	wire TZ_CPU; 
	assign TZ_CPU = &{FLT_bus[25:24], FLT_bus[22], FLT_bus[18:0]};//usuniete bledy rx2 
	BB BB_FLT_DRV_CPU(.I(TZ_CPU), .T(1'b0), .O(), .B(CPU_io[`FLT_DRV_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire[7:0] PWM_o; 
	BB BB_PWM_FPGA0(.I(PWM_o[0]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_H_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA1(.I(PWM_o[1]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_L_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA2(.I(PWM_o[2]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_H_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA3(.I(PWM_o[3]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_L_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA4(.I(PWM_o[4]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_H_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA5(.I(PWM_o[5]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_L_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA6(.I(PWM_o[6]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_H_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA7(.I(PWM_o[7]), .T(1'b0), .O(), .B(FPGA_conn_io[`PWM_L_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire[1:0] TRIGGER; 
	reg[1:0] TRIGGER_r; 
	BB BB_TRIGGER0(.I(1'b0), .T(1'b1), .O(TRIGGER[0]), .B(CPU_io[`TRIGGER0_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_TRIGGER1(.I(1'b0), .T(1'b1), .O(TRIGGER[1]), .B(CPU_io[`TRIGGER1_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	always @(posedge clk_1x) 
		TRIGGER_r <= TRIGGER; 
 
	wire PWM_EN; 
	wire TZ_EN; 
	BB BB_PWM_EN(.I(1'b0), .T(1'b1), .O(PWM_EN), .B(CPU_io[`PWM_EN_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_TZ_EN(.I(1'b0), .T(1'b1), .O(TZ_EN), .B(CPU_io[`TZ_EN_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	reg[1:0] sync_reg; 
	always @(posedge clk_5x) 
		sync_reg <= {sync_phase, sync_reg[1]}; 
 
	FD1P3DX PWM_EN_ff(.D(PWM_EN), .SP(sync_reg[1] ^ sync_reg[0]), .CK(clk_5x), .CD(!(TZ_EN & PWM_EN)), .Q(PWM_EN_r)); 
 
	Symmetrical_PWM #(.DEADTIME(65)) Symmetrical_PWM[3:0](.clk_i(clk_5x), .enable_output_i(PWM_EN_r), .duty_i({EMIF_RX_reg[4], EMIF_RX_reg[3]}), .next_period_i(next_period), .current_period_i(current_period), .local_counter_i(local_counter), .sync_phase_i(sync_phase), .PWM_o(PWM_o));
  
/*
	wire PWM_dp; 
	wire[7:0] PWM_dp_dt; 
	double_pulse double_pulse(.clk_i(XTAL_20MHz_i), .start_i(PWM_EN_r), .length0_i(EMIF_RX_reg[2][15:0]), .length1_i(EMIF_RX_reg[2][31:16]), .PWM_o(PWM_dp)); 
	deadtime deadtime(.clk_i(XTAL_20MHz_i), .deadtime_i(5'd5), .PWM_i({4{PWM_dp}}), .PWM_o(PWM_dp_dt)); 
	assign PWM_o = PWM_EN_r ? {PWM_dp_dt[0], PWM_dp_dt[1], 6'b0} : 8'b0;  
*/

	wire[8:0] REL_i; 
	wire[8:0] REL_o; 
	assign REL_o = TZ_EN ? REL_i : 9'b0; 
	BB BB_REL_CPU0(.I(1'b0), .T(1'b1), .O(REL_i[0]), .B(CPU_io[`C_SS_RLY_L1_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU1(.I(1'b0), .T(1'b1), .O(REL_i[1]), .B(CPU_io[`GR_RLY_L1_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU2(.I(1'b0), .T(1'b1), .O(REL_i[2]), .B(CPU_io[`C_SS_RLY_L2_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU3(.I(1'b0), .T(1'b1), .O(REL_i[3]), .B(CPU_io[`GR_RLY_L2_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU4(.I(1'b0), .T(1'b1), .O(REL_i[4]), .B(CPU_io[`C_SS_RLY_L3_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU5(.I(1'b0), .T(1'b1), .O(REL_i[5]), .B(CPU_io[`GR_RLY_L3_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU6(.I(1'b0), .T(1'b1), .O(REL_i[6]), .B(CPU_io[`C_SS_RLY_N_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU7(.I(1'b0), .T(1'b1), .O(REL_i[7]), .B(CPU_io[`GR_RLY_N_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU8(.I(1'b0), .T(1'b1), .O(REL_i[8]), .B(CPU_io[`SS_DClink_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	BB BB_REL_FPGA0(.I(REL_o[0]), .T(1'b0), .O(), .B(FPGA_conn_io[`C_SS_RLY_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA1(.I(REL_o[1]), .T(1'b0), .O(), .B(FPGA_conn_io[`GR_RLY_L1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA2(.I(REL_o[2]), .T(1'b0), .O(), .B(FPGA_conn_io[`C_SS_RLY_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA3(.I(REL_o[3]), .T(1'b0), .O(), .B(FPGA_conn_io[`GR_RLY_L2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA4(.I(REL_o[4]), .T(1'b0), .O(), .B(FPGA_conn_io[`C_SS_RLY_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA5(.I(REL_o[5]), .T(1'b0), .O(), .B(FPGA_conn_io[`GR_RLY_L3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA6(.I(REL_o[6]), .T(1'b0), .O(), .B(FPGA_conn_io[`C_SS_RLY_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA7(.I(REL_o[7]), .T(1'b0), .O(), .B(FPGA_conn_io[`GR_RLY_N_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA8(.I(REL_o[8]), .T(1'b0), .O(), .B(FPGA_conn_io[`SS_DClink_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire DClink_DSCH; 
	BB BB_REL_DClink_DSCH_CPU(.I(1'b0), .T(1'b1), .O(DClink_DSCH), .B(CPU_io[`DClink_DSCH_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_REL_DClink_DSCH_FPGA(.I(DClink_DSCH), .T(1'b0), .O(), .B(FPGA_conn_io[`DClink_DSCH_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire[1:0] Fan_PWM; 
	BB BB_Fan_PWM1_FPGA(.I(1'b0), .T(1'b1), .O(Fan_PWM[0]), .B(CPU_io[`FAN_PWM1_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_Fan_PWM2_FPGA(.I(1'b0), .T(1'b1), .O(Fan_PWM[1]), .B(CPU_io[`FAN_PWM2_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_Fan_PWM1_CPU(.I(Fan_PWM[0]), .T(1'b0), .O(), .B(FPGA_conn_io[`FAN_PWM1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_Fan_PWM2_CPU(.I(Fan_PWM[1]), .T(1'b0), .O(), .B(FPGA_conn_io[`FAN_PWM2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
 	BB BB_DIP_FPGA1(.I(1'b0), .T(1'b1), .O(DIP_SWITCH[1]), .B(FPGA_conn_io[`DIP_1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_DIP_FPGA2(.I(1'b0), .T(1'b1), .O(DIP_SWITCH[2]), .B(FPGA_conn_io[`DIP_2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_DIP_FPGA3(.I(1'b0), .T(1'b1), .O(DIP_SWITCH[3]), .B(FPGA_conn_io[`DIP_3_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_DIP_FPGA4(.I(1'b0), .T(1'b1), .O(DIP_SWITCH[4]), .B(FPGA_conn_io[`DIP_4_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_DIP_FPGA5(.I(1'b0), .T(1'b1), .O(DIP_SWITCH[5]), .B(FPGA_conn_io[`DIP_5_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_DIP_FPGA6(.I(1'b0), .T(1'b1), .O(DIP_SWITCH[6]), .B(FPGA_conn_io[`DIP_6_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_DIP_FPGA7(.I(1'b0), .T(1'b1), .O(DIP_SWITCH[7]), .B(FPGA_conn_io[`DIP_7_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_DIP_FPGA8(.I(sync_phase), .T(1'b0), .O(DIP_SWITCH[8]), .B(FPGA_conn_io[`DIP_8_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
 
	wire[5:1] LED_BUS; 
	BB BB_LED_CPU1(.I(1'b0), .T(1'b1), .O(LED_BUS[1]), .B(CPU_io[`LED1_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU2(.I(1'b0), .T(1'b1), .O(LED_BUS[2]), .B(CPU_io[`LED2_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU3(.I(1'b0), .T(1'b1), .O(LED_BUS[3]), .B(CPU_io[`LED3_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU4(.I(1'b0), .T(1'b1), .O(LED_BUS[4]), .B(CPU_io[`LED4_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_LED_CPU5(.I(1'b0), .T(1'b1), .O(LED_BUS[5]), .B(CPU_io[`LED5_CS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
 
	BB BB_LED_FPGA1(.I(LED_BUS[1]), .T(1'b0), .O(), .B(FPGA_conn_io[`LED1_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA2(.I(LED_BUS[2]), .T(1'b0), .O(), .B(FPGA_conn_io[`LED2_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA3(.I(LED_BUS[3]), .T(1'b0), .O(), .B(FPGA_conn_io[`LED3_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA4(.I(sync_rdy), .T(1'b0), .O(), .B(FPGA_conn_io[`LED4_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_LED_FPGA5(.I(LED_BUS[5]), .T(1'b0), .O(), .B(FPGA_conn_io[`LED5_FS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_SED_ENABLE_FPGA(.I(1'b0), .T(1'b1), .O(SED_enable), .B(CPU_io[`FPGA_SED]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 

	BB BB_FLTSPLY_FPGA(.I(1'b0), .T(1'b1), .O(fault_supply), .B(FPGA_conn_io[`FLTSPLY_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	
	wire on_off; 
	BB BB_ON_OFF_FPGA(.I(1'b0), .T(1'b1), .O(on_off), .B(FPGA_conn_io[`ON_OFF_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_ON_OFF_CPU(.I(on_off), .T(1'b0), .O(), .B(CPU_io[`ON_OFF_CS]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	BB BB_COMM_TX0(.I(tx_o[0]), .T(1'b0), .O(), .B(FPGA_conn_io[`TX1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_COMM_TX1(.I(tx_o[1]), .T(1'b0), .O(), .B(FPGA_conn_io[`TX2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_COMM_RX0(.I(1'b0), .T(1'b1), .O(rx_i[0]), .B(FPGA_conn_io[`RX1_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_COMM_RX1(.I(1'b0), .T(1'b1), .O(rx_i[1]), .B(FPGA_conn_io[`RX2_FS]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
endmodule 
