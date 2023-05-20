`timescale 1 ns / 1 ps

module Slice2 (CLK0, CE0, CE1, CE2, CE3, RST0, Mem0, Mem1, AAMemsel, ABMemsel, BAMemsel,
	BBMemsel, CMemsel, SignAA, SignAB, SignBA, SignBB, AMuxsel, BMuxsel, CMuxsel, Opcode, 
	Result, Result54, SIGNEDR, EQZ, EQZM, EQOM, EQPAT, EQPATB, OVER, UNDER, CIN, SIGNEDCIN, CO);
    input wire CLK0;
    input wire CE0;
	input wire CE1;
	input wire CE2;
	input wire CE3;
    input wire RST0;
	input wire SignAA;
	input wire SignAB;
    input wire SignBA;
	input wire SignBB;
	input wire [35:0] Mem0;
	input wire [35:0] Mem1;
	input wire [2:0] AAMemsel;
	input wire [2:0] ABMemsel;
	input wire [2:0] BAMemsel;
	input wire [2:0] BBMemsel;
	input wire CMemsel;
	input wire [1:0] AMuxsel;
    input wire [1:0] BMuxsel;
    input wire [2:0] CMuxsel;
    input wire [3:0] Opcode;
    output wire [35:0] Result;
	output wire [53:0] Result54;
	output wire SIGNEDR;
    output wire EQZ;
    output wire EQZM;
    output wire EQOM;
    output wire EQPAT;
    output wire EQPATB;
    output wire OVER;
    output wire UNDER;
	input wire [53:0] CIN;
	input wire SIGNEDCIN;
	output wire [53:0] CO;

    wire [53:0] ALU_O;
    wire [17:0] ALU_A_H;
    wire [17:0] ALU_A_L;
	wire [17:0] ALU_B_H;
    wire [17:0] ALU_B_L;
    wire [53:0] ALU_C;
	wire [35:0] ALU_MA;
	wire [35:0] ALU_MB;
	wire ALU_MA_SIGN;
	wire ALU_MB_SIGN;
    wire [53:0] ALU_CFB;
    
	
    wire [17:0] MULTA_A;
    wire [17:0] MULTA_B;
	wire [17:0] MULTA_C;
    wire [17:0] MULTA_ROB;
    wire [17:0] MULTA_ROA;
    wire [35:0] MULTA_P;
    wire MULTA_P_SIGN;
	
	wire [17:0] MULTB_A;
    wire [17:0] MULTB_B;
	wire [17:0] MULTB_C;
    wire [17:0] MULTB_ROB;
    wire [17:0] MULTB_ROA;
    wire [35:0] MULTB_P;
    wire MULTB_P_SIGN;
	
	wire scuba_vhi;
    wire scuba_vlo;
	
	reg [35:0] Mem0_r;
	reg [35:0] Mem1_r;
	
	wire [17:0] DataAA [7:0];
	assign DataAA[0] = Mem0[17:0];
	assign DataAA[1] = Mem0[35:18];
	assign DataAA[2] = Mem1[17:0];
	assign DataAA[3] = Mem1[35:18];
	assign DataAA[4] = Mem0_r[17:0];
	assign DataAA[5] = Mem0_r[35:18];
	assign DataAA[6] = Mem1_r[17:0];
	assign DataAA[7] = Mem1_r[35:18];
	wire [17:0] DataAB [7:0];
	assign DataAB[0] = Mem0[17:0];
	assign DataAB[1] = Mem0[35:18];
	assign DataAB[2] = Mem1[17:0];
	assign DataAB[3] = Mem1[35:18];
	assign DataAB[4] = Mem0_r[17:0];
	assign DataAB[5] = Mem0_r[35:18];
	assign DataAB[6] = Mem1_r[17:0];
	assign DataAB[7] = Mem1_r[35:18];
	wire [17:0] DataBA [7:0];
	assign DataBA[0] = Mem0[17:0];
	assign DataBA[1] = Mem0[35:18];
	assign DataBA[2] = Mem1[17:0];
	assign DataBA[3] = Mem1[35:18];
	assign DataBA[4] = Mem0_r[17:0];
	assign DataBA[5] = Mem0_r[35:18];
	assign DataBA[6] = Mem1_r[17:0];
	assign DataBA[7] = Mem1_r[35:18];
	wire [17:0] DataBB [7:0];
	assign DataBB[0] = Mem0[17:0];
	assign DataBB[1] = Mem0[35:18];
	assign DataBB[2] = Mem1[17:0];
	assign DataBB[3] = Mem1[35:18];
	assign DataBB[4] = Mem0_r[17:0];
	assign DataBB[5] = Mem0_r[35:18];
	assign DataBB[6] = Mem1_r[17:0];
	assign DataBB[7] = Mem1_r[35:18];
	wire [35:0] DataC [1:0];
	
		
	assign DataC[0] = Mem0;
	assign DataC[1] = Mem1;
	reg [35:0] DataC_r;
	always @(posedge CLK0) begin
		DataC_r <= DataC[CMemsel];
		Mem0_r <= Mem0;
		Mem1_r <= Mem1;
	end
		
	assign ALU_A_H = MULTA_ROB;
	assign ALU_A_L = MULTA_ROA;
	assign ALU_B_H = MULTB_ROB;
	assign ALU_B_L = MULTB_ROA;
	assign ALU_C[53:18] = DataC_r;
	assign ALU_C[17:0] = {18{DataC_r[35]}};
	assign ALU_MA = MULTA_P;
	assign ALU_MB = MULTB_P;
	assign ALU_MA_SIGN = MULTA_P_SIGN;
	assign ALU_MB_SIGN = MULTB_P_SIGN;
	assign ALU_CFB = {54{scuba_vlo}};
	
	assign MULTA_A = DataAA[AAMemsel];
	assign MULTA_B = DataAB[ABMemsel];
	assign MULTA_C = {18{scuba_vlo}};//Any
	
	assign MULTB_A = DataBA[BAMemsel];
	assign MULTB_B = DataBB[BBMemsel];
	assign MULTB_C = {18{scuba_vlo}};//ALU_C[44:27];	

	
	defparam dsp_alu.CLK3_DIV = "ENABLED" ;
    defparam dsp_alu.CLK2_DIV = "ENABLED" ;
    defparam dsp_alu.CLK1_DIV = "DISABLED" ;
    defparam dsp_alu.CLK0_DIV = "ENABLED" ;
    defparam dsp_alu.REG_INPUTCFB_RST = "RST0" ;
    defparam dsp_alu.REG_INPUTCFB_CE = "CE0" ;
    defparam dsp_alu.REG_INPUTCFB_CLK = "NONE" ;
    defparam dsp_alu.REG_OPCODEIN_1_RST = "RST0" ;
    defparam dsp_alu.REG_OPCODEIN_1_CE = "CE0" ;
    defparam dsp_alu.REG_OPCODEIN_1_CLK = "CLK0" ;
    defparam dsp_alu.REG_OPCODEIN_0_RST = "RST0" ;
    defparam dsp_alu.REG_OPCODEIN_0_CE = "CE0" ;
    defparam dsp_alu.REG_OPCODEIN_0_CLK = "CLK0" ;
    defparam dsp_alu.REG_OPCODEOP1_1_CLK = "CLK0" ;
    defparam dsp_alu.REG_OPCODEOP1_0_CLK = "CLK0" ;
    defparam dsp_alu.REG_OPCODEOP0_1_RST = "RST0" ;
    defparam dsp_alu.REG_OPCODEOP0_1_CE = "CE0" ;
    defparam dsp_alu.REG_OPCODEOP0_1_CLK = "CLK0" ;
    defparam dsp_alu.REG_OPCODEOP0_0_RST = "RST0" ;
    defparam dsp_alu.REG_OPCODEOP0_0_CE = "CE0" ;
    defparam dsp_alu.REG_OPCODEOP0_0_CLK = "CLK0" ;
    defparam dsp_alu.REG_INPUTC1_RST = "RST0" ;
    defparam dsp_alu.REG_INPUTC1_CE = "CE0" ;
    defparam dsp_alu.REG_INPUTC1_CLK = "CLK1" ;
    defparam dsp_alu.REG_INPUTC0_RST = "RST0" ;
    defparam dsp_alu.REG_INPUTC0_CE = "CE0" ;
    defparam dsp_alu.REG_INPUTC0_CLK = "CLK1" ;
    defparam dsp_alu.LEGACY = "DISABLED" ;
    defparam dsp_alu.REG_FLAG_RST = "RST0" ;
    defparam dsp_alu.REG_FLAG_CE = "CE0" ;
    defparam dsp_alu.REG_FLAG_CLK = "CLK0" ;
    defparam dsp_alu.REG_OUTPUT1_RST = "RST0" ;
    defparam dsp_alu.REG_OUTPUT1_CE = "CE0" ;
    defparam dsp_alu.REG_OUTPUT1_CLK = "CLK0" ;
    defparam dsp_alu.REG_OUTPUT0_RST = "RST0" ;
    defparam dsp_alu.REG_OUTPUT0_CE = "CE0" ;
    defparam dsp_alu.REG_OUTPUT0_CLK = "CLK0" ;
    defparam dsp_alu.MULT9_MODE = "DISABLED" ;
    defparam dsp_alu.RNDPAT = "0x00000000000000" ;
    defparam dsp_alu.MASKPAT = "0x00000000000000" ;
    defparam dsp_alu.MCPAT = "0x00000000000000" ;
    defparam dsp_alu.MASK01 = "0x00000000000000" ;
    defparam dsp_alu.MASKPAT_SOURCE = "STATIC" ;
    defparam dsp_alu.MCPAT_SOURCE = "STATIC" ;
    defparam dsp_alu.RESETMODE = "SYNC" ;
    defparam dsp_alu.GSR = "ENABLED" ;
    ALU54B dsp_alu (.A35(ALU_A_H[17]), .A34(ALU_A_H[16]), 
        .A33(ALU_A_H[15]), .A32(ALU_A_H[14]), 
        .A31(ALU_A_H[13]), .A30(ALU_A_H[12]), 
        .A29(ALU_A_H[11]), .A28(ALU_A_H[10]), 
        .A27(ALU_A_H[9]), .A26(ALU_A_H[8]), 
        .A25(ALU_A_H[7]), .A24(ALU_A_H[6]), 
        .A23(ALU_A_H[5]), .A22(ALU_A_H[4]), 
        .A21(ALU_A_H[3]), .A20(ALU_A_H[2]), 
        .A19(ALU_A_H[1]), .A18(ALU_A_H[0]), 
        .A17(ALU_A_L[17]), .A16(ALU_A_L[16]), 
        .A15(ALU_A_L[15]), .A14(ALU_A_L[14]), 
        .A13(ALU_A_L[13]), .A12(ALU_A_L[12]), 
        .A11(ALU_A_L[11]), .A10(ALU_A_L[10]), 
        .A9(ALU_A_L[9]), .A8(ALU_A_L[8]), .A7(ALU_A_L[7]), 
        .A6(ALU_A_L[6]), .A5(ALU_A_L[5]), .A4(ALU_A_L[4]), 
        .A3(ALU_A_L[3]), .A2(ALU_A_L[2]), .A1(ALU_A_L[1]), 
        .A0(ALU_A_L[0]), .B35(ALU_B_H[17]), 
        .B34(ALU_B_H[16]), .B33(ALU_B_H[15]), 
        .B32(ALU_B_H[14]), .B31(ALU_B_H[13]), 
        .B30(ALU_B_H[12]), .B29(ALU_B_H[11]), 
        .B28(ALU_B_H[10]), .B27(ALU_B_H[9]), 
        .B26(ALU_B_H[8]), .B25(ALU_B_H[7]), 
        .B24(ALU_B_H[6]), .B23(ALU_B_H[5]), 
        .B22(ALU_B_H[4]), .B21(ALU_B_H[3]), 
        .B20(ALU_B_H[2]), .B19(ALU_B_H[1]), 
        .B18(ALU_B_H[0]), .B17(ALU_B_L[17]), 
        .B16(ALU_B_L[16]), .B15(ALU_B_L[15]), 
        .B14(ALU_B_L[14]), .B13(ALU_B_L[13]), 
        .B12(ALU_B_L[12]), .B11(ALU_B_L[11]), 
        .B10(ALU_B_L[10]), .B9(ALU_B_L[9]), 
        .B8(ALU_B_L[8]), .B7(ALU_B_L[7]), .B6(ALU_B_L[6]), 
        .B5(ALU_B_L[5]), .B4(ALU_B_L[4]), .B3(ALU_B_L[3]), 
        .B2(ALU_B_L[2]), .B1(ALU_B_L[1]), .B0(ALU_B_L[0]), 
        .CFB53(ALU_CFB[53]), .CFB52(ALU_CFB[52]), .CFB51(ALU_CFB[51]), .CFB50(ALU_CFB[50]), 
        .CFB49(ALU_CFB[49]), .CFB48(ALU_CFB[48]), .CFB47(ALU_CFB[47]), .CFB46(ALU_CFB[46]), 
        .CFB45(ALU_CFB[45]), .CFB44(ALU_CFB[44]), .CFB43(ALU_CFB[43]), .CFB42(ALU_CFB[42]), 
        .CFB41(ALU_CFB[41]), .CFB40(ALU_CFB[40]), .CFB39(ALU_CFB[39]), .CFB38(ALU_CFB[38]), 
        .CFB37(ALU_CFB[37]), .CFB36(ALU_CFB[36]), .CFB35(ALU_CFB[35]), .CFB34(ALU_CFB[34]), 
        .CFB33(ALU_CFB[33]), .CFB32(ALU_CFB[32]), .CFB31(ALU_CFB[31]), .CFB30(ALU_CFB[30]), 
        .CFB29(ALU_CFB[29]), .CFB28(ALU_CFB[28]), .CFB27(ALU_CFB[27]), .CFB26(ALU_CFB[26]), 
        .CFB25(ALU_CFB[25]), .CFB24(ALU_CFB[24]), .CFB23(ALU_CFB[23]), .CFB22(ALU_CFB[22]), 
        .CFB21(ALU_CFB[21]), .CFB20(ALU_CFB[20]), .CFB19(ALU_CFB[19]), .CFB18(ALU_CFB[18]), 
        .CFB17(ALU_CFB[17]), .CFB16(ALU_CFB[16]), .CFB15(ALU_CFB[15]), .CFB14(ALU_CFB[14]), 
        .CFB13(ALU_CFB[13]), .CFB12(ALU_CFB[12]), .CFB11(ALU_CFB[11]), .CFB10(ALU_CFB[10]), 
        .CFB9(ALU_CFB[9]), .CFB8(ALU_CFB[8]), .CFB7(ALU_CFB[7]), .CFB6(ALU_CFB[6]), 
        .CFB5(ALU_CFB[5]), .CFB4(ALU_CFB[4]), .CFB3(ALU_CFB[3]), .CFB2(ALU_CFB[2]), 
        .CFB1(ALU_CFB[1]), .CFB0(ALU_CFB[0]), .C53(ALU_C[53]), .C52(ALU_C[52]), .C51(ALU_C[51]), 
        .C50(ALU_C[50]), .C49(ALU_C[49]), .C48(ALU_C[48]), .C47(ALU_C[47]), .C46(ALU_C[46]), 
        .C45(ALU_C[45]), .C44(ALU_C[44]), .C43(ALU_C[43]), .C42(ALU_C[42]), .C41(ALU_C[41]), 
        .C40(ALU_C[40]), .C39(ALU_C[39]), .C38(ALU_C[38]), .C37(ALU_C[37]), .C36(ALU_C[36]), 
        .C35(ALU_C[35]), .C34(ALU_C[34]), .C33(ALU_C[33]), .C32(ALU_C[32]), .C31(ALU_C[31]), 
        .C30(ALU_C[30]), .C29(ALU_C[29]), .C28(ALU_C[28]), .C27(ALU_C[27]), .C26(ALU_C[26]), 
        .C25(ALU_C[25]), .C24(ALU_C[24]), .C23(ALU_C[23]), .C22(ALU_C[22]), .C21(ALU_C[21]), 
        .C20(ALU_C[20]), .C19(ALU_C[19]), .C18(ALU_C[18]), .C17(ALU_C[17]), .C16(ALU_C[16]), 
        .C15(ALU_C[15]), .C14(ALU_C[14]), .C13(ALU_C[13]), .C12(ALU_C[12]), .C11(ALU_C[11]), 
        .C10(ALU_C[10]), .C9(ALU_C[9]), .C8(ALU_C[8]), .C7(ALU_C[7]), .C6(ALU_C[6]), .C5(ALU_C[5]), 
        .C4(ALU_C[4]), .C3(ALU_C[3]), .C2(ALU_C[2]), .C1(ALU_C[1]), .C0(ALU_C[0]), .CE0(CE0), 
        .CE1(scuba_vhi), .CE2(scuba_vhi), .CE3(scuba_vhi), .CLK0(CLK0), 
        .CLK1(CLK0), .CLK2(scuba_vlo), .CLK3(scuba_vlo), .RST0(RST0), .RST1(scuba_vlo), 
        .RST2(scuba_vlo), .RST3(scuba_vlo), .SIGNEDIA(ALU_MA_SIGN), 
        .SIGNEDIB(ALU_MB_SIGN), .SIGNEDCIN(SIGNEDCIN), .MA35(ALU_MA[35]), 
        .MA34(ALU_MA[34]), .MA33(ALU_MA[33]), 
        .MA32(ALU_MA[32]), .MA31(ALU_MA[31]), 
        .MA30(ALU_MA[30]), .MA29(ALU_MA[29]), 
        .MA28(ALU_MA[28]), .MA27(ALU_MA[27]), 
        .MA26(ALU_MA[26]), .MA25(ALU_MA[25]), 
        .MA24(ALU_MA[24]), .MA23(ALU_MA[23]), 
        .MA22(ALU_MA[22]), .MA21(ALU_MA[21]), 
        .MA20(ALU_MA[20]), .MA19(ALU_MA[19]), 
        .MA18(ALU_MA[18]), .MA17(ALU_MA[17]), 
        .MA16(ALU_MA[16]), .MA15(ALU_MA[15]), 
        .MA14(ALU_MA[14]), .MA13(ALU_MA[13]), 
        .MA12(ALU_MA[12]), .MA11(ALU_MA[11]), 
        .MA10(ALU_MA[10]), .MA9(ALU_MA[9]), .MA8(ALU_MA[8]), 
        .MA7(ALU_MA[7]), .MA6(ALU_MA[6]), .MA5(ALU_MA[5]), 
        .MA4(ALU_MA[4]), .MA3(ALU_MA[3]), .MA2(ALU_MA[2]), 
        .MA1(ALU_MA[1]), .MA0(ALU_MA[0]), .MB35(ALU_MB[35]), 
        .MB34(ALU_MB[34]), .MB33(ALU_MB[33]), 
        .MB32(ALU_MB[32]), .MB31(ALU_MB[31]), 
        .MB30(ALU_MB[30]), .MB29(ALU_MB[29]), 
        .MB28(ALU_MB[28]), .MB27(ALU_MB[27]), 
        .MB26(ALU_MB[26]), .MB25(ALU_MB[25]), 
        .MB24(ALU_MB[24]), .MB23(ALU_MB[23]), 
        .MB22(ALU_MB[22]), .MB21(ALU_MB[21]), 
        .MB20(ALU_MB[20]), .MB19(ALU_MB[19]), 
        .MB18(ALU_MB[18]), .MB17(ALU_MB[17]), 
        .MB16(ALU_MB[16]), .MB15(ALU_MB[15]), 
        .MB14(ALU_MB[14]), .MB13(ALU_MB[13]), 
        .MB12(ALU_MB[12]), .MB11(ALU_MB[11]), 
        .MB10(ALU_MB[10]), .MB9(ALU_MB[9]), .MB8(ALU_MB[8]), 
        .MB7(ALU_MB[7]), .MB6(ALU_MB[6]), .MB5(ALU_MB[5]), 
        .MB4(ALU_MB[4]), .MB3(ALU_MB[3]), .MB2(ALU_MB[2]), 
        .MB1(ALU_MB[1]), .MB0(ALU_MB[0]), .CIN53(CIN[53]), 
        .CIN52(CIN[52]), .CIN51(CIN[51]), .CIN50(CIN[50]), .CIN49(CIN[49]), 
        .CIN48(CIN[48]), .CIN47(CIN[47]), .CIN46(CIN[46]), .CIN45(CIN[45]), 
        .CIN44(CIN[44]), .CIN43(CIN[43]), .CIN42(CIN[42]), .CIN41(CIN[41]), 
        .CIN40(CIN[40]), .CIN39(CIN[39]), .CIN38(CIN[38]), .CIN37(CIN[37]), 
        .CIN36(CIN[36]), .CIN35(CIN[35]), .CIN34(CIN[34]), .CIN33(CIN[33]), 
        .CIN32(CIN[32]), .CIN31(CIN[31]), .CIN30(CIN[30]), .CIN29(CIN[29]), 
        .CIN28(CIN[28]), .CIN27(CIN[27]), .CIN26(CIN[26]), .CIN25(CIN[25]), 
        .CIN24(CIN[24]), .CIN23(CIN[23]), .CIN22(CIN[22]), .CIN21(CIN[21]), 
        .CIN20(CIN[20]), .CIN19(CIN[19]), .CIN18(CIN[18]), .CIN17(CIN[17]), 
        .CIN16(CIN[16]), .CIN15(CIN[15]), .CIN14(CIN[14]), .CIN13(CIN[13]), 
        .CIN12(CIN[12]), .CIN11(CIN[11]), .CIN10(CIN[10]), .CIN9(CIN[9]), 
        .CIN8(CIN[8]), .CIN7(CIN[7]), .CIN6(CIN[6]), .CIN5(CIN[5]), .CIN4(CIN[4]), 
        .CIN3(CIN[3]), .CIN2(CIN[2]), .CIN1(CIN[1]), .CIN0(CIN[0]), .OP10(Opcode[3]), 
        .OP9(Opcode[2]), .OP8(Opcode[1]), .OP7(Opcode[0]), .OP6(CMuxsel[2]), 
        .OP5(CMuxsel[1]), .OP4(CMuxsel[0]), .OP3(BMuxsel[1]), .OP2(BMuxsel[0]), 
        .OP1(AMuxsel[1]), .OP0(AMuxsel[0]), .R53(ALU_O[53]), 
        .R52(ALU_O[52]), .R51(ALU_O[51]), 
        .R50(ALU_O[50]), .R49(ALU_O[49]), 
        .R48(ALU_O[48]), .R47(ALU_O[47]), 
        .R46(ALU_O[46]), .R45(ALU_O[45]), 
        .R44(ALU_O[44]), .R43(ALU_O[43]), 
        .R42(ALU_O[42]), .R41(ALU_O[41]), 
        .R40(ALU_O[40]), .R39(ALU_O[39]), 
        .R38(ALU_O[38]), .R37(ALU_O[37]), 
        .R36(ALU_O[36]), .R35(ALU_O[35]), 
        .R34(ALU_O[34]), .R33(ALU_O[33]), 
        .R32(ALU_O[32]), .R31(ALU_O[31]), 
        .R30(ALU_O[30]), .R29(ALU_O[29]), 
        .R28(ALU_O[28]), .R27(ALU_O[27]), 
        .R26(ALU_O[26]), .R25(ALU_O[25]), 
        .R24(ALU_O[24]), .R23(ALU_O[23]), 
        .R22(ALU_O[22]), .R21(ALU_O[21]), 
        .R20(ALU_O[20]), .R19(ALU_O[19]), 
        .R18(ALU_O[18]), .R17(ALU_O[17]), 
        .R16(ALU_O[16]), .R15(ALU_O[15]), 
        .R14(ALU_O[14]), .R13(ALU_O[13]), 
        .R12(ALU_O[12]), .R11(ALU_O[11]), 
        .R10(ALU_O[10]), .R9(ALU_O[9]), 
        .R8(ALU_O[8]), .R7(ALU_O[7]), .R6(ALU_O[6]), 
        .R5(ALU_O[5]), .R4(ALU_O[4]), .R3(ALU_O[3]), 
        .R2(ALU_O[2]), .R1(ALU_O[1]), .R0(ALU_O[0]), 
		.CO53(CO[53]), .CO52(CO[52]), .CO51(CO[51]), .CO50(CO[50]), .CO49(CO[49]), .CO48(CO[48]), .CO47(CO[47]), .CO46(CO[46]),
		.CO45(CO[45]), .CO44(CO[44]), .CO43(CO[43]), .CO42(CO[42]), .CO41(CO[41]), .CO40(CO[40]), .CO39(CO[39]), .CO38(CO[38]),
		.CO37(CO[37]), .CO36(CO[36]), .CO35(CO[35]), .CO34(CO[34]), .CO33(CO[33]), .CO32(CO[32]), .CO31(CO[31]), .CO30(CO[30]),
		.CO29(CO[29]), .CO28(CO[28]), .CO27(CO[27]), .CO26(CO[26]), .CO25(CO[25]), .CO24(CO[24]), .CO23(CO[23]), .CO22(CO[22]),
		.CO21(CO[21]), .CO20(CO[20]), .CO19(CO[19]), .CO18(CO[18]), .CO17(CO[17]), .CO16(CO[16]), .CO15(CO[15]), .CO14(CO[14]),
		.CO13(CO[13]), .CO12(CO[12]), .CO11(CO[11]), .CO10(CO[10]), .CO9(CO[9]), .CO8(CO[8]), .CO7(CO[7]), .CO6(CO[6]),
		.CO5(CO[5]), .CO4(CO[4]), .CO3(CO[3]), .CO2(CO[2]), .CO1(CO[1]), .CO0(CO[0]), .EQZ(EQZ), .EQZM(EQZM), 
        .EQOM(EQOM), .EQPAT(EQPAT), .EQPATB(EQPATB), .OVER(OVER), .UNDER(UNDER), 
        .OVERUNDER(), .SIGNEDR(SIGNEDR));

    defparam dsp_mult_A.CLK3_DIV = "ENABLED" ;
    defparam dsp_mult_A.CLK2_DIV = "ENABLED" ;
    defparam dsp_mult_A.CLK1_DIV = "DISABLED" ;
    defparam dsp_mult_A.CLK0_DIV = "ENABLED" ;
    defparam dsp_mult_A.HIGHSPEED_CLK = "NONE" ;
    defparam dsp_mult_A.REG_INPUTC_RST = "RST0" ;
    defparam dsp_mult_A.REG_INPUTC_CE = "CE0" ;
    defparam dsp_mult_A.REG_INPUTC_CLK = "NONE" ;
    defparam dsp_mult_A.SOURCEB_MODE = "B_SHIFT" ;
    defparam dsp_mult_A.MULT_BYPASS = "DISABLED" ;
    defparam dsp_mult_A.CAS_MATCH_REG = "FALSE" ;
    defparam dsp_mult_A.RESETMODE = "SYNC" ;
    defparam dsp_mult_A.GSR = "ENABLED" ;
    defparam dsp_mult_A.REG_OUTPUT_RST = "RST0" ;
    defparam dsp_mult_A.REG_OUTPUT_CE = "CE0" ;
    defparam dsp_mult_A.REG_OUTPUT_CLK = "NONE" ;
    defparam dsp_mult_A.REG_PIPELINE_RST = "RST0" ;
    defparam dsp_mult_A.REG_PIPELINE_CE = "CE0" ;
    defparam dsp_mult_A.REG_PIPELINE_CLK = "CLK0" ;
    defparam dsp_mult_A.REG_INPUTB_RST = "RST0" ;
    defparam dsp_mult_A.REG_INPUTB_CE = "CE0" ;
    defparam dsp_mult_A.REG_INPUTB_CLK = "CLK0" ;
    defparam dsp_mult_A.REG_INPUTA_RST = "RST0" ;
    defparam dsp_mult_A.REG_INPUTA_CE = "CE0" ;
    defparam dsp_mult_A.REG_INPUTA_CLK = "CLK0" ;
    MULT18X18D dsp_mult_A (.A17(MULTA_A[17]), .A16(MULTA_A[16]), 
		.A15(MULTA_A[15]), .A14(MULTA_A[14]), .A13(MULTA_A[13]),
		.A12(MULTA_A[12]), .A11(MULTA_A[11]), .A10(MULTA_A[10]), 
		.A9(MULTA_A[9]), .A8(MULTA_A[8]), .A7(MULTA_A[7]), 
		.A6(MULTA_A[6]), .A5(MULTA_A[5]), .A4(MULTA_A[4]), 
		.A3(MULTA_A[3]), .A2(MULTA_A[2]), .A1(MULTA_A[1]), 
		.A0(MULTA_A[0]), .B17(MULTA_B[17]), .B16(MULTA_B[16]), 
		.B15(MULTA_B[15]), .B14(MULTA_B[14]), .B13(MULTA_B[13]),
		.B12(MULTA_B[12]), .B11(MULTA_B[11]), .B10(MULTA_B[10]), 
		.B9(MULTA_B[9]), .B8(MULTA_B[8]), .B7(MULTA_B[7]), 
		.B6(MULTA_B[6]), .B5(MULTA_B[5]), .B4(MULTA_B[4]), 
		.B3(MULTA_B[3]), .B2(MULTA_B[2]), .B1(MULTA_B[1]), 
		.B0(MULTA_B[0]), .C17(MULTA_C[17]), 
        .C16(MULTA_C[16]), .C15(MULTA_C[15]), .C14(MULTA_C[14]), .C13(MULTA_C[13]), 
        .C12(MULTA_C[12]), .C11(MULTA_C[11]), .C10(MULTA_C[10]), .C9(MULTA_C[9]), 
        .C8(MULTA_C[8]), .C7(MULTA_C[7]), .C6(MULTA_C[6]), .C5(MULTA_C[5]), 
        .C4(MULTA_C[4]), .C3(MULTA_C[3]), .C2(MULTA_C[2]), .C1(MULTA_C[1]), 
        .C0(MULTA_C[0]), .SIGNEDA(SignAA), .SIGNEDB(SignAB), .SOURCEA(scuba_vlo), 
        .SOURCEB(scuba_vlo), .CE0(CE0), .CE1(CE1), .CE2(scuba_vhi), 
        .CE3(scuba_vhi), .CLK0(CLK0), .CLK1(scuba_vlo), .CLK2(scuba_vlo), 
        .CLK3(scuba_vlo), .RST0(RST0), .RST1(scuba_vlo), .RST2(scuba_vlo), 
        .RST3(scuba_vlo), .SRIA17(scuba_vlo), .SRIA16(scuba_vlo), .SRIA15(scuba_vlo), 
        .SRIA14(scuba_vlo), .SRIA13(scuba_vlo), .SRIA12(scuba_vlo), .SRIA11(scuba_vlo), 
        .SRIA10(scuba_vlo), .SRIA9(scuba_vlo), .SRIA8(scuba_vlo), .SRIA7(scuba_vlo), 
        .SRIA6(scuba_vlo), .SRIA5(scuba_vlo), .SRIA4(scuba_vlo), .SRIA3(scuba_vlo), 
        .SRIA2(scuba_vlo), .SRIA1(scuba_vlo), .SRIA0(scuba_vlo), .SRIB17(scuba_vlo), 
        .SRIB16(scuba_vlo), .SRIB15(scuba_vlo), .SRIB14(scuba_vlo), .SRIB13(scuba_vlo), 
        .SRIB12(scuba_vlo), .SRIB11(scuba_vlo), .SRIB10(scuba_vlo), .SRIB9(scuba_vlo), 
        .SRIB8(scuba_vlo), .SRIB7(scuba_vlo), .SRIB6(scuba_vlo), .SRIB5(scuba_vlo), 
        .SRIB4(scuba_vlo), .SRIB3(scuba_vlo), .SRIB2(scuba_vlo), .SRIB1(scuba_vlo), 
        .SRIB0(scuba_vlo), .SROA17(), .SROA16(), .SROA15(), .SROA14(), .SROA13(), 
        .SROA12(), .SROA11(), .SROA10(), .SROA9(), .SROA8(), .SROA7(), .SROA6(), 
        .SROA5(), .SROA4(), .SROA3(), .SROA2(), .SROA1(), .SROA0(), .SROB17(), 
        .SROB16(), .SROB15(), .SROB14(), .SROB13(), .SROB12(), .SROB11(), 
        .SROB10(), .SROB9(), .SROB8(), .SROB7(), .SROB6(), .SROB5(), .SROB4(), 
        .SROB3(), .SROB2(), .SROB1(), .SROB0(), .ROA17(MULTA_ROA[17]), 
        .ROA16(MULTA_ROA[16]), .ROA15(MULTA_ROA[15]), 
        .ROA14(MULTA_ROA[14]), .ROA13(MULTA_ROA[13]), 
        .ROA12(MULTA_ROA[12]), .ROA11(MULTA_ROA[11]), 
        .ROA10(MULTA_ROA[10]), .ROA9(MULTA_ROA[9]), 
        .ROA8(MULTA_ROA[8]), .ROA7(MULTA_ROA[7]), 
        .ROA6(MULTA_ROA[6]), .ROA5(MULTA_ROA[5]), 
        .ROA4(MULTA_ROA[4]), .ROA3(MULTA_ROA[3]), 
        .ROA2(MULTA_ROA[2]), .ROA1(MULTA_ROA[1]), 
        .ROA0(MULTA_ROA[0]), .ROB17(MULTA_ROB[17]), 
        .ROB16(MULTA_ROB[16]), .ROB15(MULTA_ROB[15]), 
        .ROB14(MULTA_ROB[14]), .ROB13(MULTA_ROB[13]), 
        .ROB12(MULTA_ROB[12]), .ROB11(MULTA_ROB[11]), 
        .ROB10(MULTA_ROB[10]), .ROB9(MULTA_ROB[9]), 
        .ROB8(MULTA_ROB[8]), .ROB7(MULTA_ROB[7]), 
        .ROB6(MULTA_ROB[6]), .ROB5(MULTA_ROB[5]), 
        .ROB4(MULTA_ROB[4]), .ROB3(MULTA_ROB[3]), 
        .ROB2(MULTA_ROB[2]), .ROB1(MULTA_ROB[1]), 
        .ROB0(MULTA_ROB[0]), .ROC17(), .ROC16(), .ROC15(), .ROC14(), 
        .ROC13(), .ROC12(), .ROC11(), .ROC10(), .ROC9(), .ROC8(), .ROC7(), 
        .ROC6(), .ROC5(), .ROC4(), .ROC3(), .ROC2(), .ROC1(), .ROC0(), .P35(MULTA_P[35]), 
        .P34(MULTA_P[34]), .P33(MULTA_P[33]), .P32(MULTA_P[32]), 
        .P31(MULTA_P[31]), .P30(MULTA_P[30]), .P29(MULTA_P[29]), 
        .P28(MULTA_P[28]), .P27(MULTA_P[27]), .P26(MULTA_P[26]), 
        .P25(MULTA_P[25]), .P24(MULTA_P[24]), .P23(MULTA_P[23]), 
        .P22(MULTA_P[22]), .P21(MULTA_P[21]), .P20(MULTA_P[20]), 
        .P19(MULTA_P[19]), .P18(MULTA_P[18]), .P17(MULTA_P[17]), 
        .P16(MULTA_P[16]), .P15(MULTA_P[15]), .P14(MULTA_P[14]), 
        .P13(MULTA_P[13]), .P12(MULTA_P[12]), .P11(MULTA_P[11]), 
        .P10(MULTA_P[10]), .P9(MULTA_P[9]), .P8(MULTA_P[8]), 
        .P7(MULTA_P[7]), .P6(MULTA_P[6]), .P5(MULTA_P[5]), 
        .P4(MULTA_P[4]), .P3(MULTA_P[3]), .P2(MULTA_P[2]), 
        .P1(MULTA_P[1]), .P0(MULTA_P[0]), .SIGNEDP(MULTA_P_SIGN));

    VHI scuba_vhi_inst (.Z(scuba_vhi));

    VLO scuba_vlo_inst (.Z(scuba_vlo));

    defparam dsp_mult_B.CLK3_DIV = "ENABLED" ;
    defparam dsp_mult_B.CLK2_DIV = "ENABLED" ;
    defparam dsp_mult_B.CLK1_DIV = "DISABLED" ;
    defparam dsp_mult_B.CLK0_DIV = "ENABLED" ;
    defparam dsp_mult_B.HIGHSPEED_CLK = "NONE" ;
    defparam dsp_mult_B.REG_INPUTC_RST = "RST0" ;
    defparam dsp_mult_B.REG_INPUTC_CE = "CE0" ;
    defparam dsp_mult_B.REG_INPUTC_CLK = "NONE" ;
    defparam dsp_mult_B.SOURCEB_MODE = "B_SHIFT" ;
    defparam dsp_mult_B.MULT_BYPASS = "DISABLED" ;
    defparam dsp_mult_B.CAS_MATCH_REG = "FALSE" ;
    defparam dsp_mult_B.RESETMODE = "SYNC" ;
    defparam dsp_mult_B.GSR = "ENABLED" ;
    defparam dsp_mult_B.REG_OUTPUT_RST = "RST0" ;
    defparam dsp_mult_B.REG_OUTPUT_CE = "CE0" ;
    defparam dsp_mult_B.REG_OUTPUT_CLK = "NONE" ;
    defparam dsp_mult_B.REG_PIPELINE_RST = "RST0" ;
    defparam dsp_mult_B.REG_PIPELINE_CE = "CE0" ;
    defparam dsp_mult_B.REG_PIPELINE_CLK = "CLK0" ;
    defparam dsp_mult_B.REG_INPUTB_RST = "RST0" ;
    defparam dsp_mult_B.REG_INPUTB_CE = "CE0" ;
    defparam dsp_mult_B.REG_INPUTB_CLK = "CLK0" ;
    defparam dsp_mult_B.REG_INPUTA_RST = "RST0" ;
    defparam dsp_mult_B.REG_INPUTA_CE = "CE1" ;
    defparam dsp_mult_B.REG_INPUTA_CLK = "CLK0" ;
    MULT18X18D dsp_mult_B (.A17(MULTB_A[17]), .A16(MULTB_A[16]), 
		.A15(MULTB_A[15]), .A14(MULTB_A[14]), .A13(MULTB_A[13]),
		.A12(MULTB_A[12]), .A11(MULTB_A[11]), .A10(MULTB_A[10]), 
		.A9(MULTB_A[9]), .A8(MULTB_A[8]), .A7(MULTB_A[7]), 
		.A6(MULTB_A[6]), .A5(MULTB_A[5]), .A4(MULTB_A[4]), 
		.A3(MULTB_A[3]), .A2(MULTB_A[2]), .A1(MULTB_A[1]), 
		.A0(MULTB_A[0]), .B17(MULTB_B[17]), .B16(MULTB_B[16]), 
		.B15(MULTB_B[15]), .B14(MULTB_B[14]), .B13(MULTB_B[13]),
		.B12(MULTB_B[12]), .B11(MULTB_B[11]), .B10(MULTB_B[10]), 
		.B9(MULTB_B[9]), .B8(MULTB_B[8]), .B7(MULTB_B[7]), 
		.B6(MULTB_B[6]), .B5(MULTB_B[5]), .B4(MULTB_B[4]), 
		.B3(MULTB_B[3]), .B2(MULTB_B[2]), .B1(MULTB_B[1]), 
		.B0(MULTB_B[0]), .C17(MULTB_C[17]), 
        .C16(MULTB_C[16]), .C15(MULTB_C[15]), .C14(MULTB_C[14]), .C13(MULTB_C[13]), 
        .C12(MULTB_C[12]), .C11(MULTB_C[11]), .C10(MULTB_C[10]), .C9(MULTB_C[9]), 
        .C8(MULTB_C[8]), .C7(MULTB_C[7]), .C6(MULTB_C[6]), .C5(MULTB_C[5]), 
        .C4(MULTB_C[4]), .C3(MULTB_C[3]), .C2(MULTB_C[2]), .C1(MULTB_C[1]),
        .C0(MULTB_C[0]), .SIGNEDA(SignBA), .SIGNEDB(SignBB), .SOURCEA(scuba_vlo), 
        .SOURCEB(scuba_vlo), .CE0(CE0), .CE1(CE1), .CE2(scuba_vhi), 
        .CE3(scuba_vhi), .CLK0(CLK0), .CLK1(scuba_vlo), .CLK2(scuba_vlo), 
        .CLK3(scuba_vlo), .RST0(RST0), .RST1(scuba_vlo), .RST2(scuba_vlo), 
        .RST3(scuba_vlo), .SRIA17(scuba_vlo), .SRIA16(scuba_vlo), .SRIA15(scuba_vlo), 
        .SRIA14(scuba_vlo), .SRIA13(scuba_vlo), .SRIA12(scuba_vlo), .SRIA11(scuba_vlo), 
        .SRIA10(scuba_vlo), .SRIA9(scuba_vlo), .SRIA8(scuba_vlo), .SRIA7(scuba_vlo), 
        .SRIA6(scuba_vlo), .SRIA5(scuba_vlo), .SRIA4(scuba_vlo), .SRIA3(scuba_vlo), 
        .SRIA2(scuba_vlo), .SRIA1(scuba_vlo), .SRIA0(scuba_vlo), .SRIB17(scuba_vlo), 
        .SRIB16(scuba_vlo), .SRIB15(scuba_vlo), .SRIB14(scuba_vlo), .SRIB13(scuba_vlo), 
        .SRIB12(scuba_vlo), .SRIB11(scuba_vlo), .SRIB10(scuba_vlo), .SRIB9(scuba_vlo), 
        .SRIB8(scuba_vlo), .SRIB7(scuba_vlo), .SRIB6(scuba_vlo), .SRIB5(scuba_vlo), 
        .SRIB4(scuba_vlo), .SRIB3(scuba_vlo), .SRIB2(scuba_vlo), .SRIB1(scuba_vlo), 
        .SRIB0(scuba_vlo), .SROA17(), .SROA16(), .SROA15(), .SROA14(), .SROA13(), 
        .SROA12(), .SROA11(), .SROA10(), .SROA9(), .SROA8(), .SROA7(), .SROA6(), 
        .SROA5(), .SROA4(), .SROA3(), .SROA2(), .SROA1(), .SROA0(), .SROB17(), 
        .SROB16(), .SROB15(), .SROB14(), .SROB13(), .SROB12(), .SROB11(), 
        .SROB10(), .SROB9(), .SROB8(), .SROB7(), .SROB6(), .SROB5(), .SROB4(), 
        .SROB3(), .SROB2(), .SROB1(), .SROB0(), .ROA17(MULTB_ROA[17]), 
        .ROA16(MULTB_ROA[16]), .ROA15(MULTB_ROA[15]), 
        .ROA14(MULTB_ROA[14]), .ROA13(MULTB_ROA[13]), 
        .ROA12(MULTB_ROA[12]), .ROA11(MULTB_ROA[11]), 
        .ROA10(MULTB_ROA[10]), .ROA9(MULTB_ROA[9]), 
        .ROA8(MULTB_ROA[8]), .ROA7(MULTB_ROA[7]), 
        .ROA6(MULTB_ROA[6]), .ROA5(MULTB_ROA[5]), 
        .ROA4(MULTB_ROA[4]), .ROA3(MULTB_ROA[3]), 
        .ROA2(MULTB_ROA[2]), .ROA1(MULTB_ROA[1]), 
        .ROA0(MULTB_ROA[0]), .ROB17(MULTB_ROB[17]), 
        .ROB16(MULTB_ROB[16]), .ROB15(MULTB_ROB[15]), 
        .ROB14(MULTB_ROB[14]), .ROB13(MULTB_ROB[13]), 
        .ROB12(MULTB_ROB[12]), .ROB11(MULTB_ROB[11]), 
        .ROB10(MULTB_ROB[10]), .ROB9(MULTB_ROB[9]), 
        .ROB8(MULTB_ROB[8]), .ROB7(MULTB_ROB[7]), 
        .ROB6(MULTB_ROB[6]), .ROB5(MULTB_ROB[5]), 
        .ROB4(MULTB_ROB[4]), .ROB3(MULTB_ROB[3]), 
        .ROB2(MULTB_ROB[2]), .ROB1(MULTB_ROB[1]), 
        .ROB0(MULTB_ROB[0]), .ROC17(), .ROC16(), .ROC15(), .ROC14(), 
        .ROC13(), .ROC12(), .ROC11(), .ROC10(), .ROC9(), .ROC8(), .ROC7(), 
        .ROC6(), .ROC5(), .ROC4(), .ROC3(), .ROC2(), .ROC1(), .ROC0(), .P35(MULTB_P[35]), 
        .P34(MULTB_P[34]), .P33(MULTB_P[33]), .P32(MULTB_P[32]), 
        .P31(MULTB_P[31]), .P30(MULTB_P[30]), .P29(MULTB_P[29]), 
        .P28(MULTB_P[28]), .P27(MULTB_P[27]), .P26(MULTB_P[26]), 
        .P25(MULTB_P[25]), .P24(MULTB_P[24]), .P23(MULTB_P[23]), 
        .P22(MULTB_P[22]), .P21(MULTB_P[21]), .P20(MULTB_P[20]), 
        .P19(MULTB_P[19]), .P18(MULTB_P[18]), .P17(MULTB_P[17]), 
        .P16(MULTB_P[16]), .P15(MULTB_P[15]), .P14(MULTB_P[14]), 
        .P13(MULTB_P[13]), .P12(MULTB_P[12]), .P11(MULTB_P[11]), 
        .P10(MULTB_P[10]), .P9(MULTB_P[9]), .P8(MULTB_P[8]), 
        .P7(MULTB_P[7]), .P6(MULTB_P[6]), .P5(MULTB_P[5]), 
        .P4(MULTB_P[4]), .P3(MULTB_P[3]), .P2(MULTB_P[2]), 
        .P1(MULTB_P[1]), .P0(MULTB_P[0]), .SIGNEDP(MULTB_P_SIGN));

    assign Result = ALU_O[53:18];	
	assign Result54 = ALU_O;
endmodule
