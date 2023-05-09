// --------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<
// --------------------------------------------------------------------
// Copyright (c) 2011 by Lattice Semiconductor Corporation
// --------------------------------------------------------------------
//
// Permission:
//
// Lattice Semiconductor grants permission to use this code for use
// in synthesis for any Lattice programmable logic product. Other
// use of this code, including the selling or duplication of any
// portion is strictly prohibited.
//
// Disclaimer:
//
// This VHDL or Verilog source code is intended as a design reference
// which illustrates how these types of functions can be implemented.
// It is the user's responsibility to verify their design for
// consistency and functionality through the use of formal
// verification methods. Lattice Semiconductor provides no warranty
// regarding the use or functionality of this code.
//
// --------------------------------------------------------------------
//
// Lattice Semiconductor Corporation
// 5555 NE Moore Court
// Hillsboro, OR 97214
// U.S.A
//
// TEL: 1-800-Lattice (USA and Canada)
// 503-268-8001 (other locations)
//
// web: http://www.latticesemi.com/
// email: techsupport@latticesemi.com

`include "global.v"

module crc_generator(clk, rst, data_ena, crc, input_data);
	parameter INPUT_DATA_TRANSPOSE = 1;
	parameter CRC_TRANSPOSE = 1;
	parameter INPUT_DATA_COMPLEMENT = 0;
	parameter CRC_COMPLEMENT = 0;
	parameter INIT_VAL = 1;	
	parameter POLYNOMIAL = 'h8005;
	parameter INPUT_DATA_WIDTH = 8;
	parameter CRC_WIDTH = 16;

	input clk,rst,data_ena;
	output [CRC_WIDTH-1:0] crc;
	input [INPUT_DATA_WIDTH-1:0] input_data;
	wire [INPUT_DATA_WIDTH-1:0] data_buf;
	reg [CRC_WIDTH-1:0] crc_buf;

	function [CRC_WIDTH-1:0] crc_data1;
		input [CRC_WIDTH-1:0] crc;
		input data1;
		begin 
			crc_data1={crc[CRC_WIDTH-2:0],1'b0} ^ ({CRC_WIDTH{(crc[CRC_WIDTH-1]^data1)}} & POLYNOMIAL[CRC_WIDTH-1:0]);
		end
	endfunction

	function [CRC_WIDTH-1:0] crc_data;
		input [INPUT_DATA_WIDTH-1:0] data;
		input [CRC_WIDTH-1:0] crc;
		integer i;
		begin
			crc_data=crc;
			for(i=0;i<=INPUT_DATA_WIDTH-1;i=i+1) begin
				crc_data=crc_data1(crc_data,data[INPUT_DATA_WIDTH-1-i]);
			end
		end
	endfunction

	function [INPUT_DATA_WIDTH-1:0] transpose_input_data;
		input [INPUT_DATA_WIDTH-1:0] data;
		integer i;
		begin 
			for(i=0;i<=INPUT_DATA_WIDTH-1;i=i+1) begin
				transpose_input_data[i]=data[INPUT_DATA_WIDTH-1-i];
			end
		end
	endfunction

	function [CRC_WIDTH-1:0] transpose_crc;
		input [CRC_WIDTH-1:0] data;
		integer i;
		begin 
			for(i=0;i<=CRC_WIDTH-1;i=i+1) begin
				transpose_crc[i]=data[CRC_WIDTH-1-i];
			end
		end
	endfunction

	generate 
		if(INPUT_DATA_TRANSPOSE==0 && INPUT_DATA_COMPLEMENT==0)
			assign data_buf=input_data;
		else if (INPUT_DATA_TRANSPOSE==0 && INPUT_DATA_COMPLEMENT==1)
			assign data_buf=~input_data;
		else if (INPUT_DATA_TRANSPOSE==1 && INPUT_DATA_COMPLEMENT==0)
			assign data_buf=transpose_input_data(input_data);
		else if (INPUT_DATA_TRANSPOSE==1 && INPUT_DATA_COMPLEMENT==1)
			assign data_buf=~(transpose_input_data(input_data));
	endgenerate


	always @(posedge clk)
		if(rst)
			crc_buf <= {CRC_WIDTH{INIT_VAL[0]}};
		else if(data_ena)
			crc_buf <= crc_data(data_buf,crc_buf);
    
	generate 
		if(CRC_TRANSPOSE==0 && CRC_COMPLEMENT==0)
			assign crc=crc_buf;
		else if (CRC_TRANSPOSE==0 && CRC_COMPLEMENT==1)
			assign crc=~crc_buf;
		else if (CRC_TRANSPOSE==1 && CRC_COMPLEMENT==0)
			assign crc=transpose_crc(crc_buf);
		else if (CRC_TRANSPOSE==1 && CRC_COMPLEMENT==1)
			assign crc=~(transpose_crc(crc_buf));
	endgenerate
endmodule