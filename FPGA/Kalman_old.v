`timescale 1ns/1ps

module Kalman2(clk_i, Mem1_data_i, Mem1_addrw_i, Mem1_we_i, Mem1_clk_w, Mem1_clk_en_w, enable_i, Mem0_addrw_o, Mem0_we_o, Mem0_data_io, WIP_flag_o, CIN, SIGNEDCIN, CO, SIGNEDCO);
	GSR GSR_INST (.GSR (1'b1));
	PUR PUR_INST (.PUR (1'b0));

	parameter HARMONICS_NUM = 20;
	parameter IN_SERIES_NUM = 4;
	
	localparam SERIES_CNT_WIDTH = $clog2(IN_SERIES_NUM);
	localparam HARMONIC_MEM_WIDTH = $clog2(HARMONICS_NUM*2+2);
	localparam M0_ADDR_WIDTH = 9;
	localparam M0_ADDR_NUM = 2**M0_ADDR_WIDTH;
	localparam HARMONIC_CNT_WIDTH = $clog2(HARMONICS_NUM);
	localparam M0_SUM = (2**HARMONIC_MEM_WIDTH)-1;
	localparam M0_X1 = 0; 
	localparam M0_X2 = 1;
	localparam M1_ADDR_WIDTH = 9;
	localparam M1_ADDR_NUM = 2**M1_ADDR_WIDTH;
	localparam M1_COS = 0;
	localparam M1_SIN = 1;
	localparam M1_K1 = 2;
	localparam M1_K2 = 3;
	localparam M1_INPUT = M1_ADDR_NUM-IN_SERIES_NUM;
	localparam SEL_STATES = 0;
	localparam SEL_COMMON = 1;

	localparam OPCODE_WIDTH = 4;
	localparam OPCODE_SUM_A_B_C = 4'b0100;
	localparam OPCODE_SUM_A_NB_C = 4'b0101; 
	localparam OPCODE_SUM_A_B_NC = 4'b0110;
	localparam OPCODE_SUM_A_NB_NC = 4'b0111; 
	localparam OPCODE_XNOR_BC = 4'b1100; 
	localparam OPCODE_XOR_BC = 4'b1110; 
	localparam OPCODE_NAND_BC = 4'b0000; 
	localparam OPCODE_AND_BC = 4'b1000; 
	localparam OPCODE_OR_BC = 4'b0011; 
	localparam OPCODE_NOR_BC = 4'b1011; 

	localparam AMUX_WIDTH = 2;
	localparam AMUX_ALU_FB = 2'b00; 
	localparam AMUX_MULTA = 2'b01; 
	localparam AMUX_A_ALU = 2'b10; 
	localparam AMUX_GND = 2'b11; 

	localparam BMUX_WIDTH = 2;
	localparam BMUX_MULTB_L18 = 2'b00; 
	localparam BMUX_MULTB = 2'b01;
	localparam BMUX_B_ALU = 2'b10; 
	localparam BMUX_GND = 2'b11; 

	localparam CMUX_WIDTH = 3; 
	localparam CMUX_GND = 3'b000; 
	localparam CMUX_CIN_R18 = 3'b001;
	localparam CMUX_CIN = 3'b010; 
	localparam CMUX_C_ALU = 3'b011; 
	localparam CMUX_A_ALU = 3'b100; 
	localparam CMUX_ALU_FB = 3'b101; 
	localparam CMUX_RND_PN = 3'b110; 
	localparam CMUX_RND_PNM = 3'b111; 

	localparam AAMEM_WIDTH = 2; 
	localparam AAMEM_M0L = 2'b00;
	localparam AAMEM_M0H = 2'b01;
	localparam AAMEM_M1L = 2'b10;
	localparam AAMEM_M1H = 2'b11;

	localparam ABMEM_WIDTH = 2; 
	localparam ABMEM_M0L = 2'b00; 
	localparam ABMEM_M0H = 2'b01; 
	localparam ABMEM_M1L = 2'b10; 
	localparam ABMEM_M1H = 2'b11; 

	localparam BAMEM_WIDTH = 2; 
	localparam BAMEM_M0L = 2'b00; 
	localparam BAMEM_M0H = 2'b01;
	localparam BAMEM_M1L = 2'b10;
	localparam BAMEM_M1H = 2'b11;

	localparam BBMEM_WIDTH = 2;
	localparam BBMEM_M0L = 2'b00; 
	localparam BBMEM_M0H = 2'b01; 
	localparam BBMEM_M1L = 2'b10; 
	localparam BBMEM_M1H = 2'b11; 

	localparam CMEM_WIDTH = 1; 
	localparam CMEM_M0 = 1'b0; 
	localparam CMEM_M1 = 1'b1; 
	input clk_i;
	input [31:0] Mem1_data_i;
	input [M1_ADDR_WIDTH-1:0] Mem1_addrw_i;
	input Mem1_we_i;
	input Mem1_clk_w;
	input Mem1_clk_en_w;
	input enable_i;
	output [M0_ADDR_WIDTH-1:0] Mem0_addrw_o;
	output Mem0_we_o;
	output [35:0] Mem0_data_io;
	output WIP_flag_o;
	input wire [53:0] CIN;
	input wire SIGNEDCIN;
	output wire [53:0] CO;
	output wire SIGNEDCO;
	
	wire wip_flag;
	reg[OPCODE_WIDTH-1:0]Opcode;
	wire[OPCODE_WIDTH-1:0]Opcode_pip;
	reg[AMUX_WIDTH-1:0]AMuxsel;
	wire[AMUX_WIDTH-1:0]AMuxsel_pip;
	reg[BMUX_WIDTH-1:0]BMuxsel;
	wire[BMUX_WIDTH-1:0]BMuxsel_pip;
	reg[CMUX_WIDTH-1:0]CMuxsel;
	wire[CMUX_WIDTH-1:0]CMuxsel_pip;
	reg[AAMEM_WIDTH-1:0]AAMemsel;
	wire[AAMEM_WIDTH-1:0]AAMemsel_pip;
	reg[ABMEM_WIDTH-1:0]ABMemsel;
	wire[ABMEM_WIDTH-1:0]ABMemsel_pip;
	reg[BAMEM_WIDTH-1:0]BAMemsel;
	wire[BAMEM_WIDTH-1:0]BAMemsel_pip;
	reg[BBMEM_WIDTH-1:0]BBMemsel;
	wire[BBMEM_WIDTH-1:0]BBMemsel_pip;
	reg[CMEM_WIDTH-1:0]CMemsel;
	wire[CMEM_WIDTH-1:0]CMemsel_pip;
	
	reg [4:0] cnt;
	
	reg [SERIES_CNT_WIDTH-1:0] series_cnt;

	wire [35:0] Mem0_data_i;
	wire [31:0] Mem1_data_i;
	wire [35:0] Mem0_data_o;
	wire [35:0] Mem1_data_o;
	
	reg [M0_ADDR_WIDTH-1:0] Mem0_addrw;
	wire [M0_ADDR_WIDTH-1:0] Mem0_addrw_pip;
	reg Mem0_we;
	wire Mem0_we_pip;
	
	
	reg [35:0] error;
	
	reg SignAA;
	wire SignAA_pip;
	reg SignAB;
	wire SignAB_pip;
	reg SignBA;
	wire SignBA_pip;
	reg SignBB;
	wire SignBB_pip;

	reg [HARMONIC_CNT_WIDTH-1:0] harmonics_cnt;
	
    reg CE1;
	wire CE1_pip;
	
	reg addrr_M0_sel;
	reg addrr_M0_inc;
	reg addrr_M0_rst;
	reg [1:0] addrr_M0_off;
	wire [6:0] addrr_M0_out;
	
	reg addrw_M0_sel;
	reg addrw_M0_inc;
	reg addrw_M0_rst;
	reg [1:0] addrw_M0_off;
	wire [6:0] addrw_M0_out;
	
	reg addrr_M1_sel;
	reg addrr_M1_inc;
	reg addrr_M1_rst;
	reg [1:0] addrr_M1_off;
	wire [M1_ADDR_WIDTH-1:0] addrr_M1_out;
	
    //Inicjalizacja wartosci sygnalow i rejestrow w celu przeprowadzenia testow
	initial begin
		cnt = 0;
		Opcode = OPCODE_SUM_A_B_C;
		AMuxsel = AMUX_GND;
		BMuxsel = BMUX_GND;
		CMuxsel = CMUX_GND;
		AAMemsel = 0;
		ABMemsel = 0;
		BAMemsel = 0;
		BBMemsel = 0;
		CMemsel = 0;
		SignAA = 0;
		SignAB = 0;
		SignBA = 0;
		SignBB = 0;	
		harmonics_cnt = 1;
		CE1 = 0;
		addrr_M0_sel = 0;
		addrr_M0_inc = 0;
		addrr_M0_rst = 0;
		addrr_M0_off = 0;
		addrr_M1_sel = 0;
		addrr_M1_inc = 0;
		addrr_M1_rst = 0;
		addrr_M1_off = 0;
		addrw_M0_sel = 0;
		addrw_M0_inc = 0;
		addrw_M0_rst = 0;
		addrw_M0_off = 0;
		Mem0_we = 0;
		error = 0;
		series_cnt = 0;
        $dumpfile("dump.vcd");
		$dumpvars(2,Kalman2);
	end

	//Maszyna stanow odpowiedzialna za wykonanie operacji aryrmetycznych
	always @(posedge clk_i) begin
		if(wip_flag) begin
			cnt <= cnt + 1'b1;
		end
		case(cnt[4:0])
			0: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= 0;
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b1;
								
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0L;
				BBMemsel <= BBMEM_M0L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b0;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_GND;
				BMuxsel <= BMUX_GND;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end 
			1: begin
				Mem0_we <= 1'b0;
			end
			2: begin
				addrr_M0_inc <= 1'b1;
				addrr_M1_inc <= 1'b1;
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 1'b1;
			end
			3: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_SIN;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;
				
				Mem0_we <= 1'b0;
								
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M1H;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1H;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_GND;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			4: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_SIN;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;
				
				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_GND;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			5: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_COS;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M1H;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1H;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_NC;
				CE1 <= 1'b1;
			end
			6: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_COS;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= M0_X1;

				Mem0_we <= 1'b1;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_GND;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			7: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= 0;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= 0;
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b1;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0L;
				BBMemsel <= BBMEM_M0L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b0;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_GND;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			8: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_SIN;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M1H;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1H;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_GND;
			
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			9: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_SIN;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_GND;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end 
			10: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_COS;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;
				
				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M1H;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1H;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			11: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_COS;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= M0_X2;

				Mem0_we <= 1'b1;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_GND;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
				
				if(harmonics_cnt < HARMONICS_NUM) begin
					addrr_M0_inc <= 1'b1;
					addrr_M1_inc <= 1'b1;
					addrw_M0_inc <= 1'b1;
					
					cnt <= 5'd3;
					harmonics_cnt <= harmonics_cnt + 1'b1;
				end
				else begin
					addrr_M0_rst <= 1'b1;
					addrr_M1_rst <= 1'b1;
					addrw_M0_rst <= 1'b1;
					
					harmonics_cnt <= 0;
				end
			end
//-------------------------------------------------
			12: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= 0;
				
				addrr_M1_sel <= SEL_COMMON;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= 0;
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_inc <= 1'b1;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0L;
				BBMemsel <= BBMEM_M0L;
				CMemsel <= CMEM_M1;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b0;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_GND;
				BMuxsel <= BMUX_GND;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			13: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 1'b1;
				addrr_M0_off <= 0;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= 0;
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b1;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0L;
				BBMemsel <= BBMEM_M0L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b0;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_GND;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_NC;
				CE1 <= 1'b1;
			end
			14: begin
				addrr_M0_rst <= 0;
				addrr_M1_rst <= 0;
				addrw_M0_rst <= 0;
			    AMuxsel <= AMUX_GND;
				BMuxsel <= BMUX_GND;
				CMuxsel <= CMUX_GND;
				Mem0_we <= 1'b0;
			end
			15: begin
			end
			16: begin
			end
			17: begin
			end
			18: begin
			end
            19: begin
                error <= Mem0_data_i;
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_inc <= 1'b1;
				addrr_M0_rst <= 0;
				addrr_M0_off <= 0;
            end
//-------------------------------------------------
			20: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= 0;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_K1;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M1H;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1H;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b1;
				SignBA <= 1'b1;
				SignBB <= 1'b1;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_GND;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			21: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_K1;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= M0_X1;

				Mem0_we <= 1'b1;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b0;
			end
			22: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= 0;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_K2;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= 0;

				Mem0_we <= 1'b0;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M1H;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1H;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b1;
				SignBA <= 1'b1;
				SignBB <= 1'b1;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_GND;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b1;
			end
			23: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 0;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= M1_K2;
				
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 0;
				addrw_M0_off <= M0_X2;

				Mem0_we <= 1'b1;
				
				AAMemsel <= AAMEM_M0L;
				ABMemsel <= ABMEM_M0L;
				BAMemsel <= BAMEM_M0H;
				BBMemsel <= BBMEM_M1L;
				CMemsel <= CMEM_M0;
				
				SignAA <= 1'b0;
				SignAB <= 1'b0;
				SignBA <= 1'b1;
				SignBB <= 1'b0;
				
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				CE1 <= 1'b0;
				if(harmonics_cnt < HARMONICS_NUM) begin
					addrr_M0_inc <= 1'b1;
					addrr_M1_inc <= 1'b1;
					addrw_M0_inc <= 1'b1;
					
					cnt <= 5'd20;
					harmonics_cnt <= harmonics_cnt + 1'b1;
				end
				else begin
					addrr_M0_rst <= 1'b1;
					addrr_M0_inc <= 0;
					addrr_M1_rst <= 1'b1;
					addrr_M1_inc <= 0;
					addrw_M0_rst <= 1'b1;
					addrw_M0_inc <= 0;
					
					harmonics_cnt <= 6'd1;
				end
			end
			24: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_inc <= 0;
				addrr_M0_rst <= 1'b1;
				addrr_M0_off <= 0;
				
				addrr_M1_sel <= SEL_COMMON;
				addrr_M1_inc <= 0;
				addrr_M1_rst <= 0;
				addrr_M1_off <= 0;
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_inc <= 0;
				addrw_M0_rst <= 1'b1;
				addrw_M0_off <= 0;
				
				Mem0_we <= 1'b0;
			end
			25: begin
				addrr_M0_rst <= 0;
				addrr_M1_rst <= 0;
				addrw_M0_rst <= 0;
			end
			26: begin
                addrr_M1_sel <= SEL_COMMON;
                if(series_cnt<IN_SERIES_NUM-1) begin
                        addrr_M1_inc <= 1'b1;
                        series_cnt <= series_cnt + 1'b1;
                        cnt <= 5'd30;
                end
				else begin
                        addrr_M1_rst <= 1'b1;
                        series_cnt <= 0;
                end
			end
			27: begin
                addrr_M1_rst <= 0;
			end
			28: begin
			end
            29: begin
                cnt <= 0;
			end
			30: begin
                addrr_M1_inc <= 0;
            end
            31: begin
                cnt <= 0;
            end
		endcase
	end
	
	//Odwolanie do wczesniej skonfigurowanej instancji Slice'a
	Slice2 Slice2(.CLK0(clk_i), .CE0(1'b1), .CE1(CE1), .CE2(1'b0), .CE3(1'b0), 
	.RST0(1'b0), .Mem0(Mem0_data_o), .Mem1(Mem1_data_o), .AAMemsel(AAMemsel_pip), .ABMemsel(ABMemsel_pip), 
	.BAMemsel(BAMemsel_pip), .BBMemsel(BBMemsel_pip), .CMemsel(CMemsel_pip), .SignAA(SignAA_pip), .SignAB(SignAB_pip),
	.SignBA(SignBA_pip), .SignBB(SignBB_pip), .AMuxsel(AMuxsel_pip), .BMuxsel(BMuxsel_pip), .CMuxsel(CMuxsel_pip), 
	.Opcode(Opcode_pip), .Result(Mem0_data_i), .Result54(CO), .SIGNEDR(SIGNEDCO), .EQZ(), .EQZM(), .EQOM(), .EQPAT(), 
	.EQPATB(), .OVER(), .UNDER(), .CIN(CIN), .SIGNEDCIN(SIGNEDCIN), .CO());
	
	//Parametry pamieci
	pmi_ram_dp #(.pmi_wr_addr_depth(M0_ADDR_NUM), .pmi_wr_addr_width(M0_ADDR_WIDTH), .pmi_wr_data_width(36),
	.pmi_rd_addr_depth(M0_ADDR_NUM), .pmi_rd_addr_width(M0_ADDR_WIDTH), .pmi_rd_data_width(36), .pmi_regmode("reg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file("../Mem0.mem"), .pmi_init_file_format("hex")
	)
	Mem0(.Data(Mem0_data_i), .WrAddress(Mem0_addrw_pip), .RdAddress({series_cnt,addrr_M0_out}), .WrClock(clk_i),
	.RdClock(clk_i), .WrClockEn(1'b1), .RdClockEn(1'b1), .WE(Mem0_we_pip), .Reset(1'b0), 
	.Q(Mem0_data_o));
	
	pmi_ram_dp #(.pmi_wr_addr_depth(M1_ADDR_NUM), .pmi_wr_addr_width(M1_ADDR_WIDTH), .pmi_wr_data_width(36),
	.pmi_rd_addr_depth(M1_ADDR_NUM), .pmi_rd_addr_width(M1_ADDR_WIDTH), .pmi_rd_data_width(36), .pmi_regmode("reg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file("../Mem1_K.mem"), .pmi_init_file_format("hex")
	)
	Mem1(.Data({Mem1_data_i,4'd0}), .WrAddress(Mem1_addrw_i), .RdAddress(addrr_M1_out), .WrClock(Mem1_clk_w),
	.RdClock(clk_i), .WrClockEn(Mem1_clk_en_w), .RdClockEn(1'b1), .WE(Mem1_we_i), .Reset(1'b0), 
	.Q(Mem1_data_o));
	
	addr_gen_Kalman2 #(.ADDR_NUM(2),.ADDR_START_STATES(0),.ADDR_START_COMMON(M0_SUM),.ADDR_INC_STATES(2'd2),
	.ADDR_INC_COMMON(1'b1),.OFFSET_WIDTH(2),.ADDR_WIDTH(7))
	addrr_M0_gen (.clk(clk_i),.addr_sel(addrr_M0_sel),.addr_inc(addrr_M0_inc),.addr_rst(addrr_M0_rst),
	.addr_off(addrr_M0_off),.addr_out(addrr_M0_out));
	
	addr_gen_Kalman2 #(.ADDR_NUM(2),.ADDR_START_STATES(0),.ADDR_START_COMMON(M0_SUM),.ADDR_INC_STATES(2'd2),
	.ADDR_INC_COMMON(1'b1),.OFFSET_WIDTH(2),.ADDR_WIDTH(7))
	addrw_M0_gen (.clk(clk_i),.addr_sel(addrw_M0_sel),.addr_inc(addrw_M0_inc),.addr_rst(addrw_M0_rst),
	.addr_off(addrw_M0_off),.addr_out(addrw_M0_out));
	
	addr_gen_Kalman2 #(.ADDR_NUM(2),.ADDR_START_STATES(0),.ADDR_START_COMMON(M1_INPUT),.ADDR_INC_STATES(3'd4),
	.ADDR_INC_COMMON(1'b1),.OFFSET_WIDTH(2),.ADDR_WIDTH(M1_ADDR_WIDTH))
	addrr_M1_gen (.clk(clk_i),.addr_sel(addrr_M1_sel),.addr_inc(addrr_M1_inc),.addr_rst(addrr_M1_rst),
	.addr_off(addrr_M1_off),.addr_out(addrr_M1_out));
	
	pipeline_delay #(.WIDTH(1),.CYCLES(5),.SHIFT_MEM(0)) 
	we_delay (.clk(clk_i), .in(Mem0_we), .out(Mem0_we_pip));
	
	pipeline_delay #(.WIDTH(9),.CYCLES(5),.SHIFT_MEM(0)) 
	addrw_M0_delay (.clk(clk_i), .in({series_cnt,addrw_M0_out}), .out(Mem0_addrw_pip));

	pipeline_delay #(.WIDTH(OPCODE_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	opcode_delay (.clk(clk_i), .in(Opcode), .out(Opcode_pip));
	
	pipeline_delay #(.WIDTH(AMUX_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	amux_delay (.clk(clk_i), .in(AMuxsel), .out(AMuxsel_pip));
	
	pipeline_delay #(.WIDTH(BMUX_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	bmux_delay (.clk(clk_i), .in(BMuxsel), .out(BMuxsel_pip));
	
	pipeline_delay #(.WIDTH(CMUX_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	cmux_delay (.clk(clk_i), .in(CMuxsel), .out(CMuxsel_pip));
	
	pipeline_delay #(.WIDTH(AAMEM_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	aamem_delay (.clk(clk_i), .in(AAMemsel), .out(AAMemsel_pip));
	
	pipeline_delay #(.WIDTH(ABMEM_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	abmem_delay (.clk(clk_i), .in(ABMemsel), .out(ABMemsel_pip));
	
	pipeline_delay #(.WIDTH(BAMEM_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	bamem_delay (.clk(clk_i), .in(BAMemsel), .out(BAMemsel_pip));
	
	pipeline_delay #(.WIDTH(BBMEM_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	bbmem_delay (.clk(clk_i), .in(BBMemsel), .out(BBMemsel_pip));
	
	pipeline_delay #(.WIDTH(CMEM_WIDTH),.CYCLES(2),.SHIFT_MEM(0)) 
	cmem_delay (.clk(clk_i), .in(CMemsel), .out(CMemsel_pip));
	
	pipeline_delay #(.WIDTH(1),.CYCLES(2),.SHIFT_MEM(0)) 
	signaa_delay (.clk(clk_i), .in(SignAA), .out(SignAA_pip));
	
	pipeline_delay #(.WIDTH(1),.CYCLES(2),.SHIFT_MEM(0)) 
	signab_delay (.clk(clk_i), .in(SignAB), .out(SignAB_pip));
	
	pipeline_delay #(.WIDTH(1),.CYCLES(2),.SHIFT_MEM(0)) 
	signba_delay (.clk(clk_i), .in(SignBA), .out(SignBA_pip));
	
	pipeline_delay #(.WIDTH(1),.CYCLES(2),.SHIFT_MEM(0)) 
	signbb_delay (.clk(clk_i), .in(SignBB), .out(SignBB_pip));
	
	pipeline_delay #(.WIDTH(1),.CYCLES(2),.SHIFT_MEM(0)) 
	ce_delay (.clk(clk_i), .in(CE1), .out(CE1_pip));
	
	Sync_latch_input #(.OUT_POLARITY(1), .STEPS(2)) 
	code_start(.clk_i(clk_i), .in(enable_i), .out(wip_flag), .reset_i(cnt == 27), 
	.set_i(1'b0));
	
	//Przypisanie rejestrow do wyjsc modulu
    assign Mem0_data_io = Mem0_data_i;
	assign Mem0_addrw_o = Mem0_addrw_pip;
	assign Mem0_we_o = Mem0_we_pip;
	assign WIP_flag_o = wip_flag;
endmodule

module addr_gen_Kalman2(clk, addr_sel, addr_inc, addr_rst, addr_off, addr_out);
	parameter ADDR_NUM = 2;
	parameter ADDR_START_STATES = 9'd0;
	parameter ADDR_START_COMMON = 9'd0;
	parameter ADDR_INC_STATES = 1'b1;
	parameter ADDR_INC_COMMON = 1'b1;
	parameter OFFSET_WIDTH = 1;
	parameter ADDR_WIDTH = 9;

	input clk;
	input addr_sel;
	input addr_inc;
	input addr_rst;
	input [OFFSET_WIDTH-1:0] addr_off;
	output [ADDR_WIDTH-1:0] addr_out;
	
	reg [ADDR_NUM-1:0][ADDR_WIDTH-1:0]cnt;
	
	initial begin
		cnt[0] = ADDR_START_STATES;
		cnt[1] = ADDR_START_COMMON;
	end

	assign addr_out = cnt[addr_sel] + addr_off;
	
	always @(posedge clk) begin
		case(addr_sel)
			0: begin
				if(addr_inc) begin
					cnt[0] <= cnt[0] + ADDR_INC_STATES;
				end
				if(addr_rst) begin
					cnt[0] <= ADDR_START_STATES;
				end
			end
			1: begin
				if(addr_inc) begin
					cnt[1] <= cnt[1] + ADDR_INC_COMMON;
				end
				if(addr_rst) begin
					cnt[1] <= ADDR_START_COMMON;
				end
			end
		endcase
	end
endmodule
