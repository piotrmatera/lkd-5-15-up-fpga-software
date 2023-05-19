`timescale 1ns/1ps

module Kalman(clk_i, Mem1_data_i, Mem1_addrw_i, Mem1_we_i, Mem1_clk_w, Mem1_clk_en_w, enable_i, Mem2_addrw_o, Mem2_we_o, Mem2_data_o, WIP_flag_o, CIN, SIGNEDCIN, CO, SIGNEDCO);
	parameter HARMONICS_NUM = 21;
	parameter IN_SERIES_NUM = 4;
	parameter DEBUG = 0;
		
	localparam SERIES_CNT_WIDTH = $clog2(IN_SERIES_NUM);
	localparam HARMONICS_CNT_WIDTH = $clog2(HARMONICS_NUM);
	
	localparam M0_ADDR_WIDTH = 9;
	localparam M0_ADDR_NUM = 2**M0_ADDR_WIDTH;	
	localparam M0_STATES_OFFSET_NUMBER = 9'd2;
	localparam M0_X1 = 0; 
	localparam M0_X2 = 1;
	
	localparam M0_COMMON_OFFSET_NUMBER = 9'd2;
	localparam M0_START_COMMON = M0_STATES_OFFSET_NUMBER*HARMONICS_NUM;
	localparam M0_SUM = 0;
	localparam M0_ERROR = 1;
	
	localparam M0_OFFSET_WIDTH = $clog2(M0_COMMON_OFFSET_NUMBER) > $clog2(M0_STATES_OFFSET_NUMBER) ? $clog2(M0_COMMON_OFFSET_NUMBER) : $clog2(M0_STATES_OFFSET_NUMBER);

	localparam M1_ADDR_WIDTH = 9;
	localparam M1_ADDR_NUM = 2**M1_ADDR_WIDTH;
	localparam M1_STATES_OFFSET_NUMBER = 9'd4;
	localparam M1_STATES_OFFSET_WIDTH = $clog2(M1_STATES_OFFSET_NUMBER);	
	localparam M1_COS = 0;
	localparam M1_SIN = 1;
	localparam M1_K1 = 2;
	localparam M1_K2 = 3;
	
	localparam M1_COMMON_OFFSET_NUMBER = 9'd1;
	localparam M1_START_COMMON = M1_STATES_OFFSET_NUMBER*HARMONICS_NUM;
	localparam M1_INPUT = 0;
	
	localparam M1_OFFSET_WIDTH = $clog2(M1_COMMON_OFFSET_NUMBER) > $clog2(M1_STATES_OFFSET_NUMBER) ? $clog2(M1_COMMON_OFFSET_NUMBER) : $clog2(M1_STATES_OFFSET_NUMBER);

	localparam SEL_WIDTH = 3;
	localparam SEL_STATES = 3'b000;
	localparam SEL_STATES_INC = 3'b001;
	localparam SEL_STATES_RST = 3'b010;
	localparam SEL_COMMON = 3'b100;
	localparam SEL_COMMON_INC = 3'b101;
	localparam SEL_COMMON_RST = 3'b110;

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

	localparam AAMEM_WIDTH = 4; 	
	localparam ABMEM_WIDTH = 4;
	localparam BAMEM_WIDTH = 4;
	localparam BBMEM_WIDTH = 4;
	localparam CMEM_WIDTH = 1; 
	localparam MEM_M0L = 4'b0000;
	localparam MEM_M0H = 4'b0001;
	localparam MEM_M1L = 4'b0010;
	localparam MEM_M1H = 4'b0011;
	localparam MEM_RM0L = 4'b0100;
	localparam MEM_RM0H = 4'b0101;
	localparam MEM_RM1L = 4'b0110;
	localparam MEM_RM1H = 4'b0111;
	localparam MEM_M0LS = 4'b1000;
	localparam MEM_M0HS = 4'b1001;
	localparam MEM_M1LS = 4'b1010;
	localparam MEM_M1HS = 4'b1011;
	localparam MEM_RM0LS = 4'b1100;
	localparam MEM_RM0HS = 4'b1101;
	localparam MEM_RM1LS = 4'b1110;
	localparam MEM_RM1HS = 4'b1111;
	
	localparam MEM_M0 = 1'b0; 
	localparam MEM_M1 = 1'b1; 
	
	input clk_i;
	
	input [31:0] Mem1_data_i;
	input [M1_ADDR_WIDTH-1:0] Mem1_addrw_i;
	input Mem1_we_i;
	input Mem1_clk_w;
	input Mem1_clk_en_w;
	
	output [M0_ADDR_WIDTH-1:0] Mem2_addrw_o;
	output Mem2_we_o;
	output [35:0] Mem2_data_o;
	
	input enable_i;
	output WIP_flag_o;
	
	input wire [53:0] CIN;
	input wire SIGNEDCIN;
	output wire [53:0] CO;
	output wire SIGNEDCO;


	reg [4:0] cnt;
	reg [SERIES_CNT_WIDTH-1:0] series_cnt;
	reg [HARMONICS_CNT_WIDTH-1:0] harmonics_cnt;
	
	reg[OPCODE_WIDTH-1:0]Opcode;
	reg[AMUX_WIDTH-1:0]AMuxsel;
	reg[BMUX_WIDTH-1:0]BMuxsel;
	reg[CMUX_WIDTH-1:0]CMuxsel;
	reg[AAMEM_WIDTH-1:0]AAMemsel;
	reg[ABMEM_WIDTH-1:0]ABMemsel;
	reg[BAMEM_WIDTH-1:0]BAMemsel;
	reg[BBMEM_WIDTH-1:0]BBMemsel;
	reg[CMEM_WIDTH-1:0]CMemsel;
    reg CE1;
			
	wire [35:0] Mem0_data_i;
	reg Mem0_we;				
	reg [SEL_WIDTH-1:0] addrw_M0_sel;
	reg [M0_OFFSET_WIDTH-1:0] addrw_M0_off;
	wire [M0_ADDR_WIDTH-1:0] addrw_M0_out;
	
	reg [SEL_WIDTH-1:0] addrr_M0_sel;
	reg [M0_OFFSET_WIDTH-1:0] addrr_M0_off;
	wire [M0_ADDR_WIDTH-1:0] addrr_M0_out;
	wire [35:0] Mem0_data_o;

	reg [SEL_WIDTH-1:0] addrr_M1_sel;
	reg [M1_OFFSET_WIDTH-1:0] addrr_M1_off;
	wire [M1_ADDR_WIDTH-1:0] addrr_M1_out;
	wire [35:0] Mem1_data_o;
	

	wire [SERIES_CNT_WIDTH-1:0] series_cnt_pip;
	
	wire[OPCODE_WIDTH-1:0]Opcode_pip;
	wire[AMUX_WIDTH-1:0]AMuxsel_pip;
	wire[BMUX_WIDTH-1:0]BMuxsel_pip;
	wire[CMUX_WIDTH-1:0]CMuxsel_pip;
	wire[AAMEM_WIDTH-1:0]AAMemsel_pip;
	wire[ABMEM_WIDTH-1:0]ABMemsel_pip;
	wire[BAMEM_WIDTH-1:0]BAMemsel_pip;
	wire[BBMEM_WIDTH-1:0]BBMemsel_pip;
	wire[CMEM_WIDTH-1:0]CMemsel_pip;
		
	wire CE1_pip;
	
	wire [M0_ADDR_WIDTH-1:0] Mem0_addrw_pip;
	wire Mem0_we_pip;
	
	reg harmonics_inc;
	reg harmonics_rst;
	reg harmonics_end;
	
	reg series_inc;
	reg series_rst;
	reg series_end;
	
	Sync_latch_input #(.OUT_POLARITY(1), .STEPS(2)) 
	code_start(.clk_i(clk_i), .in(enable_i), .out(WIP_flag_o), .reset_i(cnt == 25), .set_i(1'b0));
	
	assign Mem2_data_o = Mem0_data_i;
	assign Mem2_addrw_o = Mem0_addrw_pip;
	assign Mem2_we_o = Mem0_we_pip;	
	
	initial begin
		cnt = 26;
		harmonics_cnt = 0;
		series_cnt = 0;
		Opcode = OPCODE_SUM_A_B_C;
		AMuxsel = AMUX_GND;
		BMuxsel = BMUX_GND;
		CMuxsel = CMUX_GND;
		CE1 = 0;
		addrr_M0_sel = 0;
		addrr_M0_off = 0;
		addrr_M1_sel = 0;
		addrr_M1_off = 0;
		addrw_M0_sel = 0;
		addrw_M0_off = 0;
		Mem0_we = 0;
	end

	always @(posedge clk_i) begin
		Opcode <= OPCODE_NAND_BC;
		AMuxsel <= AMUX_GND;
		BMuxsel <= BMUX_GND;
		CMuxsel <= CMUX_GND;
		AAMemsel <= MEM_M0L;
		ABMemsel <= MEM_M1H;
		BAMemsel <= MEM_M0HS;
		BBMemsel <= MEM_M1H;
		CMemsel <= MEM_M0;
		CE1 <= 1'b1;
		addrr_M0_sel <= 0;
		addrr_M0_off <= 0;
		addrr_M1_sel <= 0;
		addrr_M1_off <= 0;
		addrw_M0_sel <= 0;
		addrw_M0_off <= 0;
		Mem0_we <= 0;
		
		if(harmonics_inc) harmonics_cnt <= harmonics_cnt + 1'b1;
		if(harmonics_rst) harmonics_cnt <= 0;
		harmonics_end <= harmonics_cnt == HARMONICS_NUM-1;
		harmonics_inc <= 0;
		harmonics_rst <= 0;
		
		if(series_inc) series_cnt <= series_cnt + 1'b1;
		if(series_rst) series_cnt <= 0;
		series_end <= series_cnt == IN_SERIES_NUM-1;
		series_inc <= 0;
		series_rst <= 0;
		
		if(WIP_flag_o)	cnt <= cnt + 1'b1;
			
		case(cnt[4:0])
			0: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_off <= M0_X1;
											
				CMemsel <= MEM_M0;
								
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_off <= M0_SUM;
				Mem0_we <= 1'b1;
				
				harmonics_inc <= 1'b1;
			end 
			1: begin
			end
			2: begin
				addrr_M0_sel <= SEL_STATES_INC;
				addrr_M1_sel <= SEL_STATES_INC;
				addrw_M0_sel <= SEL_STATES_INC;
			end
//-------------------------------------------------
			3: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_SIN;
																
				AAMemsel <= MEM_M0L;
				ABMemsel <= MEM_M1H;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_M1H;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b0;
			end
			4: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_COS;

				AAMemsel <= MEM_M0HS;
				ABMemsel <= MEM_M1L;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_RM1L;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_NB_NC;
			end
			5: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_COS;
				
				AAMemsel <= MEM_M0L;
				ABMemsel <= MEM_M1H;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_M1H;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
								
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_off <= M0_X1;
				
				Mem0_we <= 1'b1;
			end
			6: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_off <= M0_SUM;
				
				CMemsel <= MEM_M0;
				
				AMuxsel <= AMUX_ALU_FB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;	
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_off <= M0_SUM;

				Mem0_we <= 1'b1;
			end
			7: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_SIN;
				
				AAMemsel <= MEM_M0L;
				ABMemsel <= MEM_M1H;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_M1H;
								
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
			
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b1;
			end
			8: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_COS;
				
				AAMemsel <= MEM_M0HS;
				ABMemsel <= MEM_M1L;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_RM1L;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
			end 
			9: begin
				addrr_M0_sel <= SEL_STATES_INC;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES_INC;
				addrr_M1_off <= M1_COS;
				
				AAMemsel <= MEM_M0L;
				ABMemsel <= MEM_M1H;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_M1H;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				addrw_M0_sel <= SEL_STATES_INC;
				addrw_M0_off <= M0_X2;
				Mem0_we <= 1'b1;

				harmonics_inc <= 1'b1;
				if(!harmonics_end) cnt <= 5'd3;
			end
//-------------------------------------------------
			10: begin
				harmonics_rst <= 1'b1;
			end
			11: begin
			end
			12: begin
				addrr_M1_sel <= SEL_COMMON_INC;
				addrr_M1_off <= M1_INPUT;
				
				CMemsel <= MEM_M1;
				
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
			end
			13: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_off <= M0_SUM;
								
				CMemsel <= MEM_M0;
								
				AMuxsel <= AMUX_ALU_FB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_NC;
				
				addrw_M0_sel <= SEL_COMMON;
				addrw_M0_off <= M0_ERROR;
				Mem0_we <= 1'b1;
			end
			14: begin
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
				addrr_M0_sel <= SEL_STATES_RST;
				addrr_M1_sel <= SEL_STATES_RST;
				addrw_M0_sel <= SEL_STATES_RST;
            end
//-------------------------------------------------
			20: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_off <= M0_ERROR;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_K1;
								
				AAMemsel <= MEM_M0L;
				ABMemsel <= MEM_M1HS;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_M1HS;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b0;
			end
			21: begin
				addrr_M0_sel <= SEL_STATES;
				addrr_M0_off <= M0_X1;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_K1;
				
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_RM1L;
				CMemsel <= MEM_M0;
				
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
								
				addrw_M0_sel <= SEL_STATES;
				addrw_M0_off <= M0_X1;
				Mem0_we <= 1'b1;
			end
			22: begin
				addrr_M0_sel <= SEL_COMMON;
				addrr_M0_off <= M0_ERROR;
				
				addrr_M1_sel <= SEL_STATES;
				addrr_M1_off <= M1_K2;
								
				AAMemsel <= MEM_M0L;
				ABMemsel <= MEM_M1HS;
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_M1HS;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b0;
			end
			23: begin
				addrr_M0_sel <= SEL_STATES_INC;
				addrr_M0_off <= M0_X2;
				
				addrr_M1_sel <= SEL_STATES_INC;
				addrr_M1_off <= M1_K2;
				
				BAMemsel <= MEM_M0HS;
				BBMemsel <= MEM_RM1L;
				CMemsel <= MEM_M0;
								
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				addrw_M0_sel <= SEL_STATES_INC;
				addrw_M0_off <= M0_X2;
				Mem0_we <= 1'b1;
				
				harmonics_inc <= 1'b1;
				if(!harmonics_end) cnt <= 5'd20;
			end
//-------------------------------------------------
			24: begin
				addrr_M0_sel <= SEL_STATES_RST;
				addrr_M1_sel <= SEL_STATES_RST;
				addrw_M0_sel <= SEL_STATES_RST;
				
				series_inc <= 1'b1;
				harmonics_rst <= 1'b1;
				if(!series_end) cnt <= 5'd0;
			end
			25: begin
				series_rst <= 1'b1;
				addrr_M1_sel <= SEL_COMMON_RST;
			end
			26: begin
			end
			27: begin
			end
			28: begin
			end
            29: begin
			end
			30: begin
            end
            31: begin
            end
		endcase
	end
	
	
	Slice2 Slice2(.CLK0(clk_i), .CE0(1'b1), .CE1(CE1_pip), .CE2(1'b0), .CE3(1'b0), 
	.RST0(1'b0), .Mem0(Mem0_data_o), .Mem1(Mem1_data_o), .AAMemsel(AAMemsel_pip[2:0]), .ABMemsel(ABMemsel_pip[2:0]), 
	.BAMemsel(BAMemsel_pip[2:0]), .BBMemsel(BBMemsel_pip[2:0]), .CMemsel(CMemsel_pip), .SignAA(AAMemsel_pip[3]), .SignAB(ABMemsel_pip[3]),
	.SignBA(BAMemsel_pip[3]), .SignBB(BBMemsel_pip[3]), .AMuxsel(AMuxsel_pip), .BMuxsel(BMuxsel_pip), .CMuxsel(CMuxsel_pip), 
	.Opcode(Opcode_pip), .Result(Mem0_data_i), .Result54(CO), .SIGNEDR(SIGNEDCO), .EQZ(), .EQZM(), .EQOM(), .EQPAT(), 
	.EQPATB(), .OVER(), .UNDER(), .CIN(CIN), .SIGNEDCIN(SIGNEDCIN), .CO());
	
	wire [8:0] addrr_M0_out_series;
	assign addrr_M0_out_series = {series_cnt_pip,addrr_M0_out[M0_ADDR_WIDTH-1-SERIES_CNT_WIDTH:0]};
	pmi_ram_dp #(.pmi_wr_addr_depth(M0_ADDR_NUM), .pmi_wr_addr_width(M0_ADDR_WIDTH), .pmi_wr_data_width(36),
	.pmi_rd_addr_depth(M0_ADDR_NUM), .pmi_rd_addr_width(M0_ADDR_WIDTH), .pmi_rd_data_width(36), .pmi_regmode("reg"), 
	.pmi_gsr("enable"), .pmi_resetmode("sync"), .pmi_optimization("speed"), .pmi_family("ECP5U"),
	.pmi_init_file("../Mem0.mem"), .pmi_init_file_format("hex")
	)
	Mem0(.Data(Mem0_data_i), .WrAddress(Mem0_addrw_pip), .RdAddress(addrr_M0_out_series), .WrClock(clk_i),
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
	
	addr_gen_Kalman #(.ADDR_START_STATES(0), .ADDR_START_COMMON(M0_START_COMMON), .ADDR_INC_STATES(M0_STATES_OFFSET_NUMBER),
	.ADDR_INC_COMMON(M0_COMMON_OFFSET_NUMBER), .ADDR_WIDTH(M0_ADDR_WIDTH))
	addrr_M0_gen (.clk(clk_i), .addr_sel(addrr_M0_sel[2]), .addr_inc(addrr_M0_sel[0]), .addr_rst(addrr_M0_sel[1]),
	.addr_off(addrr_M0_off), .addr_out(addrr_M0_out));
	
	addr_gen_Kalman #(.ADDR_START_STATES(0), .ADDR_START_COMMON(M0_START_COMMON), .ADDR_INC_STATES(M0_STATES_OFFSET_NUMBER),
	.ADDR_INC_COMMON(M0_COMMON_OFFSET_NUMBER), .ADDR_WIDTH(M0_ADDR_WIDTH))
	addrw_M0_gen (.clk(clk_i), .addr_sel(addrw_M0_sel[2]), .addr_inc(addrw_M0_sel[0]), .addr_rst(addrw_M0_sel[1]),
	.addr_off(addrw_M0_off), .addr_out(addrw_M0_out));
	
	addr_gen_Kalman #(.ADDR_START_STATES(0), .ADDR_START_COMMON(M1_START_COMMON), .ADDR_INC_STATES(M1_STATES_OFFSET_NUMBER),
	.ADDR_INC_COMMON(M1_COMMON_OFFSET_NUMBER), .ADDR_WIDTH(M1_ADDR_WIDTH))
	addrr_M1_gen (.clk(clk_i), .addr_sel(addrr_M1_sel[2]), .addr_inc(addrr_M1_sel[0]), .addr_rst(addrr_M1_sel[1]),
	.addr_off(addrr_M1_off), .addr_out(addrr_M1_out));
	
	pipeline_delay #(.WIDTH(1),.CYCLES(6),.SHIFT_MEM(0)) 
	we_delay (.clk(clk_i), .in(Mem0_we), .out(Mem0_we_pip));
	
	pipeline_delay #(.WIDTH(9),.CYCLES(5),.SHIFT_MEM(0)) 
	addrw_M0_delay (.clk(clk_i), .in({series_cnt_pip, addrw_M0_out[M0_ADDR_WIDTH-1-SERIES_CNT_WIDTH:0]}), .out(Mem0_addrw_pip));
	
	
	pipeline_delay #(.WIDTH(OPCODE_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	opcode_delay (.clk(clk_i), .in(Opcode), .out(Opcode_pip));
	
	pipeline_delay #(.WIDTH(AMUX_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	amux_delay (.clk(clk_i), .in(AMuxsel), .out(AMuxsel_pip));
	
	pipeline_delay #(.WIDTH(BMUX_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	bmux_delay (.clk(clk_i), .in(BMuxsel), .out(BMuxsel_pip));
	
	pipeline_delay #(.WIDTH(CMUX_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	cmux_delay (.clk(clk_i), .in(CMuxsel), .out(CMuxsel_pip));
	
	pipeline_delay #(.WIDTH(AAMEM_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	aamem_delay (.clk(clk_i), .in(AAMemsel), .out(AAMemsel_pip));
	
	pipeline_delay #(.WIDTH(ABMEM_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	abmem_delay (.clk(clk_i), .in(ABMemsel), .out(ABMemsel_pip));
	
	pipeline_delay #(.WIDTH(BAMEM_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	bamem_delay (.clk(clk_i), .in(BAMemsel), .out(BAMemsel_pip));
	
	pipeline_delay #(.WIDTH(BBMEM_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	bbmem_delay (.clk(clk_i), .in(BBMemsel), .out(BBMemsel_pip));
	
	pipeline_delay #(.WIDTH(CMEM_WIDTH),.CYCLES(3),.SHIFT_MEM(0)) 
	cmem_delay (.clk(clk_i), .in(CMemsel), .out(CMemsel_pip));
		
	pipeline_delay #(.WIDTH(1),.CYCLES(4),.SHIFT_MEM(0)) 
	ce_delay (.clk(clk_i), .in(CE1), .out(CE1_pip));
				
	pipeline_delay #(.WIDTH(SERIES_CNT_WIDTH),.CYCLES(1),.SHIFT_MEM(0)) 
	series_cnt_delay (.clk(clk_i), .in(series_cnt), .out(series_cnt_pip));
	
	wire [4:0] cnt_pip2;
	wire [SERIES_CNT_WIDTH-1:0] series_cnt_pip2;
	wire [HARMONICS_CNT_WIDTH-1:0] harmonics_cnt_pip2;
	
	wire[OPCODE_WIDTH-1:0]Opcode_pip2;
	wire[AMUX_WIDTH-1:0]AMuxsel_pip2;
	wire[BMUX_WIDTH-1:0]BMuxsel_pip2;
	wire[CMUX_WIDTH-1:0]CMuxsel_pip2;
	wire[AAMEM_WIDTH-1:0]AAMemsel_pip2;
	wire[ABMEM_WIDTH-1:0]ABMemsel_pip2;
	wire[BAMEM_WIDTH-1:0]BAMemsel_pip2;
	wire[BBMEM_WIDTH-1:0]BBMemsel_pip2;
	wire[CMEM_WIDTH-1:0]CMemsel_pip2;
		
	wire CE1_pip2;
	
	wire [M0_ADDR_WIDTH-1:0] addrr_M0_out_pip2;
	wire [35:0] Mem0_data_o_pip2;
	wire [M1_ADDR_WIDTH-1:0] addrr_M1_out_pip2;
	wire [35:0] Mem1_data_o_pip2;
		
	wire [M0_ADDR_WIDTH-1:0] Mem0_addrw_pip2;
	wire [35:0] Mem0_data_i_pip2;
	wire Mem0_we_pip2;
	
	assign Mem0_data_i_pip2 = Mem0_data_i;
		
	if(DEBUG) begin
		pipeline_delay #(.WIDTH(1),.CYCLES(6),.SHIFT_MEM(0)) 
		we_delay2 (.clk(clk_i), .in(Mem0_we), .out(Mem0_we_pip2));
		
		pipeline_delay #(.WIDTH(9),.CYCLES(5),.SHIFT_MEM(0)) 
		addrw_M0_delay2 (.clk(clk_i), .in({series_cnt_pip, addrw_M0_out[M0_ADDR_WIDTH-1-SERIES_CNT_WIDTH:0]}), .out(Mem0_addrw_pip2));
		
		pipeline_delay #(.WIDTH(SERIES_CNT_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		series_cnt_delay2 (.clk(clk_i), .in(series_cnt), .out(series_cnt_pip2));
			
		pipeline_delay #(.WIDTH(HARMONICS_CNT_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		harmonics_cnt_delay2 (.clk(clk_i), .in(harmonics_cnt), .out(harmonics_cnt_pip2));
		
		pipeline_delay #(.WIDTH(OPCODE_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		opcode_delay2 (.clk(clk_i), .in(Opcode), .out(Opcode_pip2));
		
		pipeline_delay #(.WIDTH(AMUX_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		amux_delay2 (.clk(clk_i), .in(AMuxsel), .out(AMuxsel_pip2));
		
		pipeline_delay #(.WIDTH(BMUX_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		bmux_delay2 (.clk(clk_i), .in(BMuxsel), .out(BMuxsel_pip2));
		
		pipeline_delay #(.WIDTH(CMUX_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		cmux_delay2 (.clk(clk_i), .in(CMuxsel), .out(CMuxsel_pip2));
		
		pipeline_delay #(.WIDTH(AAMEM_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		aamem_delay2 (.clk(clk_i), .in(AAMemsel), .out(AAMemsel_pip2));
		
		pipeline_delay #(.WIDTH(ABMEM_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		abmem_delay2 (.clk(clk_i), .in(ABMemsel), .out(ABMemsel_pip2));
		
		pipeline_delay #(.WIDTH(BAMEM_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		bamem_delay2 (.clk(clk_i), .in(BAMemsel), .out(BAMemsel_pip2));
		
		pipeline_delay #(.WIDTH(BBMEM_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		bbmem_delay2 (.clk(clk_i), .in(BBMemsel), .out(BBMemsel_pip2));
		
		pipeline_delay #(.WIDTH(CMEM_WIDTH),.CYCLES(6),.SHIFT_MEM(0)) 
		cmem_delay2 (.clk(clk_i), .in(CMemsel), .out(CMemsel_pip2));
			
		pipeline_delay #(.WIDTH(1),.CYCLES(6),.SHIFT_MEM(0)) 
		ce_delay2 (.clk(clk_i), .in(CE1), .out(CE1_pip2));
		
		pipeline_delay #(.WIDTH(36),.CYCLES(3),.SHIFT_MEM(0)) 
		Mem0_data_o_delay2 (.clk(clk_i), .in(Mem0_data_o), .out(Mem0_data_o_pip2));

		pipeline_delay #(.WIDTH(36),.CYCLES(3),.SHIFT_MEM(0)) 
		Mem1_data_o_delay2 (.clk(clk_i), .in(Mem1_data_o), .out(Mem1_data_o_pip2));
		
		pipeline_delay #(.WIDTH(9),.CYCLES(5),.SHIFT_MEM(0)) 
		addrr_M0_delay2 (.clk(clk_i), .in(addrr_M0_out_series), .out(addrr_M0_out_pip2));
		
		pipeline_delay #(.WIDTH(9),.CYCLES(5),.SHIFT_MEM(0)) 
		addrr_M1_delay2 (.clk(clk_i), .in(addrr_M1_out), .out(addrr_M1_out_pip2));

		pipeline_delay #(.WIDTH(5),.CYCLES(7),.SHIFT_MEM(0)) 
		cnt_delay2 (.clk(clk_i), .in(cnt), .out(cnt_pip2));
	end
endmodule

module addr_gen_Kalman(clk, addr_sel, addr_inc, addr_rst, addr_off, addr_out);
	parameter ADDR_START_STATES = 9'd0;
	parameter ADDR_START_COMMON = 9'd0;
	parameter ADDR_INC_STATES = 1'b1;
	parameter ADDR_INC_COMMON = 1'b1;
	parameter ADDR_WIDTH = 9;

	localparam OFFSET_WIDTH = $clog2(ADDR_INC_STATES) > $clog2(ADDR_INC_COMMON) ? $clog2(ADDR_INC_STATES) : $clog2(ADDR_INC_COMMON);

	input clk;
	input addr_sel;
	input addr_inc;
	input addr_rst;
	input [OFFSET_WIDTH-1:0] addr_off;
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
		addr_sel_r <= addr_sel;
		addr_reg[0] <= cnt[0] + addr_off;
		addr_reg[1] <= cnt[1] + addr_off;
		case(addr_sel)
			0: begin
				if(addr_inc) cnt[0] <= cnt[0] + ADDR_INC_STATES;
				if(addr_rst) cnt[0] <= ADDR_START_STATES;
			end
			1: begin
				if(addr_inc) cnt[1] <= cnt[1] + ADDR_INC_COMMON;
				if(addr_rst) cnt[1] <= ADDR_START_COMMON;
			end
		endcase
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
