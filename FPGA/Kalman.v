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
	localparam SM0_X1 = 2'b00; 
	localparam SM0_X2 = 2'b01;
	
	localparam M0_COMMON_OFFSET_NUMBER = 9'd2;
	localparam M0_START_COMMON = M0_STATES_OFFSET_NUMBER*HARMONICS_NUM;
	localparam CM0_SUM = 2'b10;
	localparam CM0_ERR = 2'b11;
	
	localparam M0_PTR_WIDTH = ($clog2(M0_COMMON_OFFSET_NUMBER) > $clog2(M0_STATES_OFFSET_NUMBER) ? $clog2(M0_COMMON_OFFSET_NUMBER) : $clog2(M0_STATES_OFFSET_NUMBER))+1;

	localparam M1_ADDR_WIDTH = 9;
	localparam M1_ADDR_NUM = 2**M1_ADDR_WIDTH;
	localparam M1_STATES_OFFSET_NUMBER = 9'd4;
	localparam M1_STATES_OFFSET_WIDTH = $clog2(M1_STATES_OFFSET_NUMBER);	
	localparam SM1_COS = 3'b000;
	localparam SM1_SIN = 3'b001;
	localparam SM1_K1 = 3'b010;
	localparam SM1_K2 = 3'b011;
	
	localparam M1_COMMON_OFFSET_NUMBER = 9'd1;
	localparam M1_START_COMMON = M1_STATES_OFFSET_NUMBER*HARMONICS_NUM;
	localparam CM1_INP = 3'b000;
	
	localparam M1_PTR_WIDTH = ($clog2(M1_COMMON_OFFSET_NUMBER) > $clog2(M1_STATES_OFFSET_NUMBER) ? $clog2(M1_COMMON_OFFSET_NUMBER) : $clog2(M1_STATES_OFFSET_NUMBER))+1;

	localparam SEL_WIDTH = 4;
	localparam SEL_NONE = 4'b0000;
	localparam SEL_S_INC = 4'b0001;
	localparam SEL_S_RST = 4'b0010;
	localparam SEL_C_INC = 4'b0100;
	localparam SEL_C_RST = 4'b1000;

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

	localparam AAMEM_WIDTH = 1; 	
	localparam ABMEM_WIDTH = 1;
	localparam BAMEM_WIDTH = 1;
	localparam BBMEM_WIDTH = 1;
	localparam CMEM_WIDTH = 1; 
	localparam MEM_MXL = 1'b0;
	localparam MEM_RMXL = 1'b0;
	localparam MEM_MXH = 1'b1;
	localparam MEM_RMXH = 1'b1;
	
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
	reg [M0_PTR_WIDTH-1:0] addrw_M0_ptr;
	wire [M0_ADDR_WIDTH-1:0] addrw_M0_out;
	
	reg [SEL_WIDTH-1:0] addrr_M0_sel;
	reg [M0_PTR_WIDTH-1:0] addrr_M0_ptr;
	wire [M0_ADDR_WIDTH-1:0] addrr_M0_out;
	wire [35:0] Mem0_data_o;

	reg [SEL_WIDTH-1:0] addrr_M1_sel;
	reg [M1_PTR_WIDTH-1:0] addrr_M1_ptr;
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
	code_start(.clk_i(clk_i), .in(enable_i), .out(WIP_flag_o), .reset_i(cnt == 20), .set_i(1'b0));
	
	assign Mem2_data_o = Mem0_data_i;
	assign Mem2_addrw_o = Mem0_addrw_pip;
	assign Mem2_we_o = Mem0_we_pip;	
	
	initial begin
		cnt = 21;
		harmonics_cnt = 0;
		series_cnt = 0;
		Opcode = OPCODE_SUM_A_B_C;
		AMuxsel = AMUX_GND;
		BMuxsel = BMUX_GND;
		CMuxsel = CMUX_GND;
		CE1 = 0;
		addrr_M0_sel = 0;
		addrr_M0_ptr = 0;
		addrr_M1_sel = 0;
		addrr_M1_ptr = 0;
		addrw_M0_sel = 0;
		addrw_M0_ptr = 0;
		Mem0_we = 0;
	end

	always @(posedge clk_i) begin
		Opcode <= OPCODE_NAND_BC;
		AMuxsel <= AMUX_GND;
		BMuxsel <= BMUX_GND;
		CMuxsel <= CMUX_GND;
		AAMemsel <= MEM_MXH;
		ABMemsel <= MEM_MXH;
		BAMemsel <= MEM_MXH;
		BBMemsel <= MEM_MXH;
		CMemsel <= MEM_M0;
		CE1 <= 1'b1;
		addrr_M0_sel <= 0;
		addrr_M0_ptr <= 0;
		addrr_M1_sel <= 0;
		addrr_M1_ptr <= 0;
		addrw_M0_sel <= 0;
		addrw_M0_ptr <= 0;
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
				addrr_M0_ptr <= SM0_X1;
											
				CMemsel <= MEM_M0;
								
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				addrw_M0_ptr <= CM0_SUM;
				Mem0_we <= 1'b1;
				
				addrr_M0_sel <= SEL_S_INC;
				addrr_M1_sel <= SEL_S_INC;
				addrw_M0_sel <= SEL_S_INC;
				
				harmonics_inc <= 1'b1;
			end 
//-------------------------------------------------
			1: begin
				addrr_M0_ptr <= SM0_X1;
				addrr_M1_ptr <= SM1_SIN;
				
				AAMemsel <= MEM_MXL;
				ABMemsel <= MEM_MXH;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_MXH;
								
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
			
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b1;
			end
			2: begin
				addrr_M0_ptr <= SM0_X2;
				addrr_M1_ptr <= SM1_COS;
				
				AAMemsel <= MEM_MXH;
				ABMemsel <= MEM_MXL;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_RMXL;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
			end 
			3: begin
				addrr_M0_ptr <= SM0_X2;
				addrr_M1_ptr <= SM1_COS;
				
				AAMemsel <= MEM_MXL;
				ABMemsel <= MEM_MXH;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_MXH;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				addrw_M0_ptr <= SM0_X2;
				Mem0_we <= 1'b1;
			end
			4: begin
				addrr_M0_ptr <= SM0_X2;
				addrr_M1_ptr <= SM1_SIN;
																
				AAMemsel <= MEM_MXL;
				ABMemsel <= MEM_MXH;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_MXH;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b0;
			end
			5: begin
				addrr_M0_ptr <= SM0_X1;
				addrr_M1_ptr <= SM1_COS;

				AAMemsel <= MEM_MXH;
				ABMemsel <= MEM_MXL;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_RMXL;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_NB_NC;
			end
			6: begin
				addrr_M0_ptr <= SM0_X1;
				addrr_M1_ptr <= SM1_COS;
				
				AAMemsel <= MEM_MXL;
				ABMemsel <= MEM_MXH;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_MXH;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				CMuxsel <= CMUX_ALU_FB;
				
				Opcode <= OPCODE_SUM_A_B_C;
								
				addrw_M0_ptr <= SM0_X1;				
				Mem0_we <= 1'b1;
			end
			7: begin
				addrr_M0_ptr <= CM0_SUM;
				
				CMemsel <= MEM_M0;
				
				AMuxsel <= AMUX_ALU_FB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;	
				
				addrw_M0_ptr <= CM0_SUM;
				Mem0_we <= 1'b1;
				
				addrr_M0_sel <= SEL_S_INC;
				addrw_M0_sel <= SEL_S_INC;
				addrr_M1_sel <= SEL_S_INC;

				harmonics_inc <= 1'b1;
				if(!harmonics_end) cnt <= 5'd1;
			end
//-------------------------------------------------
			8: begin
				addrr_M1_sel <= SEL_C_INC;
				addrr_M1_ptr <= CM1_INP;
				
				CMemsel <= MEM_M1;
				
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_NC;
				
				harmonics_rst <= 1'b1;
			end
			9: begin
				addrr_M0_ptr <= CM0_SUM;

				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_NC;
				
				addrw_M0_ptr <= CM0_ERR;
				Mem0_we <= 1'b1;
			end
			10: begin
			end
			11: begin
			end
			12: begin
			end
			13: begin
			end
			14: begin
				addrr_M0_sel <= SEL_S_RST;
				addrr_M1_sel <= SEL_S_RST;
				addrw_M0_sel <= SEL_S_RST;
			end
//-------------------------------------------------
			15: begin
				addrr_M0_ptr <= CM0_ERR;
				addrr_M1_ptr <= SM1_K1;
								
				AAMemsel <= MEM_MXL;
				ABMemsel <= MEM_MXH;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_MXH;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b0;
			end
			16: begin
				addrr_M0_ptr <= SM0_X1;
				
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_RMXL;
				CMemsel <= MEM_M0;
				
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
								
				addrw_M0_ptr <= SM0_X1;
				Mem0_we <= 1'b1;
			end
			17: begin
				addrr_M0_ptr <= CM0_ERR;
				addrr_M1_ptr <= SM1_K2;
								
				AAMemsel <= MEM_MXL;
				ABMemsel <= MEM_MXH;
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_MXH;
				
				AMuxsel <= AMUX_MULTA;
				BMuxsel <= BMUX_MULTB_L18;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				CE1 <= 1'b0;
			end
			18: begin
				addrr_M0_ptr <= SM0_X2;
				
				BAMemsel <= MEM_MXH;
				BBMemsel <= MEM_RMXL;
				CMemsel <= MEM_M0;
								
				AMuxsel <= AMUX_ALU_FB;
				BMuxsel <= BMUX_MULTB;
				CMuxsel <= CMUX_C_ALU;
				
				Opcode <= OPCODE_SUM_A_B_C;
				
				addrw_M0_ptr <= SM0_X2;
				Mem0_we <= 1'b1;
				
				addrr_M0_sel <= SEL_S_INC;
				addrr_M1_sel <= SEL_S_INC;
				addrw_M0_sel <= SEL_S_INC;
				
				harmonics_inc <= 1'b1;
				if(!harmonics_end) cnt <= 5'd15;
			end
//-------------------------------------------------
			19: begin
				addrr_M0_sel <= SEL_S_RST;
				addrr_M1_sel <= SEL_S_RST | SEL_C_INC;
				addrw_M0_sel <= SEL_S_RST;
				
				series_inc <= 1'b1;
				harmonics_rst <= 1'b1;
				if(!series_end) cnt <= 5'd0;
			end
			20: begin
				addrr_M0_sel <= SEL_C_RST;
				addrr_M1_sel <= SEL_C_RST;
				addrw_M0_sel <= SEL_C_RST;
				
				series_rst <= 1'b1;
			end
			21: begin
			end
            22: begin
				cnt <= 5'd0;
            end
			23: begin
			end
			24: begin
			end
            25: begin
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
	.RST0(1'b0), .Mem0(Mem0_data_o), .Mem1(Mem1_data_o), .AAMemsel(AAMemsel_pip), .ABMemsel(ABMemsel_pip), 
	.BAMemsel(BAMemsel_pip), .BBMemsel(BBMemsel_pip), .CMemsel(CMemsel_pip), .SignAA(AAMemsel_pip), .SignAB(ABMemsel_pip),
	.SignBA(BAMemsel_pip), .SignBB(BBMemsel_pip), .AMuxsel(AMuxsel_pip), .BMuxsel(BMuxsel_pip), .CMuxsel(CMuxsel_pip), 
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
	addrr_M0_gen (.clk(clk_i), .addr_sel(addrr_M0_sel),
	.addr_ptr(addrr_M0_ptr), .addr_out(addrr_M0_out));
	
	addr_gen_Kalman #(.ADDR_START_STATES(0), .ADDR_START_COMMON(M0_START_COMMON), .ADDR_INC_STATES(M0_STATES_OFFSET_NUMBER),
	.ADDR_INC_COMMON(M0_COMMON_OFFSET_NUMBER), .ADDR_WIDTH(M0_ADDR_WIDTH))
	addrw_M0_gen (.clk(clk_i), .addr_sel(addrw_M0_sel),
	.addr_ptr(addrw_M0_ptr), .addr_out(addrw_M0_out));
	
	addr_gen_Kalman #(.ADDR_START_STATES(0), .ADDR_START_COMMON(M1_START_COMMON), .ADDR_INC_STATES(M1_STATES_OFFSET_NUMBER),
	.ADDR_INC_COMMON(M1_COMMON_OFFSET_NUMBER), .ADDR_WIDTH(M1_ADDR_WIDTH))
	addrr_M1_gen (.clk(clk_i), .addr_sel(addrr_M1_sel),
	.addr_ptr(addrr_M1_ptr), .addr_out(addrr_M1_out));
	
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
