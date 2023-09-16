`include "global.v" 

module compare_LH(clk_i, value_i, compare_value_i, compare_o);
	parameter WIDTH = 16;
		
	input wire clk_i;
	input wire signed [WIDTH-1:0] value_i;
	input wire [2*WIDTH-1:0] compare_value_i;
	output reg [1:0] compare_o;
	
	wire signed [WIDTH-1:0] compare_value_H_i;
	wire signed [WIDTH-1:0] compare_value_L_i;
	assign compare_value_H_i = compare_value_i[15:0];
	assign compare_value_L_i = compare_value_i[31:16];
	
	always @(posedge clk_i) begin
		compare_o[0] <= value_i > compare_value_H_i;
		compare_o[1] <= value_i < compare_value_L_i;
	end
endmodule

module Debounce(clk, in, out);
	parameter N = 2;
	parameter POLARITY = 0;
	
	input wire clk;
	input wire in;
	output reg out;
	
	reg [N-1:0] buffer;
	initial buffer = {N{!POLARITY}};
	initial out = !POLARITY;
	
	always @(posedge clk) begin
		buffer <= {buffer[N-2:0], in};
		if(POLARITY) out <= &buffer;
		else out <= |buffer;
	end
endmodule

module MovingAverage(clk, enable, in, out);
	parameter N = 4;
	parameter WIDTH = 16;
	
	input wire clk;
	input wire enable;
	input wire [WIDTH-1:0] in;
	output wire [WIDTH-1:0] out;
	
	localparam N_WIDTH = $clog2(N);
	
	integer i;
	reg [WIDTH-1:0] buffer [N-2:0];
	always @(posedge clk) begin
		if(enable) begin
			buffer[0] <= in;
			for (i = N-2; i > 0; i = i - 1)
				buffer[i] <= buffer[i-1];
		end
	end
	
	reg [WIDTH-1+N_WIDTH:0] sum [N-1:0];
	always @(*) begin
		for (i = 0; i<N; i = i + 1) begin
			if (i == 0)
				sum[i] = {{N_WIDTH{in[WIDTH-1]}}, in};
			else
				sum[i] = sum[i-1] + {{N_WIDTH{buffer[i-1][WIDTH-1]}}, buffer[i-1]};
		end
	end
	
	assign out = sum[N-1] >> N_WIDTH;
endmodule

module Sync_latch_input(clk_i, in, out, reset_i, set_i); 
	parameter OUT_POLARITY = 0; 
	parameter STEPS = 2; 
	localparam STEPS_INT = (STEPS < 2) ? 2 : STEPS; 
	input clk_i; 
	input in; 
	output reg out; 
	input reset_i; 
	input set_i; 

	reg [STEPS_INT-1:0] in_r; 
	reg in_latch; 
	always @(posedge clk_i or posedge in) begin
		if(in) 
			in_latch <= 1'b1; 
		else if(in_r[0]) 
			in_latch <= 1'b0;
	end 
 
	always @(posedge clk_i) begin 
		in_r <= {in_latch, in_r[STEPS_INT-1:1]}; 
		if(OUT_POLARITY) begin 
			if(reset_i) 
				out <= 1'b0;
			else if(in_r[1:0] == 2'b10 || set_i) 
				out <= 1'b1; 
		end 
		else begin 
			if(in_r[1:0] == 2'b10 || reset_i) 
				out <= 1'b0;	
			else if(set_i) 
				out <= 1'b1;				
		end 
	end 
 
	initial begin 
		in_latch = 0; 
		out = 0; 
		in_r = 0; 
	end 
endmodule 

module CLK_supply(clk_i, clk_o); 
 	parameter[63:0] SUPPLY_PERIOD = 20e6/50e3;
	localparam SUPPLY_WIDTH = $clog2(SUPPLY_PERIOD);
	parameter SUPPLY_RATE = 4;
	
	input clk_i;
	output reg clk_o;
	
	reg[SUPPLY_WIDTH-1:0] supply_counter;
	reg[SUPPLY_WIDTH+SUPPLY_RATE-1:0] supply_duty;
	
	always @(posedge clk_i) begin
		supply_counter <= supply_counter + 1'b1;
		clk_o <= supply_duty[SUPPLY_RATE +: SUPPLY_WIDTH] > supply_counter;

		if(supply_counter >= SUPPLY_PERIOD - 1'b1) begin
			supply_counter <= {SUPPLY_WIDTH{1'b0}};

		if(supply_duty[SUPPLY_RATE +: SUPPLY_WIDTH] < (SUPPLY_PERIOD>>1))
			supply_duty <= supply_duty + 1'b1;
		end
	end
endmodule

module Multiplier_signed(clk_i, start_i, A_i, B_i, Y_o, rdy_o); 
	parameter WIDTH_A = 4; 
	parameter WIDTH_B = 4;
	localparam WIDTH_Y = WIDTH_A + WIDTH_B; 
	localparam WIDTH_C = $clog2(WIDTH_B);
/////////////////////////////////////////////////////////////////////

	input clk_i;
	input start_i;
	
	input[WIDTH_A-1:0] A_i;
	input[WIDTH_B-1:0] B_i; 
	output reg[WIDTH_Y-1:0] Y_o;
	output reg rdy_o; 
	
/////////////////////////////////////////////////////////////////////
 
	reg[WIDTH_Y-1:0] A_r;
	reg[WIDTH_B-1:0] B_r;
	reg[WIDTH_C-1:0] counter;
	always @(posedge clk_i) begin
		counter <= counter + 1'b1; 
		A_r <= {A_r[WIDTH_Y-2:0], 1'b0};
		B_r <= {1'b0, B_r[WIDTH_B-1:1]}; 
		if(B_r[0]) 
			Y_o <= Y_o + A_r; 
		if(counter == {WIDTH_C{1'b1}}) 
			rdy_o <= 1'b1; 
				
		if(start_i) begin
			rdy_o <= 1'b0;
			Y_o <= {WIDTH_Y{1'b0}}; 
			counter <= {WIDTH_C{1'b0}};
			A_r <= {{WIDTH_B{A_i[WIDTH_A-1]}}, A_i};
			B_r <= B_i;
		end
	end
	
	initial begin 
		Y_o = 0;
		rdy_o = 0; 
		counter = 0;
	end

endmodule 

module EMIF_RX_reg(D, SP, CK, RST, Q);
	parameter WIDTH = 32;
	parameter INIT_VAL = 32'b0;
	
	input[WIDTH-1:0] D;
	input SP;
	input CK;
	input RST;
	output[WIDTH-1:0] Q;
	
	generate
		genvar i;
		for (i = 0; i < WIDTH; i = i + 1)
			if(INIT_VAL[i])
				FD1P3BX Reg(.D(D[i]), .SP(SP), .CK(CK), .PD(RST), .Q(Q[i]));
			else
				FD1P3DX Reg(.D(D[i]), .SP(SP), .CK(CK), .CD(RST), .Q(Q[i]));
	endgenerate 
	
endmodule