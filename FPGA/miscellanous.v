`include "global.v" 

module Temperature_FPGA(clk_i, temperature_FPGA_o);
	input clk_i;
	output reg[15:0] temperature_FPGA_o;
	
	reg[15:0] enable_counter;
	wire[7:0] DTR_OUT;
	DTR DTR(
	.STARTPULSE(enable_counter[15]),
	.DTROUT7(DTR_OUT[7]),
	.DTROUT6(DTR_OUT[6]),
	.DTROUT5(DTR_OUT[5]),
	.DTROUT4(DTR_OUT[4]),
	.DTROUT3(DTR_OUT[3]),
	.DTROUT2(DTR_OUT[2]),
	.DTROUT1(DTR_OUT[1]),
	.DTROUT0(DTR_OUT[0])
	);

	reg[15:0] temperature_FPGA_conversion;
	always @(*) begin
		case(DTR_OUT)
			{2'b10, 6'd0} : temperature_FPGA_conversion = -16'd58;
			{2'b10, 6'd1} : temperature_FPGA_conversion = -16'd56;
			{2'b10, 6'd2} : temperature_FPGA_conversion = -16'd54;
			{2'b10, 6'd3} : temperature_FPGA_conversion = -16'd52;
			{2'b10, 6'd4} : temperature_FPGA_conversion = -16'd45;
			{2'b10, 6'd5} : temperature_FPGA_conversion = -16'd44;
			{2'b10, 6'd6} : temperature_FPGA_conversion = -16'd43;
			{2'b10, 6'd7} : temperature_FPGA_conversion = -16'd42;
			{2'b10, 6'd8} : temperature_FPGA_conversion = -16'd41;
			{2'b10, 6'd9} : temperature_FPGA_conversion = -16'd40;
			{2'b10, 6'd10} : temperature_FPGA_conversion = -16'd39;
			{2'b10, 6'd11} : temperature_FPGA_conversion = -16'd38;
			{2'b10, 6'd12} : temperature_FPGA_conversion = -16'd37;
			{2'b10, 6'd13} : temperature_FPGA_conversion = -16'd36;
			{2'b10, 6'd14} : temperature_FPGA_conversion = -16'd30;
			{2'b10, 6'd15} : temperature_FPGA_conversion = -16'd20;
			{2'b10, 6'd16} : temperature_FPGA_conversion = -16'd10;
			{2'b10, 6'd17} : temperature_FPGA_conversion = -16'd4;
			{2'b10, 6'd18} : temperature_FPGA_conversion = 16'd0;
			{2'b10, 6'd19} : temperature_FPGA_conversion = 16'd4;
			{2'b10, 6'd20} : temperature_FPGA_conversion = 16'd10;
			{2'b10, 6'd21} : temperature_FPGA_conversion = 16'd21;
			{2'b10, 6'd22} : temperature_FPGA_conversion = 16'd22;
			{2'b10, 6'd23} : temperature_FPGA_conversion = 16'd23;
			{2'b10, 6'd24} : temperature_FPGA_conversion = 16'd24;
			{2'b10, 6'd25} : temperature_FPGA_conversion = 16'd25;
			{2'b10, 6'd26} : temperature_FPGA_conversion = 16'd26;
			{2'b10, 6'd27} : temperature_FPGA_conversion = 16'd27;
			{2'b10, 6'd28} : temperature_FPGA_conversion = 16'd28;
			{2'b10, 6'd29} : temperature_FPGA_conversion = 16'd29;
			{2'b10, 6'd30} : temperature_FPGA_conversion = 16'd40;
			{2'b10, 6'd31} : temperature_FPGA_conversion = 16'd50;
			{2'b10, 6'd32} : temperature_FPGA_conversion = 16'd60;
			{2'b10, 6'd33} : temperature_FPGA_conversion = 16'd70;
			{2'b10, 6'd34} : temperature_FPGA_conversion = 16'd76;
			{2'b10, 6'd35} : temperature_FPGA_conversion = 16'd80;
			{2'b10, 6'd36} : temperature_FPGA_conversion = 16'd81;
			{2'b10, 6'd37} : temperature_FPGA_conversion = 16'd82;
			{2'b10, 6'd38} : temperature_FPGA_conversion = 16'd83;
			{2'b10, 6'd39} : temperature_FPGA_conversion = 16'd84;
			{2'b10, 6'd40} : temperature_FPGA_conversion = 16'd85;
			{2'b10, 6'd41} : temperature_FPGA_conversion = 16'd86;
			{2'b10, 6'd42} : temperature_FPGA_conversion = 16'd87;
			{2'b10, 6'd43} : temperature_FPGA_conversion = 16'd88;
			{2'b10, 6'd44} : temperature_FPGA_conversion = 16'd89;
			{2'b10, 6'd45} : temperature_FPGA_conversion = 16'd95;
			{2'b10, 6'd46} : temperature_FPGA_conversion = 16'd96;
			{2'b10, 6'd47} : temperature_FPGA_conversion = 16'd97;
			{2'b10, 6'd48} : temperature_FPGA_conversion = 16'd98;
			{2'b10, 6'd49} : temperature_FPGA_conversion = 16'd99;
			{2'b10, 6'd50} : temperature_FPGA_conversion = 16'd100;
			{2'b10, 6'd51} : temperature_FPGA_conversion = 16'd101;
			{2'b10, 6'd52} : temperature_FPGA_conversion = 16'd102;
			{2'b10, 6'd53} : temperature_FPGA_conversion = 16'd103;
			{2'b10, 6'd54} : temperature_FPGA_conversion = 16'd104;
			{2'b10, 6'd55} : temperature_FPGA_conversion = 16'd105;
			{2'b10, 6'd56} : temperature_FPGA_conversion = 16'd106;
			{2'b10, 6'd57} : temperature_FPGA_conversion = 16'd107;
			{2'b10, 6'd58} : temperature_FPGA_conversion = 16'd108;
			{2'b10, 6'd59} : temperature_FPGA_conversion = 16'd116;
			{2'b10, 6'd60} : temperature_FPGA_conversion = 16'd120;
			{2'b10, 6'd61} : temperature_FPGA_conversion = 16'd124;
			{2'b10, 6'd62} : temperature_FPGA_conversion = 16'd128;
			{2'b10, 6'd63} : temperature_FPGA_conversion = 16'd132;
			default : temperature_FPGA_conversion = -16'd60;
		endcase
	end
	
	always @(posedge clk_i) begin
		enable_counter <= enable_counter + 1'b1;
		if(!enable_counter[15] & DTR_OUT[7]) temperature_FPGA_o <= temperature_FPGA_conversion;
	end
	
	initial enable_counter = 0;
	initial temperature_FPGA_o = 0;
endmodule
	
module PulseLengthCounter(clk_i, signal_i, length_pos, length_neg);
	parameter COUNT_WIDTH = 16;
	parameter FILTER_BITS = 8;
    input wire clk_i;
    input wire signal_i;
    output [COUNT_WIDTH-1:0] length_pos;
    output [COUNT_WIDTH-1:0] length_neg;

	reg [COUNT_WIDTH-1:0] count;
	reg signal_i_last;
	reg [COUNT_WIDTH+FILTER_BITS-1:0] filter_pos;
	reg [COUNT_WIDTH+FILTER_BITS-1:0] filter_neg;

	always @(posedge clk_i) begin
		signal_i_last <= signal_i;
		count <= count + 1'b1;
		
		if (signal_i ^ signal_i_last) begin
			count <= 0;
			if(signal_i_last) filter_pos <= filter_pos + count - filter_pos[FILTER_BITS +: COUNT_WIDTH];
			else filter_neg <= filter_neg + count - filter_neg[FILTER_BITS +: COUNT_WIDTH];
		end
	end

	initial begin
		count = 0;
		signal_i_last = 0;
		filter_pos = 0;
		filter_neg = 0;
	end
	
	assign length_pos = filter_pos[FILTER_BITS +: COUNT_WIDTH];
	assign length_neg = filter_neg[FILTER_BITS +: COUNT_WIDTH];
endmodule

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