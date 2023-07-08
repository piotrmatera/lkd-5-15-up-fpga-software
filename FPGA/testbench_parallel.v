`include "global.v"

`define TX2_FM  `C+20 
`define TX1_FM  `B+19 
 
`define RX2_FM  `D+17 
`define RX1_FM  `C+18

module SerDes_tb2;
	reg XTAL1_25MHz_i, XTAL2_25MHz_i, XTAL3_25MHz_i;
	reg XTAL1_20MHz_i;

	GSR GSR_INST (.GSR (1'b1));
	PUR PUR_INST (.PUR (1'b0));

	reg[15:0] i;

	always begin
		i = 0;
		#100
		forever begin
			#20
			i <= i+1;
			if(i>=20000) $stop;
		end
	end

	wire[400:1] Master_FIO;
	wire[400:1] Slave1_FIO;
	wire[400:1] Slave2_FIO;
	wire[400:1] Slave3_FIO;
	wire[168:0] Master_CIO;
	wire[168:0] Slave1_CIO;
	wire[168:0] Slave2_CIO;
	wire[168:0] Slave3_CIO;

	assign Master_FIO[`G+2] = XTAL1_25MHz_i;
	assign Slave1_FIO[`G+2] = XTAL2_25MHz_i;
	assign Slave2_FIO[`G+2] = XTAL3_25MHz_i;
	assign Slave3_FIO[`G+2] = XTAL1_25MHz_i;
	assign Master_FIO[`G+3] = XTAL1_20MHz_i;
	assign Slave1_FIO[`G+3] = XTAL1_20MHz_i;
	assign Slave2_FIO[`G+3] = XTAL1_20MHz_i;
	assign Slave3_FIO[`G+3] = XTAL1_20MHz_i;

	wire[1:0] Slave1_tx;
	wire[1:0] Slave1_rx;
	wire[1:0] Slave2_tx;
	wire[1:0] Slave2_rx;
	wire[1:0] Slave3_tx;
	wire[1:0] Slave3_rx;
	wire[1:0] Master_tx;
	wire[1:0] Master_rx;

	assign Slave3_tx = {Slave3_FIO[`TX2_FM], Slave3_FIO[`TX1_FM]};
	assign {Slave3_FIO[`RX2_FM], Slave3_FIO[`RX1_FM]} = Slave3_rx;
	assign Slave2_tx = {Slave2_FIO[`TX2_FM], Slave2_FIO[`TX1_FM]};
	assign {Slave2_FIO[`RX2_FM], Slave2_FIO[`RX1_FM]} = Slave2_rx;
	assign Slave1_tx = {Slave1_FIO[`TX2_FM], Slave1_FIO[`TX1_FM]};
	assign {Slave1_FIO[`RX2_FM], Slave1_FIO[`RX1_FM]} = Slave1_rx;
	assign Master_tx = {Master_FIO[`TX2_FM], Master_FIO[`TX1_FM]};
	assign {Master_FIO[`RX2_FM], Master_FIO[`RX1_FM]} = Master_rx;

	assign Master_rx[0] = ~1'b0;
	
	assign Master_rx[1] = ~Slave1_tx[0];
	assign Slave1_rx[0] = ~Master_tx[1];
	
	assign Slave1_rx[1] = ~1'b0;
	/*
	assign Slave1_rx[1] = ~Slave2_tx[0];
	assign Slave2_rx[0] = ~Slave1_tx[1];
	
	assign Slave2_rx[1] = ~Slave3_tx[0];
	assign Slave3_rx[0] = ~Slave2_tx[1];
	
	assign Slave3_rx[1] = ~1'b0;
*/
	top_ACDC top_ACDC0(.CPU_io(Master_CIO), .FPGA_io(Master_FIO));

	top_ACDC top_ACDC1(.CPU_io(Slave1_CIO), .FPGA_io(Slave1_FIO));

	//top_ACDC top_ACDC2(.CPU_io(Slave2_CIO), .FPGA_io(Slave2_FIO));

	//top_ACDC top_ACDC3(.CPU_io(Slave3_CIO), .FPGA_io(Slave3_FIO));

	always begin
		XTAL1_25MHz_i = 0;
		forever
			#(20*(1+0e-6)) XTAL1_25MHz_i = !XTAL1_25MHz_i;
	end

	always begin
		XTAL2_25MHz_i = 0;
		forever
			#(20*(1-0e-6)) XTAL2_25MHz_i = !XTAL2_25MHz_i;
	end

	always begin
		XTAL3_25MHz_i = 0;
		forever
			#(20*(1+50e-6)) XTAL3_25MHz_i = !XTAL3_25MHz_i;
	end

	always begin
		XTAL1_20MHz_i = 0;
		forever
			#(25*(1+50e-6)) XTAL1_20MHz_i = !XTAL1_20MHz_i;
	end

endmodule

