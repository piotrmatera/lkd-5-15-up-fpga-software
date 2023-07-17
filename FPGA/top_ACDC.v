`include "global.v"  
 
 //CPU IO MASTER 
`define TRIGGER_CM  0 
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
 
//`define BGA
`define HLQFP
 
module top_ACDC(CPU_io, FPGA_io); 
	inout[168:0] CPU_io;  
	inout[400:1] FPGA_io; 
  	parameter INITIAL_COUNTER = 0;
	
	integer i;  
 
	BB BB_XTAL_25MHz(.I(1'b0), .T(1'b1), .O(XTAL_25MHz_i), .B(FPGA_io[`G+2]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_XTAL_20MHz(.I(1'b0), .T(1'b1), .O(XTAL_20MHz_i), .B(FPGA_io[`G+3]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;
	
	`ifdef BGA 
		BB BB_CLK_CPU(.I(XTAL_20MHz_i), .T(1'b0), .O(), .B(FPGA_io[`K+20]))/*synthesis IO_TYPE="LVCMOS33"*/;  
	`endif
	`ifdef HLQFP 
		BB BB_CLK_CPU(.I(XTAL_20MHz_i), .T(1'b0), .O(), .B(FPGA_io[`P+16]))/*synthesis IO_TYPE="LVCMOS33"*/;  
	`endif
	assign XTAL_20MHz_i2 = XTAL_20MHz_i; 
	 
	GSR GSR_INST (.GSR (1'b1));  
	PUR PUR_INST (.PUR (1'b1));  
/////////////////////////////////////////////////////////////////////  
	localparam EMIF_MEMORY_WIDTH = `COMM_MEMORY_EMIF_WIDTH+4;//$clog2(2x COMM_MEMORY + MUX) = $clog2(3) = 2  
	wire EMIF_oe_i;  
	wire EMIF_we_i;  
	wire [31:0] EMIF_data_i;  
	reg [31:0] EMIF_data_o;  
	wire [EMIF_MEMORY_WIDTH-1:0] EMIF_address_i;  
  
	localparam EMIF_MUX_NUMBER = 28; 
	localparam EMIF_REG_NUMBER = 19; 
	localparam EMIF_MUX_WIDTH = $clog2(EMIF_MUX_NUMBER); 
	localparam EMIF_REG_WIDTH = $clog2(EMIF_REG_NUMBER); 
	wire [31:0] EMIF_TX_mux[EMIF_MUX_NUMBER-1:0]; 
	wire [31:0] EMIF_RX_reg [EMIF_REG_NUMBER-1:0];  

	wire EMIF_oe_i2; 
	wire EMIF_we_i2; 
	wire [31:0] EMIF_data_i2;
	wire [EMIF_MEMORY_WIDTH-1:0] EMIF_address_i2;  
	 
	assign EMIF_oe_i2 = EMIF_oe_i; 
	assign EMIF_we_i2 = EMIF_we_i; 
	assign EMIF_data_i2 = EMIF_data_i;
	assign EMIF_address_i2 = EMIF_address_i;  
	 
/////////////////////////////////////////////////////////////////////  
  
	wire [1:0] rx_i;  
	wire [1:0] tx_o;  
  
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
	wire [8:0] fifo_rx1_port;  
	wire [1:0] fifo_rx1_port_we;  
	wire fifo_rx1_clk_5x;  
	wire rx1_port_rdy;  
	RX_port RX1_port(.rx_clk_i(rx1_clk_10x), .rx_i(rx_i[0]), .rx_clk_phasestep_lag_o(rx1_clk_phasestep_lag), .timestamp_code_o(timestamp_code_rx1), .rx_rdy_o(rx1_port_rdy), 
	.fifo_clk_o(fifo_rx1_clk_5x), .fifo_we_o(fifo_rx1_port_we), .fifo_o(fifo_rx1_port), 
	.clk_i(clk_1x), .rst_i(!LOCK));  
 
	wire timestamp_code_rx2; 
	wire [8:0] fifo_rx2_port;  
	wire [1:0] fifo_rx2_port_we;  
	wire fifo_rx2_clk_5x;  
	wire rx2_port_rdy;  
	RX_port RX2_port(.rx_clk_i(rx2_clk_10x), .rx_i(rx_i[1]), .rx_clk_phasestep_lag_o(rx2_clk_phasestep_lag), .timestamp_code_o(timestamp_code_rx2), .rx_rdy_o(rx2_port_rdy), 
	.fifo_clk_o(fifo_rx2_clk_5x), .fifo_we_o(fifo_rx2_port_we), .fifo_o(fifo_rx2_port), 
	.clk_i(clk_1x), .rst_i(!LOCK));  
 
	wire timestamp_code_tx1; 
	wire [8:0] fifo_tx1_core;  
	wire [1:0] fifo_tx1_core_we;  
	wire fifo_tx1_clk_1x;  
	TX_port TX1_port(.tx_clk_i(tx_clk_5x), .tx_o(tx_o[0]), .timestamp_code_o(timestamp_code_tx1),  
	.fifo1_clk_i(1'b0), .fifo1_we_i(2'b0), .fifo1_i(9'b0),  
	.fifo2_clk_i(fifo_tx1_clk_1x), .fifo2_we_i(fifo_tx1_core_we), .fifo2_i(fifo_tx1_core)); 
 
	wire timestamp_code_tx2; 
	wire [8:0] fifo_tx2_core;  
	wire [1:0] fifo_tx2_core_we;  
	wire fifo_tx2_clk_1x;  
	TX_port TX2_port(.tx_clk_i(tx_clk_5x), .tx_o(tx_o[1]), .timestamp_code_o(timestamp_code_tx2),  
	.fifo1_clk_i(1'b0), .fifo1_we_i(2'b0), .fifo1_i(9'b0),  
	.fifo2_clk_i(fifo_tx2_clk_1x), .fifo2_we_i(fifo_tx2_core_we), .fifo2_i(fifo_tx2_core));  
 	  
/////////////////////////////////////////////////////////////////////  
  
	wire [`POINTER_WIDTH-1:0] Comm_rx1_addrw;  
	wire [7:0] Comm_rx1_dataw;  
	wire Comm_rx1_we;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_ack;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_rdy;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] rx1_hipri_msg_wip;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_ack;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_rdy;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] rx1_lopri_msg_wip;  
	wire [8:0] rx1_fifo_rx_o;  
	wire rx1_fifo_rx_dv; 
	wire [2:0] rx1_state_reg; 
 
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
  
   
	wire [`POINTER_WIDTH-1:0] Comm_rx2_addrw;  
	wire [7:0] Comm_rx2_dataw;  
	wire Comm_rx2_we;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_ack;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_rdy;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] rx2_hipri_msg_wip;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_ack;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_rdy;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] rx2_lopri_msg_wip; 
 
	wire rx2_crc_error;  
	wire rx2_overrun_error;  
	wire rx2_frame_error;  
	RX_core RX2_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b1), 
	.fifo_clk_i(fifo_rx2_clk_5x), .fifo_we_i(fifo_rx2_port_we), .fifo_i(fifo_rx2_port),  
	.mem_addr_o(Comm_rx2_addrw), .mem_we_o(Comm_rx2_we), .mem_data_o(Comm_rx2_dataw),  
	.rx_hipri_msg_ack_i(rx2_hipri_msg_ack), .rx_hipri_msg_wip_o(rx2_hipri_msg_wip), .rx_hipri_msg_rdy_o(rx2_hipri_msg_rdy), 
	.rx_lopri_msg_ack_i(rx2_lopri_msg_ack), .rx_lopri_msg_wip_o(rx2_lopri_msg_wip), .rx_lopri_msg_rdy_o(rx2_lopri_msg_rdy), 
	.rx_crc_error_o(rx2_crc_error), .rx_overrun_error_o(rx2_overrun_error), .rx_frame_error_o(rx2_frame_error));  
  
  
	wire [`POINTER_WIDTH-1:0] Comm_tx1_addrr; 
	wire [7:0] Comm_tx1_datar; 
	 
	wire [7:0] Comm_tx1_mem_datar; 
	reg [7:0] Comm_tx1_mux_datar_r; 
	reg tx1_select_memory; 
	assign Comm_tx1_datar = tx1_select_memory ? Comm_tx1_mux_datar_r : Comm_tx1_mem_datar;  
	 
	wire tx1_code_start;  
	wire [8:0] tx1_code;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] tx1_hipri_msg_start; 
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] tx1_hipri_msg_wip;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] tx1_lopri_msg_start; 
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] tx1_lopri_msg_wip;  
	TX_core TX1_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b1), 
	.fifo_clk_o(fifo_tx1_clk_1x), .fifo_we_o(fifo_tx1_core_we), .fifo_o(fifo_tx1_core),  
	.mem_addr_o(Comm_tx1_addrr), .mem_data_i(Comm_tx1_datar),  
	.tx_hipri_msg_start_i(tx1_hipri_msg_start), .tx_hipri_msg_wip_o(tx1_hipri_msg_wip), 
	.tx_lopri_msg_start_i(tx1_lopri_msg_start), .tx_lopri_msg_wip_o(tx1_lopri_msg_wip), 
	.tx_code_start_i(tx1_code_start), .tx_code_i(tx1_code));  
 
  
	wire [`POINTER_WIDTH-1:0] Comm_tx2_addrr;  
	wire [7:0] Comm_tx2_datar;  
		 
	wire [7:0] Comm_tx2_mem_datar;  
	reg [7:0] Comm_tx2_mux_datar_r;  
	reg tx2_select_memory;  
	assign Comm_tx2_datar = tx2_select_memory ? Comm_tx2_mux_datar_r : Comm_tx2_mem_datar;  
	 
	wire tx2_code_start;
	wire [8:0] tx2_code;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] tx2_hipri_msg_start; 
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] tx2_hipri_msg_wip;  
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] tx2_lopri_msg_start; 
	wire [`LOPRI_MAILBOXES_NUMBER-1:0] tx2_lopri_msg_wip;  
	TX_core TX2_core(.core_clk_i(clk_1x), .hipri_msg_en_i(1'b1), .lopri_msg_en_i(1'b1), 
	.fifo_clk_o(fifo_tx2_clk_1x), .fifo_we_o(fifo_tx2_core_we), .fifo_o(fifo_tx2_core),  
	.mem_addr_o(Comm_tx2_addrr), .mem_data_i(Comm_tx2_datar),  
	.tx_hipri_msg_start_i(tx2_hipri_msg_start), .tx_hipri_msg_wip_o(tx2_hipri_msg_wip), 
	.tx_lopri_msg_start_i(tx2_lopri_msg_start), .tx_lopri_msg_wip_o(tx2_lopri_msg_wip), 
	.tx_code_start_i(tx2_code_start), .tx_code_i(tx2_code));  
	 
	wire [31:0] COMM_RX_MEM1_data_o; 
	wire [31:0] COMM_RX_MEM2_data_o;   
	 
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
	 
	parameter COMM_TX_MEM1_key = 4'd1;
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
	.WrClockEn(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == COMM_TX_MEM1_key[2 +: 2]), .WE(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == COMM_TX_MEM1_key[0 +: 2]), .Data(EMIF_data_i), 
    .RdAddress(Comm_tx1_addrr), .RdClock(clk_1x), .RdClockEn(1'b1), .Reset(1'b0), .Q(Comm_tx1_mem_datar));  
 
	parameter COMM_TX_MEM2_key = 4'd2; 
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
	.WrClockEn(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == COMM_TX_MEM2_key[2 +: 2]), .WE(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == COMM_TX_MEM2_key[0 +: 2]), .Data(EMIF_data_i), 
    .RdAddress(Comm_tx2_addrr), .RdClock(clk_1x), .RdClockEn(1'b1), .Reset(1'b0), .Q(Comm_tx2_mem_datar)); 
 
 
//////////////////////////////////////////////////////////////// 

	wire clk_DSP; 
	wire clk_20MHz; 
	PLL_DSP PLL_DSP (.CLKI(XTAL_25MHz_i), .CLKOP(clk_DSP), .CLKOS(clk_20MHz));
			 
	wire [53:0] Dummy1_CIN;
	wire [53:0] Dummy1_CO;
	wire Dummy1_SIGNEDCIN;
	wire Dummy1_SIGNEDCO; 
	Dummy_slice Dummy1_slice (.CLK0(clk_DSP), .CE0(1'b1), .RST0(1'b0), .AA(18'b0), .AB(18'b0), .BA(18'b0), 
    .BB(18'b0), .C(54'b0), .AMuxsel(2'b0), .BMuxsel(2'b0), .CMuxsel(3'b101), .Opcode(4'b0), 
    .Cin(Dummy1_CIN), .SignCin(Dummy1_SIGNEDCIN), .Result(Dummy1_CO), .SignR(Dummy1_SIGNEDCO), .EQZ( ), .EQZM( ), 
    .EQOM( ), .EQPAT( ), .EQPATB( ), .OVER( ), .UNDER( ), .SROA( ), 
    .SROB( )); 
	
	wire [53:0] Dummy2_CIN;
	wire [53:0] Dummy2_CO;
	wire Dummy2_SIGNEDCIN;
	wire Dummy2_SIGNEDCO; 
	Dummy_slice Dummy2_slice (.CLK0(clk_DSP), .CE0(1'b1), .RST0(1'b0), .AA(18'b0), .AB(18'b0), .BA(18'b0), 
    .BB(18'b0), .C(54'b0), .AMuxsel(2'b0), .BMuxsel(2'b0), .CMuxsel(3'b101), .Opcode(4'b0), 
    .Cin(Dummy2_CIN), .SignCin(Dummy2_SIGNEDCIN), .Result(Dummy2_CO), .SignR(Dummy2_SIGNEDCO), .EQZ( ), .EQZM( ), 
    .EQOM( ), .EQPAT( ), .EQPATB( ), .OVER( ), .UNDER( ), .SROA( ), 
    .SROB( )); 
	
	wire Resonant1_WIP;
	wire Resonant1_START; 
	 
	wire Resonant1_Mem2_we;
	wire [8:0] Resonant1_Mem2_addrw;
	wire [35:0] Resonant1_Mem2_data; 
	wire [31:0] Resonant1_data_o;
	wire [53:0] Resonant1_CIN;
	wire [53:0] Resonant1_CO; 
	wire Resonant1_SIGNEDCIN; 
	wire Resonant1_SIGNEDCO; 

	parameter Resonant1_Mem2_key = 4'd3;
	Resonant_grid #(.HARMONICS_NUM(25)) Resonant1(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i), 
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant1_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Resonant1_Mem2_key[0 +: 2]),
	.enable_i(Resonant1_START), .Mem2_addrw_o(Resonant1_Mem2_addrw), .Mem2_we_o(Resonant1_Mem2_we), .Mem2_data_o(Resonant1_Mem2_data), .WIP_flag_o(Resonant1_WIP), 
	.CIN(Resonant1_CIN), .SIGNEDCIN(Resonant1_SIGNEDCIN), .CO(Resonant1_CO), .SIGNEDCO(Resonant1_SIGNEDCO));

	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"), 
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	) 
	Resonant1_Mem2(.Data(Resonant1_Mem2_data[35:4]), .WrAddress(Resonant1_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant1_Mem2_we), .Reset(1'b0), .Q(Resonant1_data_o));
 
 
	wire Resonant2_WIP;
	wire Resonant2_START;
	
	wire Resonant2_Mem2_we;
	wire [8:0] Resonant2_Mem2_addrw;
	wire [35:0] Resonant2_Mem2_data;
	wire [31:0] Resonant2_data_o;
	wire [53:0] Resonant2_CIN;
	wire [53:0] Resonant2_CO;
	wire Resonant2_SIGNEDCIN;
	wire Resonant2_SIGNEDCO;

	parameter Resonant2_Mem2_key = 4'd4;
	Resonant_grid #(.HARMONICS_NUM(25)) Resonant2(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant2_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Resonant2_Mem2_key[0 +: 2]),
	.enable_i(Resonant2_START), .Mem2_addrw_o(Resonant2_Mem2_addrw), .Mem2_we_o(Resonant2_Mem2_we), .Mem2_data_o(Resonant2_Mem2_data), .WIP_flag_o(Resonant2_WIP),
	.CIN(Resonant2_CIN), .SIGNEDCIN(Resonant2_SIGNEDCIN), .CO(Resonant2_CO), .SIGNEDCO(Resonant2_SIGNEDCO)); 
	
	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	) 
	Resonant2_Mem2(.Data(Resonant2_Mem2_data[35:4]), .WrAddress(Resonant2_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant2_Mem2_we), .Reset(1'b0), .Q(Resonant2_data_o)); 

	wire Resonant3_WIP;
	wire Resonant3_START;
	
	wire Resonant3_Mem2_we;
	wire [8:0] Resonant3_Mem2_addrw;
	wire [35:0] Resonant3_Mem2_data;
	wire [31:0] Resonant3_data_o;
	wire [53:0] Resonant3_CIN;
	wire [53:0] Resonant3_CO;
	wire Resonant3_SIGNEDCIN;
	wire Resonant3_SIGNEDCO;

	parameter Resonant3_Mem2_key = 4'd5;
	Resonant_grid #(.HARMONICS_NUM(25)) Resonant3(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant3_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Resonant3_Mem2_key[0 +: 2]),
	.enable_i(Resonant3_START), .Mem2_addrw_o(Resonant3_Mem2_addrw), .Mem2_we_o(Resonant3_Mem2_we), .Mem2_data_o(Resonant3_Mem2_data), .WIP_flag_o(Resonant3_WIP),
	.CIN(Resonant3_CIN), .SIGNEDCIN(Resonant3_SIGNEDCIN), .CO(Resonant3_CO), .SIGNEDCO(Resonant3_SIGNEDCO));
	
	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	)
	Resonant3_Mem2(.Data(Resonant3_Mem2_data[35:4]), .WrAddress(Resonant3_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant3_Mem2_we), .Reset(1'b0), .Q(Resonant3_data_o)); 
	 
	wire Resonant4_WIP;
	wire Resonant4_START;
	
	wire Resonant4_Mem2_we;
	wire [8:0] Resonant4_Mem2_addrw;
	wire [35:0] Resonant4_Mem2_data;
	wire [31:0] Resonant4_data_o;
	wire [53:0] Resonant4_CIN;
	wire [53:0] Resonant4_CO;
	wire Resonant4_SIGNEDCIN;
	wire Resonant4_SIGNEDCO;

	parameter Resonant4_Mem2_key = 4'd6;
	Resonant_grid #(.HARMONICS_NUM(25)) Resonant4(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant4_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Resonant4_Mem2_key[0 +: 2]),
	.enable_i(Resonant4_START), .Mem2_addrw_o(Resonant4_Mem2_addrw), .Mem2_we_o(Resonant4_Mem2_we), .Mem2_data_o(Resonant4_Mem2_data), .WIP_flag_o(Resonant4_WIP),
	.CIN(Resonant4_CIN), .SIGNEDCIN(Resonant4_SIGNEDCIN), .CO(Resonant4_CO), .SIGNEDCO(Resonant4_SIGNEDCO));
	
	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	)
	Resonant4_Mem2(.Data(Resonant4_Mem2_data[35:4]), .WrAddress(Resonant4_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant4_Mem2_we), .Reset(1'b0), .Q(Resonant4_data_o)); 
		
	wire Resonant5_WIP;
	wire Resonant5_START;
	
	wire Resonant5_Mem2_we;
	wire [8:0] Resonant5_Mem2_addrw;
	wire [35:0] Resonant5_Mem2_data;
	wire [31:0] Resonant5_data_o;
	wire [53:0] Resonant5_CIN;
	wire [53:0] Resonant5_CO;
	wire Resonant5_SIGNEDCIN;
	wire Resonant5_SIGNEDCO;

	parameter Resonant5_Mem2_key = 4'd7;
	Resonant_grid #(.HARMONICS_NUM(25)) Resonant5(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant5_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Resonant5_Mem2_key[0 +: 2]),
	.enable_i(Resonant5_START), .Mem2_addrw_o(Resonant5_Mem2_addrw), .Mem2_we_o(Resonant5_Mem2_we), .Mem2_data_o(Resonant5_Mem2_data), .WIP_flag_o(Resonant5_WIP),
	.CIN(Resonant5_CIN), .SIGNEDCIN(Resonant5_SIGNEDCIN), .CO(Resonant5_CO), .SIGNEDCO(Resonant5_SIGNEDCO));
	
	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	)
	Resonant5_Mem2(.Data(Resonant5_Mem2_data[35:4]), .WrAddress(Resonant5_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant5_Mem2_we), .Reset(1'b0), .Q(Resonant5_data_o)); 	
	
	wire Resonant6_WIP;
	wire Resonant6_START;
	
	wire Resonant6_Mem2_we;
	wire [8:0] Resonant6_Mem2_addrw;
	wire [35:0] Resonant6_Mem2_data;
	wire [31:0] Resonant6_data_o;
	wire [53:0] Resonant6_CIN;
	wire [53:0] Resonant6_CO;
	wire Resonant6_SIGNEDCIN;
	wire Resonant6_SIGNEDCO;

	parameter Resonant6_Mem2_key = 4'd8;
	Resonant_grid #(.HARMONICS_NUM(25)) Resonant6(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Resonant6_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Resonant6_Mem2_key[0 +: 2]),
	.enable_i(Resonant6_START), .Mem2_addrw_o(Resonant6_Mem2_addrw), .Mem2_we_o(Resonant6_Mem2_we), .Mem2_data_o(Resonant6_Mem2_data), .WIP_flag_o(Resonant6_WIP),
	.CIN(Resonant6_CIN), .SIGNEDCIN(Resonant6_SIGNEDCIN), .CO(Resonant6_CO), .SIGNEDCO(Resonant6_SIGNEDCO));
	
	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	)
	Resonant6_Mem2(.Data(Resonant6_Mem2_data[35:4]), .WrAddress(Resonant6_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Resonant6_Mem2_we), .Reset(1'b0), .Q(Resonant6_data_o)); 
		
		
	wire Kalman_DC_WIP;
	wire Kalman_DC_START;
	
	wire Kalman_DC_Mem2_we;
	wire [8:0] Kalman_DC_Mem2_addrw;
	wire [35:0] Kalman_DC_Mem2_data;
	wire [31:0] Kalman_DC_data_o;
	wire [53:0] Kalman_DC_CIN;
	wire [53:0] Kalman_DC_CO;
	wire Kalman_DC_SIGNEDCIN;
	wire Kalman_DC_SIGNEDCO;

	parameter Kalman_DC_Mem2_key = 4'd9;
	Kalman #(.HARMONICS_NUM(50), .IN_SERIES_NUM(2)) Kalman_DC(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Kalman_DC_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Kalman_DC_Mem2_key[0 +: 2]),
	.enable_i(Kalman_DC_START), .Mem2_addrw_o(Kalman_DC_Mem2_addrw), .Mem2_we_o(Kalman_DC_Mem2_we), .Mem2_data_o(Kalman_DC_Mem2_data), .WIP_flag_o(Kalman_DC_WIP),
	.CIN(Kalman_DC_CIN), .SIGNEDCIN(Kalman_DC_SIGNEDCIN), .CO(Kalman_DC_CO), .SIGNEDCO(Kalman_DC_SIGNEDCO));

	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	)
	Kalman_DC_Mem2(.Data(Kalman_DC_Mem2_data[35:4]), .WrAddress(Kalman_DC_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Kalman_DC_Mem2_we), .Reset(1'b0), .Q(Kalman_DC_data_o));
 
	wire Kalman1_WIP;
	wire Kalman1_START;
	
	wire Kalman1_Mem2_we;
	wire [8:0] Kalman1_Mem2_addrw;
	wire [35:0] Kalman1_Mem2_data;
	wire [31:0] Kalman1_data_o;
	wire [53:0] Kalman1_CIN;
	wire [53:0] Kalman1_CO;
	wire Kalman1_SIGNEDCIN;
	wire Kalman1_SIGNEDCO;

	parameter Kalman1_Mem2_key = 4'd10;
	Kalman #(.HARMONICS_NUM(26), .IN_SERIES_NUM(6)) Kalman1(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Kalman1_Mem2_key[2 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 2] == Kalman1_Mem2_key[0 +: 2]),
	.enable_i(Kalman1_START), .Mem2_addrw_o(Kalman1_Mem2_addrw), .Mem2_we_o(Kalman1_Mem2_we), .Mem2_data_o(Kalman1_Mem2_data), .WIP_flag_o(Kalman1_WIP),
	.CIN(Kalman1_CIN), .SIGNEDCIN(Kalman1_SIGNEDCIN), .CO(Kalman1_CO), .SIGNEDCO(Kalman1_SIGNEDCO));

	pmi_ram_dp #(.pmi_wr_addr_depth(512), .pmi_wr_addr_width(9), .pmi_wr_data_width(32),
	.pmi_rd_addr_depth(512), .pmi_rd_addr_width(9), .pmi_rd_data_width(32), .pmi_regmode("noreg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file(/*"../Mem2.mem"*/), .pmi_init_file_format(/*"hex"*/)
	) 
	Kalman1_Mem2(.Data(Kalman1_Mem2_data[35:4]), .WrAddress(Kalman1_Mem2_addrw), .RdAddress(EMIF_address_i[8:0]), .WrClock(clk_DSP), .RdClock(!EMIF_oe_i), .WrClockEn(1'b1), .RdClockEn(1'b1), 
	.WE(Kalman1_Mem2_we), .Reset(1'b0), .Q(Kalman1_data_o));
	
	assign Dummy1_CIN = 0;
	assign Dummy1_SIGNEDCIN = 0;
	assign Dummy2_CIN = 0;
	assign Dummy2_SIGNEDCIN = 0;		 
	assign Resonant1_CIN = Dummy1_CO;
	assign Resonant1_SIGNEDCIN = Dummy1_SIGNEDCO; 
	assign Resonant2_CIN = Resonant1_CO;
	assign Resonant2_SIGNEDCIN = Resonant1_SIGNEDCO;
	assign Resonant3_CIN = Resonant2_CO;
	assign Resonant3_SIGNEDCIN = Resonant2_SIGNEDCO; 
	assign Resonant4_CIN = Resonant3_CO;
	assign Resonant4_SIGNEDCIN = Resonant3_SIGNEDCO;  
	assign Resonant5_CIN = Resonant4_CO;
	assign Resonant5_SIGNEDCIN = Resonant4_SIGNEDCO;  
	assign Resonant6_CIN = Resonant5_CO;
	assign Resonant6_SIGNEDCIN = Resonant5_SIGNEDCO; 
	assign Kalman_DC_CIN = Dummy2_CO;
	assign Kalman_DC_SIGNEDCIN = Dummy2_SIGNEDCO; 
	assign Kalman1_CIN = Kalman_DC_CO;
	assign Kalman1_SIGNEDCIN = Kalman_DC_SIGNEDCO; 
	
	assign {Kalman1_START, Kalman_DC_START, Resonant6_START, Resonant5_START, Resonant4_START, Resonant3_START, Resonant2_START, Resonant1_START} = EMIF_RX_reg[9][7:0]; 
	 
/////////////////////////////////////////////////////////////////////  
  
	localparam OSR = `DEF_OSR;  
	localparam SD_WIDTH = 16;  
	localparam SD_NUMBER = 11;  
	wire [1:0] decimator_pulse;  
	wire new_value;  
	reg avg_value;  
	wire new_value_early;  
	reg SD_sync_pulse;  
	wire select_SD;  
	SD_filter_sync_gen #(.OSR(OSR)) SD_filter_sync_gen1(.clk_i(clk_20MHz), .sync_pulse_i(SD_sync_pulse),  
	.decimator_pulse_o(decimator_pulse), .select_o(select_SD), .new_value_o(new_value), .new_value_early_o(new_value_early));  
  
	wire [SD_NUMBER-1:0] SD_DAT;  
	wire [SD_NUMBER-1:0] SD_DAT_IDDR;  
	wire [5-1:0] SD_CLK;  
 	ODDRX1F SD_CLK_gen[5-1:0] (.D0(1'b0), .D1(1'b1), .SCLK(clk_20MHz), .RST(1'b0), .Q(SD_CLK));  
	 
    IDDRX1F SD_IDDR[SD_NUMBER-1:0](.D(SD_DAT), .RST(1'b0), .SCLK(clk_20MHz), .Q0(SD_DAT_IDDR), .Q1()); 
  
	wire [SD_WIDTH*SD_NUMBER-1:0] SD_dat;  
	SD_filter_sync #(.ORDER(2), .OSR(OSR), .OUTPUT_WIDTH(SD_WIDTH)) SD_filter_sync[SD_NUMBER-1:0](.data_i({~SD_DAT_IDDR[10:8], SD_DAT_IDDR[7:3], ~SD_DAT_IDDR[2:0]}),  
	.clk_i(clk_20MHz), .decimator_pulse_i(decimator_pulse), .select_i(select_SD), .data_o(SD_dat));  
	 
	wire [SD_WIDTH*SD_NUMBER-1:0] SD_dat_avg; 
	if(`CONTROL_RATE > 1) begin
		MovingAverage #(.N(`CONTROL_RATE)) SD_average[SD_NUMBER-1:0](.clk(clk_20MHz), .enable(new_value), .in(SD_dat), .out(SD_dat_avg)); 
	end 
	else begin 
		assign SD_dat_avg = SD_dat; 
	end 
	 
	wire [SD_WIDTH-1+2:0] SD_dat_In; 
	assign SD_dat_In = 
	{{2{SD_dat[8*16+15]}}, SD_dat[8*16 +: 16]} +
	{{2{SD_dat[9*16+15]}}, SD_dat[9*16 +: 16]} +
	{{2{SD_dat[10*16+15]}}, SD_dat[10*16 +: 16]}; 
	 
	localparam COMPARE_NUMBER = 7; 
	wire [COMPARE_NUMBER*2-1:0] compare_o; 
	wire [COMPARE_NUMBER*SD_WIDTH-1:0] compare_dat; 
	assign compare_dat[0*SD_WIDTH +: 3*SD_WIDTH] = SD_dat[8*SD_WIDTH +: 3*SD_WIDTH]; 
	assign compare_dat[3*SD_WIDTH +: SD_WIDTH] = SD_dat_In[SD_WIDTH-1+2:2]; 
	assign compare_dat[4*SD_WIDTH +: 3*SD_WIDTH] = SD_dat[0*SD_WIDTH +: 3*SD_WIDTH]; 
	compare_LH compare_LH[COMPARE_NUMBER-1:0](.clk_i(clk_20MHz), .value_i(compare_dat), 
	.compare_value_i({EMIF_RX_reg[17], EMIF_RX_reg[16], EMIF_RX_reg[15], EMIF_RX_reg[14], EMIF_RX_reg[13], EMIF_RX_reg[12], EMIF_RX_reg[11]}), 
	.compare_o(compare_o)); 
	 
	wire local_counter_pulse_rate_latch;
	wire local_counter_pulse_rate; 
	Sync_latch_input #(.OUT_POLARITY(1), .STEPS(2)) local_counter_pulse_rate_latch_avg(.clk_i(clk_20MHz), .in(local_counter_pulse_rate), .out(local_counter_pulse_rate_latch), .reset_i(new_value), .set_i(1'b0));  
	 
	always @(posedge clk_20MHz) 
		avg_value <= new_value & local_counter_pulse_rate_latch; 
	 
/////////////////////////////////////////////////////////////////////  
	 
	wire SED_enable; 
	SED_machine SED_machine(.clk_i(XTAL_20MHz_i), .enable_i(SED_enable), .err_o(sed_err)); 
 
	localparam FAULT_NUMBER = 28;  
  
	wire [FAULT_NUMBER-1:0] FLT_REG_O;  
	wire [FAULT_NUMBER-1:0] FLT_bus;  
 
	assign FLT_bus[23:16] = ~compare_o[7:0];  
	assign FLT_bus[24] = ~|compare_o[9:8];  
	assign FLT_bus[25] = ~|compare_o[11:10];  
	assign FLT_bus[26] = ~|compare_o[13:12];   
	assign FLT_bus[27] = !sed_err; 
	//assign FLT_bus[16] = !rx1_crc_error;  
	//assign FLT_bus[17] = !rx1_overrun_error;  
	//assign FLT_bus[18] = !rx1_frame_error;  
	//assign FLT_bus[19] = !rx2_crc_error;  
	//assign FLT_bus[20] = !rx2_overrun_error;  
	//assign FLT_bus[21] = !rx2_frame_error;  
	//assign FLT_bus[22] = rx1_port_rdy;  
	//assign FLT_bus[23] = rx2_port_rdy;
  
 	wire rst_faults; 
	FD1P3BX FLT_ff[FAULT_NUMBER-1:0](.D(1'b0), .SP(rst_faults), .CK(clk_1x), .PD(~FLT_bus), .Q(FLT_REG_O));  
 
	wire TZ_FPGA;
	assign TZ_FPGA = !(|FLT_REG_O); 

/////////////////////////////////////////////////////////////////////   
	 	 
	defparam TX1_port.TIMESTAMP_CODE = `K_Timestamp_slave;	
	defparam TX2_port.TIMESTAMP_CODE = `K_Timestamp_master;
	defparam RX1_port.TIMESTAMP_CODE = `K_Timestamp_master;
	defparam RX2_port.TIMESTAMP_CODE = `K_Timestamp_slave;	
	  
	wire [63:0] snapshot_value; 
	wire [15:0] local_free_counter;  
	wire [1:0] shift_value;
	wire shift_start;
	
	Local_free_counter #(.INITIAL_COUNTER(INITIAL_COUNTER)) Local_free_counter(.clk_i(clk_5x),
	.shift_value_i(shift_value), .shift_start_i(shift_start), .local_counter_o(local_free_counter),
	.snapshot_start_i({timestamp_code_rx2, timestamp_code_tx2, timestamp_code_rx1, timestamp_code_tx1}), .snapshot_value_o(snapshot_value));  
	 
	wire [15:0] local_counter;  
	wire [15:0] next_period;
	wire [15:0] current_period;
	wire [1:0] shift_value2;
	wire shift_start2;
	wire local_counter_phase; 
	Local_counter Local_counter(.clk_i(clk_5x), .shift_value_i(shift_value2), .shift_start_i(shift_start2),
	.next_period_o(next_period), .current_period_o(current_period), .local_counter_o(local_counter),
	.sync_phase_o(local_counter_phase), .sync_rate_o(local_counter_pulse_rate), .sync_o(local_counter_pulse));  
 
 	always @(posedge clk_5x) SD_sync_pulse <= local_counter >= EMIF_RX_reg [2][15:0];
		
	reg local_counter_timestamp_new = 0;  
	reg [15:0] local_counter_timestamp = 0;
	reg local_counter_timestamp_phase = 0;
	always @(posedge clk_5x) begin
		local_counter_timestamp_new <= local_free_counter == 0;
		if(local_counter_timestamp_new) begin
			local_counter_timestamp <= local_counter;
			local_counter_timestamp_phase <= local_counter_phase;
		end
	end

	wire comm_pulse; 
	Local_counter #(.CYCLE_PERIOD(`KALMAN_CYCLE_PERIOD)) Local_counter2(.clk_i(clk_5x), .shift_value_i(shift_value), .shift_start_i(shift_start),
	.next_period_o(), .current_period_o(), .local_counter_o(),
	.sync_phase_o(), .sync_rate_o(), .sync_o(comm_pulse));  
	
/////////////////////////////////////////////////////////////////////   
		 
	wire [`HIPRI_MAILBOXES_NUMBER*16-1:0] clock_offsets; 
	wire [`HIPRI_MAILBOXES_NUMBER*16-1:0] comm_delays;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] rx_ok;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] sync_ok;  
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] slave_rdy; 
	wire [`HIPRI_MAILBOXES_NUMBER-1:0] scope_trigger_request; 
	Master_sync Master_sync(.clk_i(clk_1x), .pulse_cycle_i(tx2_code_start), 
	.rx_addrw_i(Comm_rx2_addrw), .rx_dataw_i(Comm_rx2_dataw), .rx_we_i(Comm_rx2_we), 
	.snapshot_value_i(snapshot_value), .clock_offsets_o(clock_offsets), .comm_delays_o(comm_delays), 
	.rx_ok_o(rx_ok), .sync_ok_o(sync_ok), .slave_rdy_o(slave_rdy), .scope_trigger_request_o(scope_trigger_request)); 

/////////////////////////////////////////////////////////////////////   

	wire[`HIPRI_MAILBOXES_WIDTH-1:0] node_number;
	wire node_number_rdy; 
	Node_number_eval Node_number_eval(.clk_1x_i(clk_1x), .fifo_clk_i(fifo_rx1_clk_5x), .fifo_we_i(fifo_rx1_port_we), .fifo_i(fifo_rx1_port), .node_number_o(node_number), .node_number_rdy_o(node_number_rdy)); 
 
	wire[31:0] Kalman_offset; 
	wire[31:0] Kalman_rate;  
	wire[15:0] offset_memory;  
	wire sync_rdy; 
	Slave_sync Slave_sync(.clk_i(clk_1x), .msg_rdy_i(rx1_hipri_msg_rdy[0]),
	.local_counter_timestamp_i(local_counter_timestamp), .local_counter_timestamp_phase_i(local_counter_timestamp_phase), .local_counter_timestamp_new_i(local_counter_timestamp_new),
	.rx_addrw_i(Comm_rx1_addrw), .rx_dataw_i(Comm_rx1_dataw), .rx_we_i(Comm_rx1_we), 
	.node_number_i(node_number), .node_number_rdy_i(node_number_rdy),
	.Kalman_offset_o(Kalman_offset), .Kalman_rate_o(Kalman_rate), .offset_memory_o(offset_memory),
	.shift_value_o(shift_value), .shift_start_o(shift_start), 
	.shift_value2_o(shift_value2), .shift_start2_o(shift_start2), 
	.phase_shift_i(EMIF_RX_reg[18][15:0]),
	.sync_rdy_o(sync_rdy)); 

/////////////////////////////////////////////////////////////////////   
	
	reg master_slave_selector;
	reg tx2_code_start_last = 0;
	reg[8:0] tx2_code_start_delay_counter = 0;
	always @(posedge clk_5x) begin
		master_slave_selector <= !rx1_port_rdy && rx2_port_rdy;
		
		tx2_code_start_last <= tx2_code_start;
		if((tx2_code_start && !tx2_code_start_last) | (|tx2_code_start_delay_counter)) tx2_code_start_delay_counter <= tx2_code_start_delay_counter + 1'b1;
	end
	
	assign rx1_hipri_msg_ack = {EMIF_RX_reg[1][1*8+1 +: `HIPRI_MAILBOXES_NUMBER-1], rx1_hipri_msg_rdy[0]}; 
	assign rx2_hipri_msg_ack = rx2_hipri_msg_rdy;//EMIF_RX_reg [1][3*8 +: `HIPRI_MAILBOXES_NUMBER];  
 
	assign rx1_lopri_msg_ack = EMIF_RX_reg [1][0*8 +: `LOPRI_MAILBOXES_NUMBER];  
	assign rx2_lopri_msg_ack = EMIF_RX_reg [1][2*8 +: `LOPRI_MAILBOXES_NUMBER];  
	 
	assign tx1_hipri_msg_start = {EMIF_RX_reg[0][1*8+1 +: `HIPRI_MAILBOXES_NUMBER-1], rx1_hipri_msg_wip[0]}; 
	assign tx2_hipri_msg_start = {EMIF_RX_reg [0][3*8+1 +: `HIPRI_MAILBOXES_NUMBER-1], tx2_code_start_delay_counter[8] && master_slave_selector};  
	 
	assign tx1_lopri_msg_start = EMIF_RX_reg [0][0*8 +: `LOPRI_MAILBOXES_NUMBER];  
	assign tx2_lopri_msg_start = EMIF_RX_reg [0][2*8 +: `LOPRI_MAILBOXES_NUMBER];  
 	
 	assign tx1_code = `K_Timestamp_slave; 
	assign tx2_code = master_slave_selector ? `K_Timestamp_master : `K_Enum_nodes; 
	assign tx1_code_start = timestamp_code_rx1;  
	assign tx2_code_start = master_slave_selector ? comm_pulse : timestamp_code_rx1; 
	
	wire PWM_EN_r;
	wire scope_trigger_request_slave;
	assign scope_trigger_request_slave = 0;
	always @(posedge clk_1x) begin 
		tx1_select_memory <= 1'b1; 
		tx2_select_memory <= 1'b1; 
		
		case(Comm_tx1_addrr)
			11'h400 : Comm_tx1_mux_datar_r <= {4'h0, {4-`HIPRI_MAILBOXES_WIDTH{1'b0}}, node_number};
			11'h401 : Comm_tx1_mux_datar_r <= 8'd15 - 8'd1;
			11'h402 : Comm_tx1_mux_datar_r <= {5'b0, scope_trigger_request_slave, PWM_EN_r, sync_rdy}; 
			11'h403 : Comm_tx1_mux_datar_r <= 8'b0; 
			11'h404 : Comm_tx1_mux_datar_r <= snapshot_value[0*8 +: 8];
			11'h405 : Comm_tx1_mux_datar_r <= snapshot_value[1*8 +: 8];
			11'h406 : Comm_tx1_mux_datar_r <= snapshot_value[2*8 +: 8];
			11'h407 : Comm_tx1_mux_datar_r <= snapshot_value[3*8 +: 8];
			11'h408 : Comm_tx1_mux_datar_r <= snapshot_value[4*8 +: 8];
			11'h409 : Comm_tx1_mux_datar_r <= snapshot_value[5*8 +: 8];
			11'h40A : Comm_tx1_mux_datar_r <= snapshot_value[6*8 +: 8];
			11'h40B : Comm_tx1_mux_datar_r <= snapshot_value[7*8 +: 8];
			default : begin
				tx1_select_memory <= 1'b0;
				Comm_tx1_mux_datar_r <= 8'd0;
			end
		endcase
		case(Comm_tx2_addrr) 
			11'h400 : Comm_tx2_mux_datar_r <= {4'd0, 4'd0}; 
			11'h401 : Comm_tx2_mux_datar_r <= 8'd17 - 8'd1; 
			11'h402 : Comm_tx2_mux_datar_r <= {{8-`HIPRI_MAILBOXES_NUMBER{1'b0}}, rx_ok}; 
			11'h403 : Comm_tx2_mux_datar_r <= {6'b0, local_counter_timestamp_phase, local_counter_phase}; 
			11'h404 : Comm_tx2_mux_datar_r <= clock_offsets[0*8 +: 8]; 
			11'h405 : Comm_tx2_mux_datar_r <= clock_offsets[1*8 +: 8]; 
			11'h406 : Comm_tx2_mux_datar_r <= clock_offsets[2*8 +: 8]; 
			11'h407 : Comm_tx2_mux_datar_r <= clock_offsets[3*8 +: 8]; 
			11'h408 : Comm_tx2_mux_datar_r <= clock_offsets[4*8 +: 8]; 
			11'h409 : Comm_tx2_mux_datar_r <= clock_offsets[5*8 +: 8];  
			11'h40A : Comm_tx2_mux_datar_r <= clock_offsets[6*8 +: 8];  
			11'h40B : Comm_tx2_mux_datar_r <= clock_offsets[7*8 +: 8]; 
			11'h40C : Comm_tx2_mux_datar_r <= `CYCLE_PERIOD;
			11'h40D : Comm_tx2_mux_datar_r <= `CYCLE_PERIOD>>8;
			11'h40E : Comm_tx2_mux_datar_r <= local_counter_timestamp[0*8 +: 8];
			11'h40F : Comm_tx2_mux_datar_r <= local_counter_timestamp[1*8 +: 8];
			default : begin 
				tx2_select_memory <= 1'b0; 
				Comm_tx2_mux_datar_r <= 8'd0; 
			end 
		endcase 
	end  
	 
///////////////////////////////////////////////////////////////////// 

	localparam DEB_WRITE_WIDTH_MULTIPLY = 2;
	localparam DEB_WADDR_WIDTH = 9; 
	
	localparam DEB_READ_WIDTH = 36;
	localparam DEB_WRITE_WIDTH = DEB_WRITE_WIDTH_MULTIPLY*DEB_READ_WIDTH;
	localparam DEB_WADDR_DEPTH = 2**DEB_WADDR_WIDTH;
	localparam DEB_RADDR_DEPTH = DEB_WRITE_WIDTH_MULTIPLY*DEB_WADDR_DEPTH;
	localparam DEB_RADDR_WIDTH = $clog2(DEB_RADDR_DEPTH);
	 
	wire Scope_enable; 
	wire Scope_enable_extended; 
	reg [5:0] Scope_enable_counter;
	wire Scope_trigger; 
	reg Scope_trigger_r;
	wire Scope_WE;
	wire [DEB_READ_WIDTH-1:0] Scope_data_out; 
	wire [DEB_WRITE_WIDTH-1:0] Scope_data_in;
	reg [DEB_WADDR_WIDTH-1:0] Scope_index; 
	reg [DEB_WADDR_WIDTH-1:0] Scope_index_last; 
	reg [DEB_WADDR_WIDTH-1:0] Scope_acquire_counter; 
	reg [DEB_WADDR_WIDTH-1:0] Scope_acquire_before_counter; 
	wire [DEB_WADDR_WIDTH-1:0] Scope_acquire_before_trigger;
	 
	localparam STATES_WIDTH = 2;
	localparam [STATES_WIDTH-1:0]
    Scope_0 = 0,
    Scope_1 = 1,
    Scope_2 = 2,
	Scope_3 = 3;
	reg [STATES_WIDTH-1:0] Scope_state; 
	 
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
					Scope_acquire_counter <= DEB_WADDR_DEPTH[DEB_WADDR_WIDTH-1:0] - Scope_acquire_before_trigger; 
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
    .RdAddress(EMIF_RX_reg [3][DEB_RADDR_WIDTH-1:0]), .RdClock(!EMIF_oe_i), .RdClockEn(1'b1), .Reset(1'b0), .Q(Scope_data_out)); 
 
	reg [31:0] timestamp_counter;
	reg [31:0] timestamp_mem;
	wire [31:0] timestamp_diff;
	
	assign timestamp_diff = timestamp_counter - timestamp_mem;
	always @(posedge clk_1x) begin
		timestamp_counter <= timestamp_counter + 1'b1;
		if(Scope_enable_extended) timestamp_mem <= timestamp_counter;
	end 
	
	assign Scope_acquire_before_trigger = EMIF_RX_reg[4][DEB_WADDR_WIDTH-1:0];
	assign Scope_trigger = EMIF_RX_reg[5][0]; 
		
	wire [7:0] PWM_i; 
	assign Scope_enable = 1'b1; 
	assign Scope_data_in = {PWM_i[6], PWM_i[4], PWM_i[2], PWM_i[0], FLT_REG_O[15:0], FLT_bus[15:0], local_counter_phase, Scope_trigger_r, timestamp_diff}; 

	 
/////////////////////////////////////////////////////////////////////  
  
	always @(*) begin 
		case(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4])
			0 : EMIF_data_o = EMIF_TX_mux[EMIF_address_i[EMIF_MUX_WIDTH-1:0]];
			1 : EMIF_data_o = COMM_RX_MEM1_data_o; 
			2 : EMIF_data_o = COMM_RX_MEM2_data_o; 
			3 : EMIF_data_o = Resonant1_data_o; 
			4 : EMIF_data_o = Resonant2_data_o; 
			5 : EMIF_data_o = Resonant3_data_o;  
			6 : EMIF_data_o = Resonant4_data_o;
			7 : EMIF_data_o = Resonant5_data_o;
			8 : EMIF_data_o = Resonant6_data_o;
			9 : EMIF_data_o = Kalman_DC_data_o; 
			10 : EMIF_data_o = Kalman1_data_o;
			11 : EMIF_data_o = 0;
			12 : EMIF_data_o = 0;
			13 : EMIF_data_o = 0;
			14 : EMIF_data_o = 0;
			15 : EMIF_data_o = 0;
		endcase 
	end
 	
	localparam [15:0] CONTROL_RATE = `CONTROL_RATE;
	localparam [15:0] CYCLE_PERIOD = `CYCLE_PERIOD;
	localparam [15:0] DEF_OSR = `DEF_OSR;
	localparam WIDTH = $clog2((DEF_OSR**2)+1)+1;
	localparam OUTPUT_SHIFT_BIT = (WIDTH - SD_WIDTH > 0) ? WIDTH - SD_WIDTH : 0;
	localparam [15:0] OUTPUT_SHIFT = 2**OUTPUT_SHIFT_BIT;
		 
	assign EMIF_TX_mux[0] = {tx2_hipri_msg_wip, tx2_lopri_msg_wip, tx1_hipri_msg_wip, tx1_lopri_msg_wip};  
	assign EMIF_TX_mux[1] = {rx2_hipri_msg_rdy, rx2_lopri_msg_rdy, rx1_hipri_msg_rdy, rx1_lopri_msg_rdy};  
	assign EMIF_TX_mux[2] = SD_dat[0*32 +: 32]; 
	assign EMIF_TX_mux[3] = SD_dat[1*32 +: 32]; 
	assign EMIF_TX_mux[4] = SD_dat[2*32 +: 32]; 
	assign EMIF_TX_mux[5] = SD_dat[3*32 +: 32]; 
	assign EMIF_TX_mux[6] = SD_dat[4*32 +: 32]; 
	assign EMIF_TX_mux[7] = SD_dat[5*32 +: 16]; 
	assign EMIF_TX_mux[8] = SD_dat_avg[0*32 +: 32]; 
	assign EMIF_TX_mux[9] = SD_dat_avg[1*32 +: 32]; 
	assign EMIF_TX_mux[10] = SD_dat_avg[2*32 +: 32]; 
	assign EMIF_TX_mux[11] = SD_dat_avg[3*32 +: 32]; 
	assign EMIF_TX_mux[12] = SD_dat_avg[4*32 +: 32]; 
	assign EMIF_TX_mux[13] = SD_dat_avg[5*32 +: 16]; 
	assign EMIF_TX_mux[14] = {{32-FAULT_NUMBER{1'b0}}, FLT_REG_O};  
	assign EMIF_TX_mux[15] = {2'b0, master_slave_selector, sync_rdy, node_number_rdy, node_number, slave_rdy, sync_ok, rx_ok};
	assign EMIF_TX_mux[16] = EMIF_RX_reg [2]; 
	assign EMIF_TX_mux[17] = `VERSION; 
	assign EMIF_TX_mux[18] = Scope_data_out[31:0]; 
	assign EMIF_TX_mux[19] = {28'b0, Scope_data_out[DEB_READ_WIDTH-1:32]}; 
	assign EMIF_TX_mux[20] = DEB_RADDR_DEPTH; 
	assign EMIF_TX_mux[21] = DEB_WRITE_WIDTH_MULTIPLY; 
	assign EMIF_TX_mux[22] = Scope_state[1]; 
	assign EMIF_TX_mux[23] = Scope_index_last; 
	assign EMIF_TX_mux[24] = {CONTROL_RATE, CYCLE_PERIOD}; 
	assign EMIF_TX_mux[25] = {OUTPUT_SHIFT, DEF_OSR}; 
	assign EMIF_TX_mux[26] = {local_counter_phase, Kalman1_WIP, Kalman_DC_WIP, Resonant6_WIP, Resonant5_WIP, Resonant4_WIP, Resonant3_WIP, Resonant2_WIP, Resonant1_WIP};  
	assign EMIF_TX_mux[27] = next_period;
	
 	FD1P3DX EMIF_RX_reg_0[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 0), .CK(EMIF_we_i), .CD({tx2_hipri_msg_wip, tx2_lopri_msg_wip, tx1_hipri_msg_wip, tx1_lopri_msg_wip}), .Q(EMIF_RX_reg [0])); 
 	FD1P3DX EMIF_RX_reg_1[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 1), .CK(EMIF_we_i), .CD(~{rx2_hipri_msg_rdy, rx2_lopri_msg_rdy, rx1_hipri_msg_rdy, rx1_lopri_msg_rdy}), .Q(EMIF_RX_reg [1])); 
	EMIF_RX_reg #(.INIT_VAL(32'd1625)) EMIF_RX_reg_2(.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 2), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg [2])); 
	EMIF_RX_reg #(.INIT_VAL(32'd0)) EMIF_RX_reg_3(.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 3), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg [3])); 
	EMIF_RX_reg #(.INIT_VAL(DEB_WADDR_DEPTH>>1)) EMIF_RX_reg_4(.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 4), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg [4])); 
	EMIF_RX_reg #(.INIT_VAL(32'd0)) EMIF_RX_reg_5(.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 5), .CK(EMIF_we_i), .RST(1'b0), .Q(EMIF_RX_reg [5]));
 	FD1P3DX EMIF_RX_reg_6[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 6), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [6])); 
 	FD1P3DX EMIF_RX_reg_7[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 7), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [7])); 
 	FD1P3DX EMIF_RX_reg_8[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 8), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [8])); 
 	FD1P3DX EMIF_RX_reg_9[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 9), .CK(EMIF_we_i), .CD({24'b0, Kalman1_WIP, Kalman_DC_WIP, Resonant6_WIP, Resonant5_WIP, Resonant4_WIP, Resonant3_WIP, Resonant2_WIP, Resonant1_WIP}), .Q(EMIF_RX_reg [9])); 
 	FD1P3DX EMIF_RX_reg_10[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 10), .CK(EMIF_we_i), .CD({32{!rst_faults}}), .Q(EMIF_RX_reg [10])); 
 	FD1P3DX EMIF_RX_reg_11[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 11), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [11])); 
 	FD1P3DX EMIF_RX_reg_12[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 12), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [12])); 
 	FD1P3DX EMIF_RX_reg_13[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 13), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [13])); 
 	FD1P3DX EMIF_RX_reg_14[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 14), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [14])); 
 	FD1P3DX EMIF_RX_reg_15[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 15), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [15])); 
 	FD1P3DX EMIF_RX_reg_16[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 16), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [16])); 
 	FD1P3DX EMIF_RX_reg_17[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 17), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [17]));
 	FD1P3DX EMIF_RX_reg_18[31:0](.D(EMIF_data_i), .SP(EMIF_address_i[EMIF_MEMORY_WIDTH-4 +: 4] == 4'b0 && EMIF_address_i[EMIF_REG_WIDTH-1:0] == 18), .CK(EMIF_we_i), .CD(1'b0), .Q(EMIF_RX_reg [18]));
 
/////////////////////////////////////////////////////////////////////  
 
	BB BB_EMIF_OE(.I(1'b0), .T(1'b1), .O(EMIF_oe_i), .B(CPU_io[`EM1OE]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/;  
	BB BB_EMIF_WE(.I(1'b0), .T(1'b1), .O(EMIF_we_i), .B(CPU_io[`EM1WE]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP"*/; 
  
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
	BB BB_EMIF_address12(.I(1'b0), .T(1'b1), .O(EMIF_address_i[12]), .B(CPU_io[`EM1A12])) `EMIF_address_options;  
 
 
	BB BB_RST(.I(1'b0), .T(1'b1), .O(rst_faults), .B(CPU_io[`RST_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */;  
 
	BB BB_SD_NEW(.I(new_value), .T(1'b0), .O(), .B(CPU_io[`SD_NEW_CM]))/*synthesis IO_TYPE="LVCMOS33"*/; 
	BB BB_SD_AVG(.I(avg_value), .T(1'b0), .O(), .B(CPU_io[`SD_AVG_CM]))/*synthesis IO_TYPE="LVCMOS33"*/; 
	BB BB_SYNC_PWM(.I(local_counter_pulse_rate), .T(1'b0), .O(), .B(CPU_io[`SYNC_PWM_CM]))/*synthesis IO_TYPE="LVCMOS33"*/;
	 
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
 	BB BB_SD_DAT6(.I(1'b0), .T(1'b1), .O(SD_DAT[6]), .B(FPGA_io[`SD_UDC_1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT7(.I(1'b0), .T(1'b1), .O(SD_DAT[7]), .B(FPGA_io[`SD_UDC_05_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT8(.I(1'b0), .T(1'b1), .O(SD_DAT[8]), .B(FPGA_io[`SD_I_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT9(.I(1'b0), .T(1'b1), .O(SD_DAT[9]), .B(FPGA_io[`SD_I_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_SD_DAT10(.I(1'b0), .T(1'b1), .O(SD_DAT[10]), .B(FPGA_io[`SD_I_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;
   
	wire CLK_supply_meas; 
	wire CLK_supply_drv; 
	CLK_supply #(.SUPPLY_PERIOD(20e6/50e3), .SUPPLY_RATE(4)) CLK_supply_3_3mH(.clk_i(XTAL_20MHz_i), .clk_o(CLK_supply_meas));  
	CLK_supply #(.SUPPLY_PERIOD(20e6/100e3), .SUPPLY_RATE(7)) CLK_supply_470uH(.clk_i(XTAL_20MHz_i), .clk_o(CLK_supply_drv));  
	 
	BB BB_PWM_DCDC_UDC(.I(CLK_supply_meas), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_UDC_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_PWM_DCDC_TRDS(.I(CLK_supply_meas), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_TRDS_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_PWM_DCDC_ISO(.I(CLK_supply_meas), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_ISO_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_PWM_DCDC_DRV(.I(CLK_supply_drv), .T(1'b0), .O(), .B(FPGA_io[`PWM_DCDC_DRV_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  

	BB BB_FLT_FPGA0(.I(1'b0), .T(1'b1), .O(FLT_bus[0]), .B(FPGA_io[`FLT_H_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_FLT_FPGA1(.I(1'b0), .T(1'b1), .O(FLT_bus[1]), .B(FPGA_io[`FLT_L_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_FLT_FPGA2(.I(1'b0), .T(1'b1), .O(FLT_bus[2]), .B(FPGA_io[`FLT_H_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_FLT_FPGA3(.I(1'b0), .T(1'b1), .O(FLT_bus[3]), .B(FPGA_io[`FLT_L_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_FLT_FPGA4(.I(1'b0), .T(1'b1), .O(FLT_bus[4]), .B(FPGA_io[`FLT_H_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_FLT_FPGA5(.I(1'b0), .T(1'b1), .O(FLT_bus[5]), .B(FPGA_io[`FLT_L_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_FLT_FPGA6(.I(1'b0), .T(1'b1), .O(FLT_bus[6]), .B(FPGA_io[`FLT_H_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	BB BB_FLT_FPGA7(.I(1'b0), .T(1'b1), .O(FLT_bus[7]), .B(FPGA_io[`FLT_L_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
 
	wire [7:0] REL_i; 
	wire [7:0] REL_o; 
	wire [7:0] RDY_temp;
	assign FLT_bus[15:8] = RDY_temp | {8{REL_i[6]}};
	BB BB_FLT_FPGA8(.I(!EMIF_RX_reg [10][8]), .T(1'b0), .O(RDY_temp[0]), .B(FPGA_io[`RDY_H_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA9(.I(!EMIF_RX_reg [10][9]), .T(1'b0), .O(RDY_temp[1]), .B(FPGA_io[`RDY_L_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA10(.I(!EMIF_RX_reg [10][10]), .T(1'b0), .O(RDY_temp[2]), .B(FPGA_io[`RDY_H_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA11(.I(!EMIF_RX_reg [10][11]), .T(1'b0), .O(RDY_temp[3]), .B(FPGA_io[`RDY_L_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA12(.I(!EMIF_RX_reg [10][12]), .T(1'b0), .O(RDY_temp[4]), .B(FPGA_io[`RDY_H_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA13(.I(!EMIF_RX_reg [10][13]), .T(1'b0), .O(RDY_temp[5]), .B(FPGA_io[`RDY_L_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA14(.I(!EMIF_RX_reg [10][14]), .T(1'b0), .O(RDY_temp[6]), .B(FPGA_io[`RDY_H_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
	BB BB_FLT_FPGA15(.I(!EMIF_RX_reg [10][15]), .T(1'b0), .O(RDY_temp[7]), .B(FPGA_io[`RDY_L_N_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" OPENDRAIN="ON" */; 
  
	wire [7:0] PWM_o; 
	BB BB_PWM_FPGA0(.I(PWM_o[0]), .T(1'b0), .O(PWM_i[0]), .B(FPGA_io[`PWM_H_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA1(.I(PWM_o[1]), .T(1'b0), .O(PWM_i[1]), .B(FPGA_io[`PWM_L_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA2(.I(PWM_o[2]), .T(1'b0), .O(PWM_i[2]), .B(FPGA_io[`PWM_H_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA3(.I(PWM_o[3]), .T(1'b0), .O(PWM_i[3]), .B(FPGA_io[`PWM_L_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA4(.I(PWM_o[4]), .T(1'b0), .O(PWM_i[4]), .B(FPGA_io[`PWM_H_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA5(.I(PWM_o[5]), .T(1'b0), .O(PWM_i[5]), .B(FPGA_io[`PWM_L_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA6(.I(PWM_o[6]), .T(1'b0), .O(PWM_i[6]), .B(FPGA_io[`PWM_H_N_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_PWM_FPGA7(.I(PWM_o[7]), .T(1'b0), .O(PWM_i[7]), .B(FPGA_io[`PWM_L_N_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire TRIGGER; 
	reg TRIGGER_r; 
	BB BB_TRIGGER(.I(1'b0), .T(1'b1), .O(TRIGGER), .B(CPU_io[`TRIGGER_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	always @(posedge clk_1x) 
		TRIGGER_r <= TRIGGER; 
 
	wire PWM_EN; 
	wire TZ_EN_CPU1;  
	wire TZ_EN_CPU2; 
	BB BB_PWM_EN(.I(1'b0), .T(1'b1), .O(PWM_EN), .B(CPU_io[`PWM_EN_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_TZ_EN_CPU1(.I(1'b0), .T(1'b1), .O(TZ_EN_CPU1), .B(CPU_io[`TZ_EN_CPU1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;  
	BB BB_TZ_EN_CPU2(.I(1'b0), .T(1'b1), .O(TZ_EN_CPU2), .B(CPU_io[`TZ_EN_CPU2_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	reg [1:0] sync_reg; 
	always @(posedge clk_5x) 
		sync_reg <= {local_counter_phase, sync_reg [1]}; 
  
 	assign TZ_CLR = !(TZ_EN_CPU1 & TZ_EN_CPU2 & PWM_EN & TZ_FPGA);
	FD1P3DX PWM_EN_ff(.D(PWM_EN), .SP(sync_reg [1] ^ sync_reg [0]), .CK(clk_5x), .CD(TZ_CLR), .Q(PWM_EN_r)); 
 
	Symmetrical_PWM #(.DEADTIME(65)) 
	Symmetrical_PWM[3:0](.clk_i(clk_5x), .enable_output_i(PWM_EN_r), .override_i(EMIF_RX_reg [10][7:0]), .duty_i({EMIF_RX_reg [7], EMIF_RX_reg [6]}), 
	.next_period_i(next_period), .current_period_i(current_period), .local_counter_i(local_counter), .sync_phase_i(local_counter_phase), .PWM_o(PWM_o));

	//wire PWM_dp; 
	//wire [7:0] PWM_dp_dt; 
	//double_pulse double_pulse(.clk_i(XTAL_20MHz_i), .start_i(PWM_EN_r), .length0_i(EMIF_RX_reg [8][15:0]), .length1_i(EMIF_RX_reg [8][31:16]), .PWM_o(PWM_dp)); 
	//deadtime deadtime[3:0](.clk_i(XTAL_20MHz_i), .PWM_i({4{PWM_dp}}), .enable_i({PWM_EN_r, 3'b0}), .override_i(EMIF_RX_reg [10][7:0]), .PWM_o(PWM_o)); 

	assign REL_o = (TZ_EN_CPU1 & TZ_EN_CPU2 & TZ_FPGA) ? REL_i : 8'b0; 
	BB BB_REL_CPU0(.I(1'b0), .T(1'b1), .O(REL_i[0]), .B(CPU_io[`C_SS_RLY_L1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU1(.I(1'b0), .T(1'b1), .O(REL_i[1]), .B(CPU_io[`GR_RLY_L1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU2(.I(1'b0), .T(1'b1), .O(REL_i[2]), .B(CPU_io[`C_SS_RLY_L2_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU3(.I(1'b0), .T(1'b1), .O(REL_i[3]), .B(CPU_io[`GR_RLY_L2_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU4(.I(1'b0), .T(1'b1), .O(REL_i[4]), .B(CPU_io[`C_SS_RLY_L3_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU5(.I(1'b0), .T(1'b1), .O(REL_i[5]), .B(CPU_io[`GR_RLY_L3_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;
	BB BB_REL_CPU8(.I(1'b0), .T(1'b1), .O(REL_i[6]), .B(CPU_io[`SS_DClink_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
	BB BB_REL_CPU7(.I(1'b0), .T(1'b1), .O(REL_i[7]), .B(CPU_io[`GR_RLY_N_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;  
	//BB BB_REL_CPU6(.I(1'b0), .T(1'b1), .O(REL_i[8]), .B(CPU_io[`DClink_DSCH_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */; 
  
 	BB BB_REL_FPGA0(.I(REL_o[0]), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA1(.I(REL_o[1]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA2(.I(REL_o[2]), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA3(.I(REL_o[3]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA4(.I(REL_o[4]), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA5(.I(REL_o[5]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */;
	BB BB_REL_FPGA8(.I(REL_o[6]), .T(1'b0), .O(), .B(FPGA_io[`SS_DClink_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	BB BB_REL_FPGA7(.I(REL_o[7]), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_N_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	//BB BB_REL_FPGA6(.I(REL_o[8]), .T(1'b0), .O(), .B(FPGA_io[`DClink_DSCH_CM]))/*synthesis IO_TYPE="LVCMOS33" */; 
  
	//BB BB_REL_FPGA0(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_REL_FPGA1(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L1_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_REL_FPGA2(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_REL_FPGA3(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L2_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_REL_FPGA4(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`C_SS_RLY_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_REL_FPGA5(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_L3_FM]))/*synthesis IO_TYPE="LVCMOS33" */;
	//BB BB_REL_FPGA8(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`SS_DClink_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	//BB BB_REL_FPGA7(.I(SED_enable), .T(1'b0), .O(), .B(FPGA_io[`GR_RLY_N_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
	//BB BB_REL_FPGA6(.I(REL_o[8]), .T(1'b0), .O(), .B(FPGA_io[`DClink_DSCH_CM]))/*synthesis IO_TYPE="LVCMOS33" */; 
  
	wire [5:1] LED_BUS;  
	BB BB_LED_CPU1(.I(1'b0), .T(1'b1), .O(LED_BUS[1]), .B(CPU_io[`LED1_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;  
	BB BB_LED_CPU2(.I(1'b0), .T(1'b1), .O(LED_BUS[2]), .B(CPU_io[`LED2_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;  
	BB BB_LED_CPU3(.I(1'b0), .T(1'b1), .O(LED_BUS[3]), .B(CPU_io[`LED3_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;  
	BB BB_LED_CPU4(.I(1'b0), .T(1'b1), .O(LED_BUS[4]), .B(CPU_io[`LED4_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;  
	BB BB_LED_CPU5(.I(1'b0), .T(1'b1), .O(LED_BUS[5]), .B(CPU_io[`LED5_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="DOWN" */;  
  
	BB BB_LED_FPGA1(.I(LED_BUS[1]), .T(1'b0), .O(), .B(FPGA_io[`LED1_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_LED_FPGA2(.I(LED_BUS[2]), .T(1'b0), .O(), .B(FPGA_io[`LED2_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_LED_FPGA3(.I(LED_BUS[3]), .T(1'b0), .O(), .B(FPGA_io[`LED3_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_LED_FPGA4(.I(LED_BUS[4]), .T(1'b0), .O(), .B(FPGA_io[`LED4_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_LED_FPGA5(.I(LED_BUS[5]), .T(1'b0), .O(), .B(FPGA_io[`LED5_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
 
	BB BB_SED_ENABLE_FPGA(.I(1'b0), .T(1'b1), .O(SED_enable), .B(CPU_io[`FPGA_SED]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */;  
 
	wire on_off;  
	BB BB_ON_OFF_FPGA(.I(1'b0), .T(1'b1), .O(on_off), .B(FPGA_io[`ON_OFF_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */;  
	BB BB_ON_OFF_CPU(.I(on_off), .T(1'b0), .O(), .B(CPU_io[`ON_OFF_CM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	 
	wire FAN; 
	BB BB_FAN_CPU(.I(1'b0), .T(1'b1), .O(FAN), .B(CPU_io[`FAN_CM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="UP" */; 
	BB BB_FAN_FPGA(.I(FAN), .T(1'b0), .O(), .B(FPGA_io[`FAN_FM]))/*synthesis IO_TYPE="LVCMOS33" */; 
 
	wire Modbus_EN;  
	wire Modbus_TX;  
	wire Modbus_RX;  

	assign Modbus_EN = 1'b0;
	assign Modbus_TX = 1'b1; 
	 
	BB BB_Modbus_EN_FPGA(.I(Modbus_EN), .T(1'b0), .O(), .B(FPGA_io[`EN_Mod_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_Modbus_TX_FPGA(.I(Modbus_TX), .T(1'b0), .O(), .B(FPGA_io[`TX_Mod_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
	BB BB_Modbus_RX_FPGA(.I(1'b0), .T(1'b1), .O(Modbus_RX), .B(FPGA_io[`RX_Mod_FM]))/*synthesis IO_TYPE="LVCMOS33" */;  
   
   	BB BB_COMM_TX0(.I(tx_o[0]), .T(1'b0), .O(), .B(FPGA_io[`TX1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */;  
	BB BB_COMM_TX1(.I(tx_o[1]), .T(1'b0), .O(), .B(FPGA_io[`TX2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */;  
	BB BB_COMM_RX0(.I(1'b0), .T(1'b1), .O(rx_i[0]), .B(FPGA_io[`RX1_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */;  
	BB BB_COMM_RX1(.I(1'b0), .T(1'b1), .O(rx_i[1]), .B(FPGA_io[`RX2_FM]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */; 
	  
	BB BB_TEST(.I(local_free_counter[10]), .T(1'b0), .O(), .B(FPGA_io[`K+4]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */;  
	BB BB_TEST2(.I(local_counter_pulse), .T(1'b0), .O(), .B(FPGA_io[`L+5]))/*synthesis IO_TYPE="LVCMOS33" PULLMODE="NONE" */;  
endmodule  
