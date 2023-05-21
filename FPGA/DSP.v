`timescale 1ns/1ps

module addr_gen_Kalman(clk, addr_sel, addr_ptr, addr_out);
	parameter ADDR_START_STATES = 9'd0;
	parameter ADDR_START_COMMON = 9'd0;
	parameter ADDR_INC_STATES = 1'b1;
	parameter ADDR_INC_COMMON = 1'b1;
	parameter ADDR_WIDTH = 9;

	localparam OFFSET_WIDTH = ($clog2(ADDR_INC_STATES) > $clog2(ADDR_INC_COMMON) ? $clog2(ADDR_INC_STATES) : $clog2(ADDR_INC_COMMON))+1;

	input clk;
	input [3:0] addr_sel;
	input [OFFSET_WIDTH-1:0] addr_ptr;
	output [ADDR_WIDTH-1:0] addr_out;
	
	reg [ADDR_WIDTH-1:0] cnt[1:0];
	
	initial begin
		cnt[0] = ADDR_START_STATES;
		cnt[1] = ADDR_START_COMMON;
	end
	
	reg addr_sel_r;
	reg [ADDR_WIDTH-1:0] addr_reg[1:0];
	
	assign addr_out = addr_reg[addr_sel_r];
	
	always @(posedge clk) begin
		addr_sel_r <= addr_ptr[OFFSET_WIDTH-1];
		addr_reg[0] <= cnt[0] + addr_ptr[OFFSET_WIDTH-2:0];
		addr_reg[1] <= cnt[1] + addr_ptr[OFFSET_WIDTH-2:0];
		if(addr_sel[0]) cnt[0] <= cnt[0] + ADDR_INC_STATES;
		if(addr_sel[1]) cnt[0] <= ADDR_START_STATES;
		if(addr_sel[2]) cnt[1] <= cnt[1] + ADDR_INC_COMMON;
		if(addr_sel[3]) cnt[1] <= ADDR_START_COMMON;
	end
endmodule

module pipeline_delay(clk, in, out);
	parameter WIDTH = 1;
	parameter CYCLES = 1;
	parameter SHIFT_MEM = 0;
	input clk;
	input [WIDTH-1:0] in;
	output [WIDTH-1:0] out;
	reg [WIDTH-1:0] in_r[CYCLES-1:0];
	
	genvar i;
	
	for (i = 0; i < CYCLES; i = i + 1)
        initial in_r[i] = 0;

	if(SHIFT_MEM) begin
		pmi_distributed_shift_reg
		#(.pmi_data_width(WIDTH), .pmi_regmode("reg"), .pmi_shiftreg_type("fixed"), .pmi_num_shift(CYCLES - 1),
		.pmi_num_width(), .pmi_max_shift(), .pmi_max_width(), .pmi_init_file(),
		.pmi_init_file_format(), .pmi_family())
		Mem0_wreg(.Din(in), .Addr(), .Clock(clk), .ClockEn(1'b1), .Reset(1'b0),
		.Q(out));
	end
	else begin
		always @(posedge clk) in_r[CYCLES-1] <= in;
		for (i = 0; i < CYCLES-1; i = i + 1)
			always @(posedge clk) in_r[i] <= in_r[i+1];
		assign out = in_r[0];
	end
endmodule