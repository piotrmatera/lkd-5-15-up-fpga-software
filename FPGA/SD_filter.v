`include "global.v"

module SD_filter_sync_gen(clk_i, sync_pulse_i, decimator_pulse_o, select_o, new_value_o, new_value_early_o);
	parameter OSR = 200;

	localparam OSR_WIDTH = $clog2(OSR);

	input clk_i;
	input sync_pulse_i;
	output[1:0] decimator_pulse_o;
	output reg select_o;
	output reg new_value_o;
	output reg new_value_early_o;

	reg[3:0] sync_pulse_r;
	reg[1:0] change_counter;
	reg[OSR_WIDTH-1:0] OSR_counter[1:0];
	reg change_in_progress;
	reg change_done;

	initial begin
		change_done = 0;
		change_in_progress = 0;
		new_value_early_o = 0;
		new_value_o = 0;
		OSR_counter[0] = 0;
		OSR_counter[1] = 0;
		change_counter = 0;
		sync_pulse_r = 0;
		select_o = 0;
	end

	assign decimator_pulse_o = {OSR_counter[1] == 0, OSR_counter[0] == 0};
	assign OSR_change = sync_pulse_r[1:0] == 2'b10 && change_done;
	assign change_switch_rdy = change_counter == 2'b11;

	always @(posedge clk_i) begin
		new_value_o <= OSR_counter[select_o] == OSR-1;
		new_value_early_o <= OSR_counter[select_o] == OSR-3;

		sync_pulse_r <= {sync_pulse_i, sync_pulse_r[3:1]};

		if(OSR_change && select_o)
			OSR_counter[0] <= 4;
		else if(OSR_counter[0] == OSR-1)
			OSR_counter[0] <= 0;
		else
			OSR_counter[0] <= OSR_counter[0] + 1'b1;

		if(OSR_change && !select_o)
			OSR_counter[1] <= 4;
		else if(OSR_counter[1] == OSR-1)
			OSR_counter[1] <= 0;
		else
			OSR_counter[1] <= OSR_counter[1] + 1'b1;

		if(OSR_change)
			change_in_progress <= 1'b1;
		else if(change_switch_rdy)
			change_in_progress <= 1'b0;

		if(change_switch_rdy && !change_done && OSR_counter[!select_o] == OSR/2) begin
			select_o <= !select_o;
			change_done <= 1'b1;
		end
		else if(OSR_change)
			change_done <= 1'b0;

		if(OSR_change) begin
			change_counter <= 0;
		end
		else if(OSR_counter[!select_o] == 0 && !change_switch_rdy)
			change_counter <= change_counter + 1'b1;
	end
endmodule


module SD_filter_sync(data_i, clk_i, decimator_pulse_i, select_i, data_o);
	parameter OSR = 200;
	parameter ORDER = 2;
	parameter OUTPUT_WIDTH = 16;

	input clk_i;
	input data_i;
	input[1:0] decimator_pulse_i;
	input select_i;
	output reg[OUTPUT_WIDTH-1:0] data_o;

	wire[OUTPUT_WIDTH*2-1:0] data_o_temp;

	initial data_o = 0;
	always @(posedge clk_i)
		if(decimator_pulse_i[select_i])
			data_o <= select_i ? data_o_temp[OUTPUT_WIDTH+:OUTPUT_WIDTH] : data_o_temp[0+:OUTPUT_WIDTH];

	SD_filter #(.ORDER(ORDER), .OSR(OSR), .OUTPUT_WIDTH(OUTPUT_WIDTH), .OUTPUT_NUMBER(2), .ENABLE_OREG(0)) SD_filter(.data_i(data_i), .clk_i(clk_i), .decimator_pulse_i(decimator_pulse_i), .data_o(data_o_temp));
endmodule


module SD_filter(data_i, clk_i, decimator_pulse_i, data_o);
	parameter ORDER = 2;
	parameter OSR = 200;
	parameter OUTPUT_WIDTH = 16;
	parameter OUTPUT_NUMBER = 1;
	parameter ENABLE_OREG = 1;

	localparam WIDTH = $clog2((OSR**ORDER)+1)+1;

	input clk_i;
	input data_i;
	input[OUTPUT_NUMBER-1:0] decimator_pulse_i;
	output [(OUTPUT_WIDTH*OUTPUT_NUMBER)-1:0] data_o;

	genvar i;

	//
	// Integrate
	//

	reg [WIDTH-1:0] integrator[ORDER-1:0];

    for (i=0; i<ORDER; i=i+1)
		initial integrator[i] = 0;


	always @(posedge clk_i) begin
		if(data_i)
			integrator[0] <= integrator[0] + 1'b1;
		else
			integrator[0] <= integrator[0] - 1'b1;
	end

	for (i=1; i<ORDER; i=i+1)
		always @(posedge clk_i)
			integrator[i] <= integrator[i] + integrator[i-1];

	Decim_differ #(.ORDER(ORDER), .OSR(OSR), .OUTPUT_WIDTH(OUTPUT_WIDTH), .ENABLE_OREG(ENABLE_OREG)) Decim_differ1[OUTPUT_NUMBER-1:0] (.clk_i(clk_i), .integrator_i(integrator[ORDER-1]), .decimator_pulse_i(decimator_pulse_i), .data_o(data_o));
endmodule

module Decim_differ(clk_i, integrator_i, decimator_pulse_i, data_o);
	parameter ORDER = 2;
	parameter OSR = 200;
	parameter OUTPUT_WIDTH = 16;
	parameter ENABLE_OREG = 1;

	localparam WIDTH = $clog2((OSR**ORDER)+1)+1;
	localparam OUTPUT_SHIFT = (WIDTH - OUTPUT_WIDTH > 0) ? WIDTH - OUTPUT_WIDTH : 0;
	localparam OUTPUT_EXTEND = (OUTPUT_WIDTH - WIDTH > 0) ? OUTPUT_WIDTH - WIDTH : 0;

	input clk_i;
	input[WIDTH-1:0] integrator_i;
	input decimator_pulse_i;
	output [OUTPUT_WIDTH-1:0] data_o;

	genvar i;

	//
	// Differentiate
	//

	reg [WIDTH-1:0] differentiator[ORDER-1:0];
	wire [WIDTH-1:0] differentiator_w[ORDER:0];

	assign differentiator_w[0] = integrator_i;
	for (i=0; i<ORDER; i=i+1) begin
		assign differentiator_w[i+1] = differentiator_w[i] - differentiator[i];

		always @(posedge clk_i)
			if(decimator_pulse_i)
				differentiator[i] <= differentiator_w[i];

		initial differentiator[i] = 0;
	end

	if(ENABLE_OREG) begin
		reg [WIDTH-OUTPUT_SHIFT-1:0] data_o_reg;
		always @ (posedge clk_i)
			if(decimator_pulse_i)
				data_o_reg <= differentiator_w[ORDER][OUTPUT_SHIFT +: (WIDTH-OUTPUT_SHIFT)];

		initial data_o_reg = 0;
		assign data_o = { {OUTPUT_EXTEND{data_o_reg[WIDTH-OUTPUT_SHIFT-1]}},data_o_reg};
	end
	else begin
		wire [WIDTH-OUTPUT_SHIFT-1:0] data_o_reg;
		assign data_o_reg = differentiator_w[ORDER][OUTPUT_SHIFT +: (WIDTH-OUTPUT_SHIFT)];
		assign data_o = { {OUTPUT_EXTEND{data_o_reg[WIDTH-OUTPUT_SHIFT-1]}},data_o_reg};
	end


endmodule
