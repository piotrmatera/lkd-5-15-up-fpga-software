`include "global.v"

module Clocking_block(XTAL_i, rx1_phase_i, rx2_phase_i, LOCK_o, local_5x_clk_o, tx_clk_5x_o, rx1_clk_10x_o, rx2_clk_10x_o);
	input XTAL_i;
	input rx1_phase_i;
	input rx2_phase_i;
	output LOCK_o;
	output local_5x_clk_o;
	output tx_clk_5x_o;
	output rx1_clk_10x_o;
	output rx2_clk_10x_o;

	reg[1:0] PHASESEL_r;
    reg PHASESTEP_r;

	PLL_SerDes PLL_SerDes (.CLKI(XTAL_i), .PHASESEL(PHASESEL_r), .PHASEDIR(1'b0), .PHASESTEP(PHASESTEP_r), .CLKOP(rx1_clk_10x_o), .CLKOS(rx2_clk_10x_o), .CLKOS2(tx_clk_5x_o), .CLKOS3(local_5x_clk_o), .LOCK(LOCK_o), .PHASELOADREG(1'b0));

	reg[2:0] rx1_phase_r;
	reg[2:0] rx2_phase_r;

	reg rx1_phase_flag;
	reg rx2_phase_flag;
	reg tx_phase_flag;

	reg[6:0] tx_clk_periodic_slow_r;

	reg[1:0] state_reg;

	always @(posedge tx_clk_5x_o) begin
		rx1_phase_r <= {rx1_phase_i, rx1_phase_r[2:1]};
		rx2_phase_r <= {rx2_phase_i, rx2_phase_r[2:1]};
		tx_clk_periodic_slow_r <= tx_clk_periodic_slow_r + 1'b1;

		if(rx1_phase_r[1] & !rx1_phase_r[0])
			rx1_phase_flag <= 1'b1;
		if(rx2_phase_r[1] & !rx2_phase_r[0])
			rx2_phase_flag <= 1'b1;
		if(&tx_clk_periodic_slow_r)
			tx_phase_flag <= 1'b1;

		case (state_reg)
			0 : begin
				if (tx_phase_flag) begin
					PHASESEL_r <= 2'b01;
					tx_phase_flag <= 1'b0;
				end
				else if (rx1_phase_flag) begin
					PHASESEL_r <= 2'b11;
					rx1_phase_flag <= 1'b0;
				end
				else if (rx2_phase_flag) begin
					PHASESEL_r <= 2'b00;
					rx2_phase_flag <= 1'b0;
				end

				PHASESTEP_r <= 1'b0;
				if(tx_phase_flag | rx1_phase_flag | rx2_phase_flag)
					state_reg <= 1;
			end
			1 : begin
				PHASESTEP_r <= 1'b1;
				state_reg <= 2;
			end
			2 : begin
				PHASESTEP_r <= 1'b0;
				state_reg <= 3;
			end
			3 : begin
				PHASESTEP_r <= 1'b0;
				state_reg <= 0;
			end
		endcase
	end

	initial begin
		PHASESEL_r = 0;
		state_reg = 0;
		rx1_phase_flag = 0;
		rx2_phase_flag = 0;
		tx_phase_flag = 0;
		rx1_phase_r = 0;
		rx2_phase_r = 0;
		tx_clk_periodic_slow_r = 0;
		PHASESTEP_r = 0;
	end

endmodule

module TX_port(tx_clk_i, tx_o, timestamp_code_o, fifo1_clk_i, fifo1_we_i, fifo1_i, fifo2_clk_i, fifo2_we_i, fifo2_i);
	input tx_clk_i;
	output tx_o;
	output reg timestamp_code_o;

	input fifo1_clk_i;
	input[1:0] fifo1_we_i;
	input[8:0] fifo1_i;

	input fifo2_clk_i;
	input[1:0] fifo2_we_i;
	input[8:0] fifo2_i;
	
	parameter TIMESTAMP_CODE = 0;

///////////////////////////////////////////////////////////////////// 

	wire[8:0] fifo1_sync_o;
	wire[8:0] fifo1_async_o;
	wire[8:0] fifo2_sync_o;
	wire[8:0] fifo2_async_o;

	pmi_fifo_dc 
	#(.pmi_data_width_w(9),
	.pmi_data_width_r(9),
	.pmi_data_depth_w(512),
	.pmi_data_depth_r(512),
	.pmi_full_flag(512),
	.pmi_empty_flag(0),
	.pmi_almost_full_flag(508),
	.pmi_almost_empty_flag(4),
	.pmi_regmode("noreg"),		//"reg", "noreg"
	.pmi_resetmode("sync"),		//"async", "sync"
	.pmi_family("ECP5U"),
	.pmi_implementation("EBR")		//"EBR", "LUT"
	) FIFO1_sync(.Data(fifo1_i), .WrClock(fifo1_clk_i), .RdClock(tx_clk_i), .WrEn(fifo1_we_i[0]), .RdEn(fifo1_sync_re),
	.Reset(1'b0), .RPReset(1'b0), .Q(fifo1_sync_o), .Empty(fifo1_sync_empty), .Full(), .AlmostEmpty(), .AlmostFull());
	
	pmi_fifo_dc 
	#(.pmi_data_width_w(9),
	.pmi_data_width_r(9),
	.pmi_data_depth_w(512),
	.pmi_data_depth_r(512),
	.pmi_full_flag(512),
	.pmi_empty_flag(0),
	.pmi_almost_full_flag(508),
	.pmi_almost_empty_flag(4),
	.pmi_regmode("noreg"),		//"reg", "noreg"
	.pmi_resetmode("sync"),		//"async", "sync"
	.pmi_family("ECP5U"),
	.pmi_implementation("EBR")		//"EBR", "LUT"
	) FIFO1_async(.Data(fifo1_i), .WrClock(fifo1_clk_i), .RdClock(tx_clk_i), .WrEn(fifo1_we_i[1]), .RdEn(fifo1_async_re),
	.Reset(1'b0), .RPReset(1'b0), .Q(fifo1_async_o), .Empty(fifo1_async_empty), .Full(), .AlmostEmpty(), .AlmostFull());
	
	pmi_fifo_dc 
	#(.pmi_data_width_w(9),
	.pmi_data_width_r(9),
	.pmi_data_depth_w(512),
	.pmi_data_depth_r(512),
	.pmi_full_flag(512),
	.pmi_empty_flag(0),
	.pmi_almost_full_flag(508),
	.pmi_almost_empty_flag(4),
	.pmi_regmode("noreg"),		//"reg", "noreg"
	.pmi_resetmode("sync"),		//"async", "sync"
	.pmi_family("ECP5U"),
	.pmi_implementation("EBR")		//"EBR", "LUT"
	) FIFO2_sync(.Data(fifo2_i), .WrClock(fifo2_clk_i), .RdClock(tx_clk_i), .WrEn(fifo2_we_i[0]), .RdEn(fifo2_sync_re),
	.Reset(1'b0), .RPReset(1'b0), .Q(fifo2_sync_o), .Empty(fifo2_sync_empty), .Full(), .AlmostEmpty(), .AlmostFull());

	pmi_fifo_dc 
	#(.pmi_data_width_w(9),
	.pmi_data_width_r(9),
	.pmi_data_depth_w(512),
	.pmi_data_depth_r(512),
	.pmi_full_flag(512),
	.pmi_empty_flag(0),
	.pmi_almost_full_flag(508),
	.pmi_almost_empty_flag(4),
	.pmi_regmode("noreg"),		//"reg", "noreg"
	.pmi_resetmode("sync"),		//"async", "sync"
	.pmi_family("ECP5U"),
	.pmi_implementation("EBR")		//"EBR", "LUT"
	) FIFO2_async(.Data(fifo2_i), .WrClock(fifo2_clk_i), .RdClock(tx_clk_i), .WrEn(fifo2_we_i[1]), .RdEn(fifo2_async_re),
	.Reset(1'b0), .RPReset(1'b0), .Q(fifo2_async_o), .Empty(fifo2_async_empty), .Full(), .AlmostEmpty(), .AlmostFull());
	
	localparam STATES_WIDTH = 2;
	localparam [STATES_WIDTH-1:0]
    s0 = 0,
    s1 = 1,
    s2 = 2,
	s3 = 3;
    reg[STATES_WIDTH-1:0] state_reg, state_reg_async_last;
	reg[2:0] DIV5_bits;
	
	assign fifo1_async_re = DIV5_bits[1:0] == 2'b01 && state_reg == s0 && !fifo1_async_empty;
	assign fifo1_sync_re = DIV5_bits[1:0] == 2'b01 && state_reg == s1 && !fifo1_sync_empty;
	assign fifo2_async_re = DIV5_bits[1:0] == 2'b01 && state_reg == s2 && !fifo2_async_empty;
	assign fifo2_sync_re = DIV5_bits[1:0] == 2'b01 && state_reg == s3 && !fifo2_sync_empty;
	reg fifo_re_r;

	reg[8:0] enc_i;
	reg[3:0] empty_counter;
	reg DIV5_zero;
	
	always @(posedge tx_clk_i) begin
		if(DIV5_bits[1:0] == 2'b10) begin
			if(fifo_re_r)
				empty_counter <= 0;
			else if(!empty_counter[3])
				empty_counter <= empty_counter + 1'b1;
		end
		
		DIV5_zero <= DIV5_bits[2];

		fifo_re_r <= fifo1_sync_re | fifo1_async_re | fifo2_sync_re | fifo2_async_re;
		if(fifo_re_r)
			case (state_reg)
				s0 : enc_i <= fifo1_async_o;
				s1 : enc_i <= fifo1_sync_o;
				s2 : enc_i <= fifo2_async_o;
				s3 : enc_i <= fifo2_sync_o;
			endcase
		else
			enc_i <= `K_Idle;
			
		if(DIV5_zero) begin
			case (state_reg)
				s0 : begin
					state_reg_async_last <= state_reg;
					if(!fifo1_sync_empty)
						state_reg <= s1;
					else if(!fifo2_sync_empty)
						state_reg <= s3;
					else if(!fifo1_async_empty || empty_counter < 4'd8)
						state_reg <= s0;
					else if(!fifo2_async_empty)
						state_reg <= s2;
				end
				s1 : begin
					if(!fifo1_sync_empty || empty_counter < 4'd2)
						state_reg <= s1;
					else if(!fifo2_sync_empty)
						state_reg <= s3;
					else
						state_reg <= state_reg_async_last;
				end
				s2 : begin
					state_reg_async_last <= state_reg;
					if(!fifo1_sync_empty)
						state_reg <= s1;
					else if(!fifo2_sync_empty)
						state_reg <= s3;
					else if(!fifo2_async_empty || empty_counter < 4'd8)
						state_reg <= s2;
					else if(!fifo1_async_empty)
						state_reg <= s0;
				end
				s3 : begin
					if(!fifo2_sync_empty || empty_counter < 4'd2)
						state_reg <= s3;
					else if(!fifo1_sync_empty)
						state_reg <= s1;
					else
						state_reg <= state_reg_async_last;
				end
			endcase
		end
	end

	initial begin
		empty_counter = 0;
		fifo_re_r = 0;
		state_reg = 0;
		state_reg_async_last = 0;
	end

	wire[9:0] enc_o;
	wire running_disparity;
	assign enc_en_i = DIV5_bits == 3'b011;
	
	pmi_rom
	#(.pmi_addr_depth(1024),
	.pmi_addr_width(10),
	.pmi_data_width(11),
	.pmi_regmode("noreg"),	//"reg", "noreg"
	.pmi_gsr("disable"),		//"enable", "disable"
	.pmi_resetmode("sync"),	//"async", "sync"
	.pmi_optimization("speed"),   //"speed", "area"
	.pmi_init_file("../enc_table_8b10b.mem"),
	.pmi_init_file_format("binary"),	//"binary", "hex"
	.pmi_family("ECP5U")
	) enc_ROM_8b10b (.Address({running_disparity, enc_i}), .OutClock(tx_clk_i), .OutClockEn(enc_en_i), .Reset(1'b0), .Q({running_disparity, enc_o}));
	
	reg[9:0] barrel;
	always @(posedge tx_clk_i) begin
		if(enc_en_i)
			timestamp_code_o <= TIMESTAMP_CODE == enc_i;
		
		if(DIV5_bits[2]) begin
			DIV5_bits <= 0;
			barrel <= enc_o;
		end
		else begin
			barrel <= {2'b0, barrel[9:2]};
			DIV5_bits <= DIV5_bits + 1'b1;
		end
	end

	initial begin
		timestamp_code_o = 0;
		DIV5_bits = 0;
		barrel = 0;
	end

	ODDRX1F ODDR_ser (.D0(barrel[0]), .D1(barrel[1]), .SCLK(tx_clk_i), .RST(1'b0), .Q(tx_o));

endmodule

module RX_port(rx_clk_i, rx_i, rx_clk_phasestep_lag_o, timestamp_code_o, rx_rdy_o, fifo_clk_o, fifo_we_o, fifo_o, clk_i, rst_i);
	input rx_clk_i;
	input rx_i;
	output reg rx_clk_phasestep_lag_o;
	output reg timestamp_code_o;
	output rx_rdy_o;
	
	output fifo_clk_o;
	output reg [1:0] fifo_we_o;
	output reg [8:0] fifo_o;
	input clk_i;
	input rst_i;
	
	parameter TIMESTAMP_CODE = 0;

	reg[1:0] rst_r;
	always @(posedge clk_i) begin
		rst_r <= {rst_i, rst_r[1]};
	end

	initial begin
		rst_r = 0;
	end

	reg[7:0] counter_alignwd;
	wire[3:0] DDR1_o;
	wire clk_single;
	IDDR IDDR (.alignwd(counter_alignwd[6]), .clkin(rx_clk_i), .ready(), .sclk(clk_single), .start(!rst_r[0]), 
    .sync_clk(clk_i), .sync_reset(rst_r[0]), .datain(rx_i), .q(DDR1_o));
	assign fifo_clk_o = clk_single;

	reg[1:0] rx_clk_phasestep_lag_r;
	reg[3:0] phasestep_lag_counter;
	reg[2:0] DIV5_bits;
	reg[9:0] barrel;
	reg bit_offset;

	assign clk_phase_advancing = DDR1_o[3] ^ DDR1_o[2] || DDR1_o[1] ^ DDR1_o[0];

	reg[2:0] rst2_r;
	assign comma_even = barrel[7:0] == 8'h7C | barrel[7:0] == 8'h83;
	assign comma_odd = barrel[8:1] == 8'h7C | barrel[8:1] == 8'h83;
	always @(posedge clk_single) begin
		if(DIV5_bits[2] | comma_even)
			DIV5_bits <= 0;
		else
			DIV5_bits <= DIV5_bits + 1'b1;

		barrel <= {!DDR1_o[3], !DDR1_o[1], barrel[9:2]};

		rst2_r <= {rst_i, rst2_r[2:1]};
		if(rst2_r[0]) begin
			bit_offset <= 1'b0;
			counter_alignwd <= 0;
		end else begin
			if(bit_offset)
				counter_alignwd <= counter_alignwd + 1'b1;
			if(comma_odd)
				bit_offset <= 1'b1;
			if(counter_alignwd == 8'hFF)
				bit_offset <= 1'b0;
		end

		if(rx_clk_phasestep_lag_o) begin
			phasestep_lag_counter <= phasestep_lag_counter + 1'b1;
			if(phasestep_lag_counter == -4'd1)
				rx_clk_phasestep_lag_o <= 0;
		end
		else begin
			phasestep_lag_counter <= 0;
			rx_clk_phasestep_lag_o <= clk_phase_advancing;
		end
	end

	initial begin
		rx_clk_phasestep_lag_o = 0;
		phasestep_lag_counter = 0;
		DIV5_bits = 0;
		barrel = 0;
		bit_offset = 0;
		rst2_r = 0;
		counter_alignwd = 0;
	end

	wire[9:0] dec_i;
	wire[8:0] dec_o;
	
	assign dec_i = {!DDR1_o[3], !DDR1_o[1], barrel[9:2]};
	assign dec_en_i = DIV5_bits == 3'b011 || DIV5_bits == 3'b100;
	
	pmi_rom
	#(.pmi_addr_depth(1024),
	.pmi_addr_width(10),
	.pmi_data_width(16),
	.pmi_regmode("reg"),	//"reg", "noreg"
	.pmi_gsr("disable"),		//"enable", "disable"
	.pmi_resetmode("sync"),	//"async", "sync"
	.pmi_optimization("speed"),   //"speed", "area"
	.pmi_init_file("../dec_table_8b10b.mem"),
	.pmi_init_file_format("binary"),	//"binary", "hex"
	.pmi_family("ECP5U")
	) dec_ROM_8b10b (.Address(dec_i), .OutClock(clk_single), .OutClockEn(dec_en_i), .Reset(1'b0), .Q({Forward_async, Forward_sync, K_Start_Sync_Packet, K_idle, K_error, Rdisp_in, Rdisp_out, dec_o}));
	
	reg DIV5_zero;
	reg parity_last;
	reg[3:0] idle_counter;
	assign rx_rdy_o = idle_counter[3];
	assign error = DIV5_zero && ((Rdisp_in != Rdisp_out && parity_last != Rdisp_in) || K_error);
	reg sync_msg;
	reg K_idle_last;

	always @(posedge clk_single) begin 
		timestamp_code_o <= fifo_o == TIMESTAMP_CODE;
		DIV5_zero <= DIV5_bits[2];
		fifo_we_o <= 2'b0;
		
		if(DIV5_zero) begin
			fifo_o <= dec_o;
			fifo_we_o[0] <= rx_rdy_o && !error && Forward_sync && (sync_msg | (Forward_sync & dec_o[8]));
			fifo_we_o[1] <= rx_rdy_o && !error && Forward_async && !sync_msg;
		
			if(error)
				idle_counter <= 4'b0;
			else if(K_idle && !rx_rdy_o)
				idle_counter <= idle_counter + 1'b1;
			
			if(Rdisp_in != Rdisp_out)
				parity_last <= Rdisp_out;
			
			K_idle_last <= K_idle;
			
			if(K_Start_Sync_Packet)
				sync_msg <= 1'b1;
			else if((dec_o[8] & !K_idle) || (K_idle && K_idle_last))
				sync_msg <= 1'b0;
		end
	end 
 
	initial begin 
		fifo_o = 0; 
		fifo_we_o = 0; 
		DIV5_zero = 0; 
		sync_msg = 0; 
		idle_counter = 0; 
		parity_last = 0;
		timestamp_code_o = 0;
	end 
endmodule