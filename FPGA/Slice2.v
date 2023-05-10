`timescale 1 ns / 1 ps

module Slice2 (CLK0, CE0, CE1, CE2, CE3, RST0, Mem0, Mem1, AAMemsel, ABMemsel, BAMemsel,
	BBMemsel, CMemsel, SignAA, SignAB, SignBA, SignBB, AMuxsel, BMuxsel, CMuxsel, Opcode, 
	Result, Result54, EQZ, EQZM, EQOM, EQPAT, EQPATB, OVER, UNDER);
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
	input wire [1:0] AAMemsel;
	input wire [1:0] ABMemsel;
	input wire [1:0] BAMemsel;
	input wire [1:0] BBMemsel;
	input wire CMemsel;
	input wire [1:0] AMuxsel;
    input wire [1:0] BMuxsel;
    input wire [2:0] CMuxsel;
    input wire [3:0] Opcode;
    output wire [35:0] Result;
	output wire [53:0] Result54;
    output wire EQZ;
    output wire EQZM;
    output wire EQOM;
    output wire EQPAT;
    output wire EQPATB;
    output wire OVER;
    output wire UNDER;

    wire Slice_alu_signedr_1_0;
    wire Slice_alu_output[53:0];
    wire Slice_0_mult_out_rob_0[17:0];
    wire Slice_0_mult_out_roa_0[17:0];
    wire Slice_0_mult_out_p_0[35:0];
    wire Slice_0_mult_out_signedp_0;
    wire Slice_0_mult_out_rob_1[17:0];
    wire Slice_0_mult_out_roa_1[17:0];
    wire Slice_0_mult_out_p_1[35:0];
    wire Slice_0_mult_out_signedp_1;
	wire scuba_vhi;
    wire scuba_vlo;
	
	wire [17:0] DataAA [3:0];
	assign DataAA[0] = Mem0[17:0];
	assign DataAA[1] = Mem0[35:18];
	assign DataAA[2] = Mem1[17:0];
	assign DataAA[3] = Mem1[35:18];
	wire [17:0] DataAB [3:0];
	assign DataAB[0] = Mem0[17:0];
	assign DataAB[1] = Mem0[35:18];
	assign DataAB[2] = Mem1[17:0];
	assign DataAB[3] = Mem1[35:18];
	wire [17:0] DataBA [3:0];
	assign DataBA[0] = Mem0[17:0];
	assign DataBA[1] = Mem0[35:18];
	assign DataBA[2] = Mem1[17:0];
	assign DataBA[3] = Mem1[35:18];
	wire [17:0] DataBB [3:0];
	assign DataBB[0] = Mem0[17:0];
	assign DataBB[1] = Mem0[35:18];
	assign DataBB[2] = Mem1[17:0];
	assign DataBB[3] = Mem1[35:18];
	wire [35:0] DataC [1:0];
	assign DataC[0] = Mem0;
	assign DataC[1] = Mem1;
	reg [35:0] DataC_r;
	always @(posedge CLK0) begin
		DataC_r <= DataC[CMemsel];
	end
	
	defparam dsp_alu_0.CLK3_DIV = "ENABLED" ;
    defparam dsp_alu_0.CLK2_DIV = "ENABLED" ;
    defparam dsp_alu_0.CLK1_DIV = "DISABLED" ;
    defparam dsp_alu_0.CLK0_DIV = "ENABLED" ;
    defparam dsp_alu_0.REG_INPUTCFB_RST = "RST0" ;
    defparam dsp_alu_0.REG_INPUTCFB_CE = "CE0" ;
    defparam dsp_alu_0.REG_INPUTCFB_CLK = "NONE" ;
    defparam dsp_alu_0.REG_OPCODEIN_1_RST = "RST0" ;
    defparam dsp_alu_0.REG_OPCODEIN_1_CE = "CE0" ;
    defparam dsp_alu_0.REG_OPCODEIN_1_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_OPCODEIN_0_RST = "RST0" ;
    defparam dsp_alu_0.REG_OPCODEIN_0_CE = "CE0" ;
    defparam dsp_alu_0.REG_OPCODEIN_0_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_OPCODEOP1_1_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_OPCODEOP1_0_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_OPCODEOP0_1_RST = "RST0" ;
    defparam dsp_alu_0.REG_OPCODEOP0_1_CE = "CE0" ;
    defparam dsp_alu_0.REG_OPCODEOP0_1_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_OPCODEOP0_0_RST = "RST0" ;
    defparam dsp_alu_0.REG_OPCODEOP0_0_CE = "CE0" ;
    defparam dsp_alu_0.REG_OPCODEOP0_0_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_INPUTC1_RST = "RST0" ;
    defparam dsp_alu_0.REG_INPUTC1_CE = "CE0" ;
    defparam dsp_alu_0.REG_INPUTC1_CLK = "CLK1" ;
    defparam dsp_alu_0.REG_INPUTC0_RST = "RST0" ;
    defparam dsp_alu_0.REG_INPUTC0_CE = "CE0" ;
    defparam dsp_alu_0.REG_INPUTC0_CLK = "CLK1" ;
    defparam dsp_alu_0.LEGACY = "DISABLED" ;
    defparam dsp_alu_0.REG_FLAG_RST = "RST0" ;
    defparam dsp_alu_0.REG_FLAG_CE = "CE0" ;
    defparam dsp_alu_0.REG_FLAG_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_OUTPUT1_RST = "RST0" ;
    defparam dsp_alu_0.REG_OUTPUT1_CE = "CE0" ;
    defparam dsp_alu_0.REG_OUTPUT1_CLK = "CLK0" ;
    defparam dsp_alu_0.REG_OUTPUT0_RST = "RST0" ;
    defparam dsp_alu_0.REG_OUTPUT0_CE = "CE0" ;
    defparam dsp_alu_0.REG_OUTPUT0_CLK = "CLK0" ;
    defparam dsp_alu_0.MULT9_MODE = "DISABLED" ;
    defparam dsp_alu_0.RNDPAT = "0x00000000000000" ;
    defparam dsp_alu_0.MASKPAT = "0x00000000000000" ;
    defparam dsp_alu_0.MCPAT = "0x00000000000000" ;
    defparam dsp_alu_0.MASK01 = "0x00000000000000" ;
    defparam dsp_alu_0.MASKPAT_SOURCE = "STATIC" ;
    defparam dsp_alu_0.MCPAT_SOURCE = "STATIC" ;
    defparam dsp_alu_0.RESETMODE = "SYNC" ;
    defparam dsp_alu_0.GSR = "ENABLED" ;
    ALU54B dsp_alu_0 (.A35(Slice_0_mult_out_rob_0[17]), .A34(Slice_0_mult_out_rob_0[16]), 
        .A33(Slice_0_mult_out_rob_0[15]), .A32(Slice_0_mult_out_rob_0[14]), 
        .A31(Slice_0_mult_out_rob_0[13]), .A30(Slice_0_mult_out_rob_0[12]), 
        .A29(Slice_0_mult_out_rob_0[11]), .A28(Slice_0_mult_out_rob_0[10]), 
        .A27(Slice_0_mult_out_rob_0[9]), .A26(Slice_0_mult_out_rob_0[8]), 
        .A25(Slice_0_mult_out_rob_0[7]), .A24(Slice_0_mult_out_rob_0[6]), 
        .A23(Slice_0_mult_out_rob_0[5]), .A22(Slice_0_mult_out_rob_0[4]), 
        .A21(Slice_0_mult_out_rob_0[3]), .A20(Slice_0_mult_out_rob_0[2]), 
        .A19(Slice_0_mult_out_rob_0[1]), .A18(Slice_0_mult_out_rob_0[0]), 
        .A17(Slice_0_mult_out_roa_0[17]), .A16(Slice_0_mult_out_roa_0[16]), 
        .A15(Slice_0_mult_out_roa_0[15]), .A14(Slice_0_mult_out_roa_0[14]), 
        .A13(Slice_0_mult_out_roa_0[13]), .A12(Slice_0_mult_out_roa_0[12]), 
        .A11(Slice_0_mult_out_roa_0[11]), .A10(Slice_0_mult_out_roa_0[10]), 
        .A9(Slice_0_mult_out_roa_0[9]), .A8(Slice_0_mult_out_roa_0[8]), .A7(Slice_0_mult_out_roa_0[7]), 
        .A6(Slice_0_mult_out_roa_0[6]), .A5(Slice_0_mult_out_roa_0[5]), .A4(Slice_0_mult_out_roa_0[4]), 
        .A3(Slice_0_mult_out_roa_0[3]), .A2(Slice_0_mult_out_roa_0[2]), .A1(Slice_0_mult_out_roa_0[1]), 
        .A0(Slice_0_mult_out_roa_0[0]), .B35(Slice_0_mult_out_rob_1[17]), 
        .B34(Slice_0_mult_out_rob_1[16]), .B33(Slice_0_mult_out_rob_1[15]), 
        .B32(Slice_0_mult_out_rob_1[14]), .B31(Slice_0_mult_out_rob_1[13]), 
        .B30(Slice_0_mult_out_rob_1[12]), .B29(Slice_0_mult_out_rob_1[11]), 
        .B28(Slice_0_mult_out_rob_1[10]), .B27(Slice_0_mult_out_rob_1[9]), 
        .B26(Slice_0_mult_out_rob_1[8]), .B25(Slice_0_mult_out_rob_1[7]), 
        .B24(Slice_0_mult_out_rob_1[6]), .B23(Slice_0_mult_out_rob_1[5]), 
        .B22(Slice_0_mult_out_rob_1[4]), .B21(Slice_0_mult_out_rob_1[3]), 
        .B20(Slice_0_mult_out_rob_1[2]), .B19(Slice_0_mult_out_rob_1[1]), 
        .B18(Slice_0_mult_out_rob_1[0]), .B17(Slice_0_mult_out_roa_1[17]), 
        .B16(Slice_0_mult_out_roa_1[16]), .B15(Slice_0_mult_out_roa_1[15]), 
        .B14(Slice_0_mult_out_roa_1[14]), .B13(Slice_0_mult_out_roa_1[13]), 
        .B12(Slice_0_mult_out_roa_1[12]), .B11(Slice_0_mult_out_roa_1[11]), 
        .B10(Slice_0_mult_out_roa_1[10]), .B9(Slice_0_mult_out_roa_1[9]), 
        .B8(Slice_0_mult_out_roa_1[8]), .B7(Slice_0_mult_out_roa_1[7]), .B6(Slice_0_mult_out_roa_1[6]), 
        .B5(Slice_0_mult_out_roa_1[5]), .B4(Slice_0_mult_out_roa_1[4]), .B3(Slice_0_mult_out_roa_1[3]), 
        .B2(Slice_0_mult_out_roa_1[2]), .B1(Slice_0_mult_out_roa_1[1]), .B0(Slice_0_mult_out_roa_1[0]), 
        .CFB53(scuba_vlo), .CFB52(scuba_vlo), .CFB51(scuba_vlo), .CFB50(scuba_vlo), 
        .CFB49(scuba_vlo), .CFB48(scuba_vlo), .CFB47(scuba_vlo), .CFB46(scuba_vlo), 
        .CFB45(scuba_vlo), .CFB44(scuba_vlo), .CFB43(scuba_vlo), .CFB42(scuba_vlo), 
        .CFB41(scuba_vlo), .CFB40(scuba_vlo), .CFB39(scuba_vlo), .CFB38(scuba_vlo), 
        .CFB37(scuba_vlo), .CFB36(scuba_vlo), .CFB35(scuba_vlo), .CFB34(scuba_vlo), 
        .CFB33(scuba_vlo), .CFB32(scuba_vlo), .CFB31(scuba_vlo), .CFB30(scuba_vlo), 
        .CFB29(scuba_vlo), .CFB28(scuba_vlo), .CFB27(scuba_vlo), .CFB26(scuba_vlo), 
        .CFB25(scuba_vlo), .CFB24(scuba_vlo), .CFB23(scuba_vlo), .CFB22(scuba_vlo), 
        .CFB21(scuba_vlo), .CFB20(scuba_vlo), .CFB19(scuba_vlo), .CFB18(scuba_vlo), 
        .CFB17(scuba_vlo), .CFB16(scuba_vlo), .CFB15(scuba_vlo), .CFB14(scuba_vlo), 
        .CFB13(scuba_vlo), .CFB12(scuba_vlo), .CFB11(scuba_vlo), .CFB10(scuba_vlo), 
        .CFB9(scuba_vlo), .CFB8(scuba_vlo), .CFB7(scuba_vlo), .CFB6(scuba_vlo), 
        .CFB5(scuba_vlo), .CFB4(scuba_vlo), .CFB3(scuba_vlo), .CFB2(scuba_vlo), 
        .CFB1(scuba_vlo), .CFB0(scuba_vlo), .C53(DataC_r[35]), .C52(DataC_r[34]), .C51(DataC_r[33]), 
        .C50(DataC_r[32]), .C49(DataC_r[31]), .C48(DataC_r[30]), .C47(DataC_r[29]), .C46(DataC_r[28]), 
        .C45(DataC_r[27]), .C44(DataC_r[26]), .C43(DataC_r[25]), .C42(DataC_r[24]), .C41(DataC_r[23]), 
        .C40(DataC_r[22]), .C39(DataC_r[21]), .C38(DataC_r[20]), .C37(DataC_r[19]), .C36(DataC_r[18]), 
        .C35(DataC_r[17]), .C34(DataC_r[16]), .C33(DataC_r[15]), .C32(DataC_r[14]), .C31(DataC_r[13]), 
        .C30(DataC_r[12]), .C29(DataC_r[11]), .C28(DataC_r[10]), .C27(DataC_r[9]), .C26(DataC_r[8]), 
        .C25(DataC_r[7]), .C24(DataC_r[6]), .C23(DataC_r[5]), .C22(DataC_r[4]), .C21(DataC_r[32]), 
        .C20(DataC_r[2]), .C19(DataC_r[1]), .C18(DataC_r[0]), .C17(scuba_vlo), .C16(scuba_vlo), 
        .C15(scuba_vlo), .C14(scuba_vlo), .C13(scuba_vlo), .C12(scuba_vlo), .C11(scuba_vlo), 
        .C10(scuba_vlo), .C9(scuba_vlo), .C8(scuba_vlo), .C7(scuba_vlo), .C6(scuba_vlo), .C5(scuba_vlo), 
        .C4(scuba_vlo), .C3(scuba_vlo), .C2(scuba_vlo), .C1(scuba_vlo), .C0(scuba_vlo), .CE0(CE0), 
        .CE1(scuba_vhi), .CE2(scuba_vhi), .CE3(scuba_vhi), .CLK0(CLK0), 
        .CLK1(CLK0), .CLK2(scuba_vlo), .CLK3(scuba_vlo), .RST0(RST0), .RST1(scuba_vlo), 
        .RST2(scuba_vlo), .RST3(scuba_vlo), .SIGNEDIA(Slice_0_mult_out_signedp_0), 
        .SIGNEDIB(Slice_0_mult_out_signedp_1), .SIGNEDCIN(scuba_vlo), .MA35(Slice_0_mult_out_p_0[35]), 
        .MA34(Slice_0_mult_out_p_0[34]), .MA33(Slice_0_mult_out_p_0[33]), 
        .MA32(Slice_0_mult_out_p_0[32]), .MA31(Slice_0_mult_out_p_0[31]), 
        .MA30(Slice_0_mult_out_p_0[30]), .MA29(Slice_0_mult_out_p_0[29]), 
        .MA28(Slice_0_mult_out_p_0[28]), .MA27(Slice_0_mult_out_p_0[27]), 
        .MA26(Slice_0_mult_out_p_0[26]), .MA25(Slice_0_mult_out_p_0[25]), 
        .MA24(Slice_0_mult_out_p_0[24]), .MA23(Slice_0_mult_out_p_0[23]), 
        .MA22(Slice_0_mult_out_p_0[22]), .MA21(Slice_0_mult_out_p_0[21]), 
        .MA20(Slice_0_mult_out_p_0[20]), .MA19(Slice_0_mult_out_p_0[19]), 
        .MA18(Slice_0_mult_out_p_0[18]), .MA17(Slice_0_mult_out_p_0[17]), 
        .MA16(Slice_0_mult_out_p_0[16]), .MA15(Slice_0_mult_out_p_0[15]), 
        .MA14(Slice_0_mult_out_p_0[14]), .MA13(Slice_0_mult_out_p_0[13]), 
        .MA12(Slice_0_mult_out_p_0[12]), .MA11(Slice_0_mult_out_p_0[11]), 
        .MA10(Slice_0_mult_out_p_0[10]), .MA9(Slice_0_mult_out_p_0[9]), .MA8(Slice_0_mult_out_p_0[8]), 
        .MA7(Slice_0_mult_out_p_0[7]), .MA6(Slice_0_mult_out_p_0[6]), .MA5(Slice_0_mult_out_p_0[5]), 
        .MA4(Slice_0_mult_out_p_0[4]), .MA3(Slice_0_mult_out_p_0[3]), .MA2(Slice_0_mult_out_p_0[2]), 
        .MA1(Slice_0_mult_out_p_0[1]), .MA0(Slice_0_mult_out_p_0[0]), .MB35(Slice_0_mult_out_p_1[35]), 
        .MB34(Slice_0_mult_out_p_1[34]), .MB33(Slice_0_mult_out_p_1[33]), 
        .MB32(Slice_0_mult_out_p_1[32]), .MB31(Slice_0_mult_out_p_1[31]), 
        .MB30(Slice_0_mult_out_p_1[30]), .MB29(Slice_0_mult_out_p_1[29]), 
        .MB28(Slice_0_mult_out_p_1[28]), .MB27(Slice_0_mult_out_p_1[27]), 
        .MB26(Slice_0_mult_out_p_1[26]), .MB25(Slice_0_mult_out_p_1[25]), 
        .MB24(Slice_0_mult_out_p_1[24]), .MB23(Slice_0_mult_out_p_1[23]), 
        .MB22(Slice_0_mult_out_p_1[22]), .MB21(Slice_0_mult_out_p_1[21]), 
        .MB20(Slice_0_mult_out_p_1[20]), .MB19(Slice_0_mult_out_p_1[19]), 
        .MB18(Slice_0_mult_out_p_1[18]), .MB17(Slice_0_mult_out_p_1[17]), 
        .MB16(Slice_0_mult_out_p_1[16]), .MB15(Slice_0_mult_out_p_1[15]), 
        .MB14(Slice_0_mult_out_p_1[14]), .MB13(Slice_0_mult_out_p_1[13]), 
        .MB12(Slice_0_mult_out_p_1[12]), .MB11(Slice_0_mult_out_p_1[11]), 
        .MB10(Slice_0_mult_out_p_1[10]), .MB9(Slice_0_mult_out_p_1[9]), .MB8(Slice_0_mult_out_p_1[8]), 
        .MB7(Slice_0_mult_out_p_1[7]), .MB6(Slice_0_mult_out_p_1[6]), .MB5(Slice_0_mult_out_p_1[5]), 
        .MB4(Slice_0_mult_out_p_1[4]), .MB3(Slice_0_mult_out_p_1[3]), .MB2(Slice_0_mult_out_p_1[2]), 
        .MB1(Slice_0_mult_out_p_1[1]), .MB0(Slice_0_mult_out_p_1[0]), .CIN53(scuba_vlo), 
        .CIN52(scuba_vlo), .CIN51(scuba_vlo), .CIN50(scuba_vlo), .CIN49(scuba_vlo), 
        .CIN48(scuba_vlo), .CIN47(scuba_vlo), .CIN46(scuba_vlo), .CIN45(scuba_vlo), 
        .CIN44(scuba_vlo), .CIN43(scuba_vlo), .CIN42(scuba_vlo), .CIN41(scuba_vlo), 
        .CIN40(scuba_vlo), .CIN39(scuba_vlo), .CIN38(scuba_vlo), .CIN37(scuba_vlo), 
        .CIN36(scuba_vlo), .CIN35(scuba_vlo), .CIN34(scuba_vlo), .CIN33(scuba_vlo), 
        .CIN32(scuba_vlo), .CIN31(scuba_vlo), .CIN30(scuba_vlo), .CIN29(scuba_vlo), 
        .CIN28(scuba_vlo), .CIN27(scuba_vlo), .CIN26(scuba_vlo), .CIN25(scuba_vlo), 
        .CIN24(scuba_vlo), .CIN23(scuba_vlo), .CIN22(scuba_vlo), .CIN21(scuba_vlo), 
        .CIN20(scuba_vlo), .CIN19(scuba_vlo), .CIN18(scuba_vlo), .CIN17(scuba_vlo), 
        .CIN16(scuba_vlo), .CIN15(scuba_vlo), .CIN14(scuba_vlo), .CIN13(scuba_vlo), 
        .CIN12(scuba_vlo), .CIN11(scuba_vlo), .CIN10(scuba_vlo), .CIN9(scuba_vlo), 
        .CIN8(scuba_vlo), .CIN7(scuba_vlo), .CIN6(scuba_vlo), .CIN5(scuba_vlo), .CIN4(scuba_vlo), 
        .CIN3(scuba_vlo), .CIN2(scuba_vlo), .CIN1(scuba_vlo), .CIN0(scuba_vlo), .OP10(Opcode[3]), 
        .OP9(Opcode[2]), .OP8(Opcode[1]), .OP7(Opcode[0]), .OP6(CMuxsel[2]), 
        .OP5(CMuxsel[1]), .OP4(CMuxsel[0]), .OP3(BMuxsel[1]), .OP2(BMuxsel[0]), 
        .OP1(AMuxsel[1]), .OP0(AMuxsel[0]), .R53(Slice_alu_output[53]), 
        .R52(Slice_alu_output[52]), .R51(Slice_alu_output[51]), 
        .R50(Slice_alu_output[50]), .R49(Slice_alu_output[49]), 
        .R48(Slice_alu_output[48]), .R47(Slice_alu_output[47]), 
        .R46(Slice_alu_output[46]), .R45(Slice_alu_output[45]), 
        .R44(Slice_alu_output[44]), .R43(Slice_alu_output[43]), 
        .R42(Slice_alu_output[42]), .R41(Slice_alu_output[41]), 
        .R40(Slice_alu_output[40]), .R39(Slice_alu_output[39]), 
        .R38(Slice_alu_output[38]), .R37(Slice_alu_output[37]), 
        .R36(Slice_alu_output[36]), .R35(Slice_alu_output[35]), 
        .R34(Slice_alu_output[34]), .R33(Slice_alu_output[33]), 
        .R32(Slice_alu_output[32]), .R31(Slice_alu_output[31]), 
        .R30(Slice_alu_output[30]), .R29(Slice_alu_output[29]), 
        .R28(Slice_alu_output[28]), .R27(Slice_alu_output[27]), 
        .R26(Slice_alu_output[26]), .R25(Slice_alu_output[25]), 
        .R24(Slice_alu_output[24]), .R23(Slice_alu_output[23]), 
        .R22(Slice_alu_output[22]), .R21(Slice_alu_output[21]), 
        .R20(Slice_alu_output[20]), .R19(Slice_alu_output[19]), 
        .R18(Slice_alu_output[18]), .R17(Slice_alu_output[17]), 
        .R16(Slice_alu_output[16]), .R15(Slice_alu_output[15]), 
        .R14(Slice_alu_output[14]), .R13(Slice_alu_output[13]), 
        .R12(Slice_alu_output[12]), .R11(Slice_alu_output[11]), 
        .R10(Slice_alu_output[10]), .R9(Slice_alu_output[9]), 
        .R8(Slice_alu_output[8]), .R7(Slice_alu_output[7]), .R6(Slice_alu_output[6]), 
        .R5(Slice_alu_output[5]), .R4(Slice_alu_output[4]), .R3(Slice_alu_output[3]), 
        .R2(Slice_alu_output[2]), .R1(Slice_alu_output[1]), .R0(Slice_alu_output[0]), 
        .CO53(), .CO52(), .CO51(), .CO50(), .CO49(), .CO48(), .CO47(), .CO46(), 
        .CO45(), .CO44(), .CO43(), .CO42(), .CO41(), .CO40(), .CO39(), .CO38(), 
        .CO37(), .CO36(), .CO35(), .CO34(), .CO33(), .CO32(), .CO31(), .CO30(), 
        .CO29(), .CO28(), .CO27(), .CO26(), .CO25(), .CO24(), .CO23(), .CO22(), 
        .CO21(), .CO20(), .CO19(), .CO18(), .CO17(), .CO16(), .CO15(), .CO14(), 
        .CO13(), .CO12(), .CO11(), .CO10(), .CO9(), .CO8(), .CO7(), .CO6(), 
        .CO5(), .CO4(), .CO3(), .CO2(), .CO1(), .CO0(), .EQZ(EQZ), .EQZM(EQZM), 
        .EQOM(EQOM), .EQPAT(EQPAT), .EQPATB(EQPATB), .OVER(OVER), .UNDER(UNDER), 
        .OVERUNDER(), .SIGNEDR(Slice_alu_signedr_1_0));

    defparam dsp_mult_1.CLK3_DIV = "ENABLED" ;
    defparam dsp_mult_1.CLK2_DIV = "ENABLED" ;
    defparam dsp_mult_1.CLK1_DIV = "DISABLED" ;
    defparam dsp_mult_1.CLK0_DIV = "ENABLED" ;
    defparam dsp_mult_1.HIGHSPEED_CLK = "NONE" ;
    defparam dsp_mult_1.REG_INPUTC_RST = "RST0" ;
    defparam dsp_mult_1.REG_INPUTC_CE = "CE0" ;
    defparam dsp_mult_1.REG_INPUTC_CLK = "NONE" ;
    defparam dsp_mult_1.SOURCEB_MODE = "B_SHIFT" ;
    defparam dsp_mult_1.MULT_BYPASS = "DISABLED" ;
    defparam dsp_mult_1.CAS_MATCH_REG = "FALSE" ;
    defparam dsp_mult_1.RESETMODE = "SYNC" ;
    defparam dsp_mult_1.GSR = "ENABLED" ;
    defparam dsp_mult_1.REG_OUTPUT_RST = "RST0" ;
    defparam dsp_mult_1.REG_OUTPUT_CE = "CE0" ;
    defparam dsp_mult_1.REG_OUTPUT_CLK = "NONE" ;
    defparam dsp_mult_1.REG_PIPELINE_RST = "RST0" ;
    defparam dsp_mult_1.REG_PIPELINE_CE = "CE0" ;
    defparam dsp_mult_1.REG_PIPELINE_CLK = "CLK0" ;
    defparam dsp_mult_1.REG_INPUTB_RST = "RST0" ;
    defparam dsp_mult_1.REG_INPUTB_CE = "CE0" ;
    defparam dsp_mult_1.REG_INPUTB_CLK = "CLK0" ;
    defparam dsp_mult_1.REG_INPUTA_RST = "RST0" ;
    defparam dsp_mult_1.REG_INPUTA_CE = "CE1" ;
    defparam dsp_mult_1.REG_INPUTA_CLK = "CLK0" ;
    MULT18X18D dsp_mult_1 (.A17(DataAA[AAMemsel][17]), .A16(DataAA[AAMemsel][16]), 
		.A15(DataAA[AAMemsel][15]), .A14(DataAA[AAMemsel][14]), .A13(DataAA[AAMemsel][13]),
		.A12(DataAA[AAMemsel][12]), .A11(DataAA[AAMemsel][11]), .A10(DataAA[AAMemsel][10]), 
		.A9(DataAA[AAMemsel][9]), .A8(DataAA[AAMemsel][8]), .A7(DataAA[AAMemsel][7]), 
		.A6(DataAA[AAMemsel][6]), .A5(DataAA[AAMemsel][5]), .A4(DataAA[AAMemsel][4]), 
		.A3(DataAA[AAMemsel][3]), .A2(DataAA[AAMemsel][2]), .A1(DataAA[AAMemsel][1]), 
		.A0(DataAA[AAMemsel][0]), .B17(DataAB[ABMemsel][17]), .B16(DataAB[ABMemsel][16]), 
		.B15(DataAB[ABMemsel][15]), .B14(DataAB[ABMemsel][14]), .B13(DataAB[ABMemsel][13]),
		.B12(DataAB[ABMemsel][12]), .B11(DataAB[ABMemsel][11]), .B10(DataAB[ABMemsel][10]), 
		.B9(DataAB[ABMemsel][9]), .B8(DataAB[ABMemsel][8]), .B7(DataAB[ABMemsel][7]), 
		.B6(DataAB[ABMemsel][6]), .B5(DataAB[ABMemsel][5]), .B4(DataAB[ABMemsel][4]), 
		.B3(DataAB[ABMemsel][3]), .B2(DataAB[ABMemsel][2]), .B1(DataAB[ABMemsel][1]), 
		.B0(DataAB[ABMemsel][0]), .C17(scuba_vlo), 
        .C16(scuba_vlo), .C15(scuba_vlo), .C14(scuba_vlo), .C13(scuba_vlo), 
        .C12(scuba_vlo), .C11(scuba_vlo), .C10(scuba_vlo), .C9(scuba_vlo), 
        .C8(scuba_vlo), .C7(scuba_vlo), .C6(scuba_vlo), .C5(scuba_vlo), 
        .C4(scuba_vlo), .C3(scuba_vlo), .C2(scuba_vlo), .C1(scuba_vlo), 
        .C0(scuba_vlo), .SIGNEDA(SignAA), .SIGNEDB(SignAB), .SOURCEA(scuba_vlo), 
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
        .SROB3(), .SROB2(), .SROB1(), .SROB0(), .ROA17(Slice_0_mult_out_roa_0[17]), 
        .ROA16(Slice_0_mult_out_roa_0[16]), .ROA15(Slice_0_mult_out_roa_0[15]), 
        .ROA14(Slice_0_mult_out_roa_0[14]), .ROA13(Slice_0_mult_out_roa_0[13]), 
        .ROA12(Slice_0_mult_out_roa_0[12]), .ROA11(Slice_0_mult_out_roa_0[11]), 
        .ROA10(Slice_0_mult_out_roa_0[10]), .ROA9(Slice_0_mult_out_roa_0[9]), 
        .ROA8(Slice_0_mult_out_roa_0[8]), .ROA7(Slice_0_mult_out_roa_0[7]), 
        .ROA6(Slice_0_mult_out_roa_0[6]), .ROA5(Slice_0_mult_out_roa_0[5]), 
        .ROA4(Slice_0_mult_out_roa_0[4]), .ROA3(Slice_0_mult_out_roa_0[3]), 
        .ROA2(Slice_0_mult_out_roa_0[2]), .ROA1(Slice_0_mult_out_roa_0[1]), 
        .ROA0(Slice_0_mult_out_roa_0[0]), .ROB17(Slice_0_mult_out_rob_0[17]), 
        .ROB16(Slice_0_mult_out_rob_0[16]), .ROB15(Slice_0_mult_out_rob_0[15]), 
        .ROB14(Slice_0_mult_out_rob_0[14]), .ROB13(Slice_0_mult_out_rob_0[13]), 
        .ROB12(Slice_0_mult_out_rob_0[12]), .ROB11(Slice_0_mult_out_rob_0[11]), 
        .ROB10(Slice_0_mult_out_rob_0[10]), .ROB9(Slice_0_mult_out_rob_0[9]), 
        .ROB8(Slice_0_mult_out_rob_0[8]), .ROB7(Slice_0_mult_out_rob_0[7]), 
        .ROB6(Slice_0_mult_out_rob_0[6]), .ROB5(Slice_0_mult_out_rob_0[5]), 
        .ROB4(Slice_0_mult_out_rob_0[4]), .ROB3(Slice_0_mult_out_rob_0[3]), 
        .ROB2(Slice_0_mult_out_rob_0[2]), .ROB1(Slice_0_mult_out_rob_0[1]), 
        .ROB0(Slice_0_mult_out_rob_0[0]), .ROC17(), .ROC16(), .ROC15(), .ROC14(), 
        .ROC13(), .ROC12(), .ROC11(), .ROC10(), .ROC9(), .ROC8(), .ROC7(), 
        .ROC6(), .ROC5(), .ROC4(), .ROC3(), .ROC2(), .ROC1(), .ROC0(), .P35(Slice_0_mult_out_p_0[35]), 
        .P34(Slice_0_mult_out_p_0[34]), .P33(Slice_0_mult_out_p_0[33]), .P32(Slice_0_mult_out_p_0[32]), 
        .P31(Slice_0_mult_out_p_0[31]), .P30(Slice_0_mult_out_p_0[30]), .P29(Slice_0_mult_out_p_0[29]), 
        .P28(Slice_0_mult_out_p_0[28]), .P27(Slice_0_mult_out_p_0[27]), .P26(Slice_0_mult_out_p_0[26]), 
        .P25(Slice_0_mult_out_p_0[25]), .P24(Slice_0_mult_out_p_0[24]), .P23(Slice_0_mult_out_p_0[23]), 
        .P22(Slice_0_mult_out_p_0[22]), .P21(Slice_0_mult_out_p_0[21]), .P20(Slice_0_mult_out_p_0[20]), 
        .P19(Slice_0_mult_out_p_0[19]), .P18(Slice_0_mult_out_p_0[18]), .P17(Slice_0_mult_out_p_0[17]), 
        .P16(Slice_0_mult_out_p_0[16]), .P15(Slice_0_mult_out_p_0[15]), .P14(Slice_0_mult_out_p_0[14]), 
        .P13(Slice_0_mult_out_p_0[13]), .P12(Slice_0_mult_out_p_0[12]), .P11(Slice_0_mult_out_p_0[11]), 
        .P10(Slice_0_mult_out_p_0[10]), .P9(Slice_0_mult_out_p_0[9]), .P8(Slice_0_mult_out_p_0[8]), 
        .P7(Slice_0_mult_out_p_0[7]), .P6(Slice_0_mult_out_p_0[6]), .P5(Slice_0_mult_out_p_0[5]), 
        .P4(Slice_0_mult_out_p_0[4]), .P3(Slice_0_mult_out_p_0[3]), .P2(Slice_0_mult_out_p_0[2]), 
        .P1(Slice_0_mult_out_p_0[1]), .P0(Slice_0_mult_out_p_0[0]), .SIGNEDP(Slice_0_mult_out_signedp_0));

    VHI scuba_vhi_inst (.Z(scuba_vhi));

    VLO scuba_vlo_inst (.Z(scuba_vlo));

    defparam dsp_mult_0.CLK3_DIV = "ENABLED" ;
    defparam dsp_mult_0.CLK2_DIV = "ENABLED" ;
    defparam dsp_mult_0.CLK1_DIV = "DISABLED" ;
    defparam dsp_mult_0.CLK0_DIV = "ENABLED" ;
    defparam dsp_mult_0.HIGHSPEED_CLK = "NONE" ;
    defparam dsp_mult_0.REG_INPUTC_RST = "RST0" ;
    defparam dsp_mult_0.REG_INPUTC_CE = "CE0" ;
    defparam dsp_mult_0.REG_INPUTC_CLK = "NONE" ;
    defparam dsp_mult_0.SOURCEB_MODE = "B_SHIFT" ;
    defparam dsp_mult_0.MULT_BYPASS = "DISABLED" ;
    defparam dsp_mult_0.CAS_MATCH_REG = "FALSE" ;
    defparam dsp_mult_0.RESETMODE = "SYNC" ;
    defparam dsp_mult_0.GSR = "ENABLED" ;
    defparam dsp_mult_0.REG_OUTPUT_RST = "RST0" ;
    defparam dsp_mult_0.REG_OUTPUT_CE = "CE0" ;
    defparam dsp_mult_0.REG_OUTPUT_CLK = "NONE" ;
    defparam dsp_mult_0.REG_PIPELINE_RST = "RST0" ;
    defparam dsp_mult_0.REG_PIPELINE_CE = "CE0" ;
    defparam dsp_mult_0.REG_PIPELINE_CLK = "CLK0" ;
    defparam dsp_mult_0.REG_INPUTB_RST = "RST0" ;
    defparam dsp_mult_0.REG_INPUTB_CE = "CE0" ;
    defparam dsp_mult_0.REG_INPUTB_CLK = "CLK0" ;
    defparam dsp_mult_0.REG_INPUTA_RST = "RST0" ;
    defparam dsp_mult_0.REG_INPUTA_CE = "CE1" ;
    defparam dsp_mult_0.REG_INPUTA_CLK = "CLK0" ;
    MULT18X18D dsp_mult_0 (.A17(DataBA[BAMemsel][17]), .A16(DataBA[BAMemsel][16]), 
		.A15(DataBA[BAMemsel][15]), .A14(DataBA[BAMemsel][14]), .A13(DataBA[BAMemsel][13]),
		.A12(DataBA[BAMemsel][12]), .A11(DataBA[BAMemsel][11]), .A10(DataBA[BAMemsel][10]), 
		.A9(DataBA[BAMemsel][9]), .A8(DataBA[BAMemsel][8]), .A7(DataBA[BAMemsel][7]), 
		.A6(DataBA[BAMemsel][6]), .A5(DataBA[BAMemsel][5]), .A4(DataBA[BAMemsel][4]), 
		.A3(DataBA[BAMemsel][3]), .A2(DataBA[BAMemsel][2]), .A1(DataBA[BAMemsel][1]), 
		.A0(DataBA[BAMemsel][0]), .B17(DataBB[BBMemsel][17]), .B16(DataBB[BBMemsel][16]), 
		.B15(DataBB[BBMemsel][15]), .B14(DataBB[BBMemsel][14]), .B13(DataBB[BBMemsel][13]),
		.B12(DataBB[BBMemsel][12]), .B11(DataBB[BBMemsel][11]), .B10(DataBB[BBMemsel][10]), 
		.B9(DataBB[BBMemsel][9]), .B8(DataBB[BBMemsel][8]), .B7(DataBB[BBMemsel][7]), 
		.B6(DataBB[BBMemsel][6]), .B5(DataBB[BBMemsel][5]), .B4(DataBB[BBMemsel][4]), 
		.B3(DataBB[BBMemsel][3]), .B2(DataBB[BBMemsel][2]), .B1(DataBB[BBMemsel][1]), 
		.B0(DataBB[BBMemsel][0]), .C17(scuba_vlo), 
        .C16(scuba_vlo), .C15(scuba_vlo), .C14(scuba_vlo), .C13(scuba_vlo), 
        .C12(scuba_vlo), .C11(scuba_vlo), .C10(scuba_vlo), .C9(scuba_vlo), 
        .C8(scuba_vlo), .C7(scuba_vlo), .C6(scuba_vlo), .C5(scuba_vlo), 
        .C4(scuba_vlo), .C3(scuba_vlo), .C2(scuba_vlo), .C1(scuba_vlo),
        .C0(scuba_vlo), .SIGNEDA(SignBA), .SIGNEDB(SignBB), .SOURCEA(scuba_vlo), 
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
        .SROB3(), .SROB2(), .SROB1(), .SROB0(), .ROA17(Slice_0_mult_out_roa_1[17]), 
        .ROA16(Slice_0_mult_out_roa_1[16]), .ROA15(Slice_0_mult_out_roa_1[15]), 
        .ROA14(Slice_0_mult_out_roa_1[14]), .ROA13(Slice_0_mult_out_roa_1[13]), 
        .ROA12(Slice_0_mult_out_roa_1[12]), .ROA11(Slice_0_mult_out_roa_1[11]), 
        .ROA10(Slice_0_mult_out_roa_1[10]), .ROA9(Slice_0_mult_out_roa_1[9]), 
        .ROA8(Slice_0_mult_out_roa_1[8]), .ROA7(Slice_0_mult_out_roa_1[7]), 
        .ROA6(Slice_0_mult_out_roa_1[6]), .ROA5(Slice_0_mult_out_roa_1[5]), 
        .ROA4(Slice_0_mult_out_roa_1[4]), .ROA3(Slice_0_mult_out_roa_1[3]), 
        .ROA2(Slice_0_mult_out_roa_1[2]), .ROA1(Slice_0_mult_out_roa_1[1]), 
        .ROA0(Slice_0_mult_out_roa_1[0]), .ROB17(Slice_0_mult_out_rob_1[17]), 
        .ROB16(Slice_0_mult_out_rob_1[16]), .ROB15(Slice_0_mult_out_rob_1[15]), 
        .ROB14(Slice_0_mult_out_rob_1[14]), .ROB13(Slice_0_mult_out_rob_1[13]), 
        .ROB12(Slice_0_mult_out_rob_1[12]), .ROB11(Slice_0_mult_out_rob_1[11]), 
        .ROB10(Slice_0_mult_out_rob_1[10]), .ROB9(Slice_0_mult_out_rob_1[9]), 
        .ROB8(Slice_0_mult_out_rob_1[8]), .ROB7(Slice_0_mult_out_rob_1[7]), 
        .ROB6(Slice_0_mult_out_rob_1[6]), .ROB5(Slice_0_mult_out_rob_1[5]), 
        .ROB4(Slice_0_mult_out_rob_1[4]), .ROB3(Slice_0_mult_out_rob_1[3]), 
        .ROB2(Slice_0_mult_out_rob_1[2]), .ROB1(Slice_0_mult_out_rob_1[1]), 
        .ROB0(Slice_0_mult_out_rob_1[0]), .ROC17(), .ROC16(), .ROC15(), .ROC14(), 
        .ROC13(), .ROC12(), .ROC11(), .ROC10(), .ROC9(), .ROC8(), .ROC7(), 
        .ROC6(), .ROC5(), .ROC4(), .ROC3(), .ROC2(), .ROC1(), .ROC0(), .P35(Slice_0_mult_out_p_1[35]), 
        .P34(Slice_0_mult_out_p_1[34]), .P33(Slice_0_mult_out_p_1[33]), .P32(Slice_0_mult_out_p_1[32]), 
        .P31(Slice_0_mult_out_p_1[31]), .P30(Slice_0_mult_out_p_1[30]), .P29(Slice_0_mult_out_p_1[29]), 
        .P28(Slice_0_mult_out_p_1[28]), .P27(Slice_0_mult_out_p_1[27]), .P26(Slice_0_mult_out_p_1[26]), 
        .P25(Slice_0_mult_out_p_1[25]), .P24(Slice_0_mult_out_p_1[24]), .P23(Slice_0_mult_out_p_1[23]), 
        .P22(Slice_0_mult_out_p_1[22]), .P21(Slice_0_mult_out_p_1[21]), .P20(Slice_0_mult_out_p_1[20]), 
        .P19(Slice_0_mult_out_p_1[19]), .P18(Slice_0_mult_out_p_1[18]), .P17(Slice_0_mult_out_p_1[17]), 
        .P16(Slice_0_mult_out_p_1[16]), .P15(Slice_0_mult_out_p_1[15]), .P14(Slice_0_mult_out_p_1[14]), 
        .P13(Slice_0_mult_out_p_1[13]), .P12(Slice_0_mult_out_p_1[12]), .P11(Slice_0_mult_out_p_1[11]), 
        .P10(Slice_0_mult_out_p_1[10]), .P9(Slice_0_mult_out_p_1[9]), .P8(Slice_0_mult_out_p_1[8]), 
        .P7(Slice_0_mult_out_p_1[7]), .P6(Slice_0_mult_out_p_1[6]), .P5(Slice_0_mult_out_p_1[5]), 
        .P4(Slice_0_mult_out_p_1[4]), .P3(Slice_0_mult_out_p_1[3]), .P2(Slice_0_mult_out_p_1[2]), 
        .P1(Slice_0_mult_out_p_1[1]), .P0(Slice_0_mult_out_p_1[0]), .SIGNEDP(Slice_0_mult_out_signedp_1));

    assign Result[35] = Slice_alu_output[53];
	assign Result[34] = Slice_alu_output[52];
	assign Result[33] = Slice_alu_output[51];
	assign Result[32] = Slice_alu_output[50];
	assign Result[31] = Slice_alu_output[49];
	assign Result[30] = Slice_alu_output[48];
	assign Result[29] = Slice_alu_output[47];
	assign Result[28] = Slice_alu_output[46];
	assign Result[27] = Slice_alu_output[45];
	assign Result[26] = Slice_alu_output[44];
	assign Result[25] = Slice_alu_output[43];
	assign Result[24] = Slice_alu_output[42];
	assign Result[23] = Slice_alu_output[41];
	assign Result[22] = Slice_alu_output[40];
	assign Result[21] = Slice_alu_output[39];
	assign Result[20] = Slice_alu_output[38];
	assign Result[19] = Slice_alu_output[37];
	assign Result[18] = Slice_alu_output[36];
	assign Result[17] = Slice_alu_output[35];
	assign Result[16] = Slice_alu_output[34];
	assign Result[15] = Slice_alu_output[33];
	assign Result[14] = Slice_alu_output[32];
	assign Result[13] = Slice_alu_output[31];
	assign Result[12] = Slice_alu_output[30];
	assign Result[11] = Slice_alu_output[29];
	assign Result[10] = Slice_alu_output[28];
	assign Result[9] = Slice_alu_output[27];
	assign Result[8] = Slice_alu_output[26];
	assign Result[7] = Slice_alu_output[25];
	assign Result[6] = Slice_alu_output[24];
	assign Result[5] = Slice_alu_output[23];
	assign Result[4] = Slice_alu_output[22];
	assign Result[3] = Slice_alu_output[21];
	assign Result[2] = Slice_alu_output[20];
	assign Result[1] = Slice_alu_output[19];
	assign Result[0] = Slice_alu_output[18];
	
	assign Result54[0] = Slice_alu_output[0];
	assign Result54[1] = Slice_alu_output[1];
	assign Result54[2] = Slice_alu_output[2];
	assign Result54[3] = Slice_alu_output[3];
	assign Result54[4] = Slice_alu_output[4];
	assign Result54[5] = Slice_alu_output[5];
	assign Result54[6] = Slice_alu_output[6];
	assign Result54[7] = Slice_alu_output[7];
	assign Result54[8] = Slice_alu_output[8];
	assign Result54[9] = Slice_alu_output[9];
	assign Result54[10] = Slice_alu_output[10];
	assign Result54[11] = Slice_alu_output[11];
	assign Result54[12] = Slice_alu_output[12];
	assign Result54[13] = Slice_alu_output[13];
	assign Result54[14] = Slice_alu_output[14];
	assign Result54[15] = Slice_alu_output[15];
	assign Result54[16] = Slice_alu_output[16];
	assign Result54[17] = Slice_alu_output[17];
	assign Result54[18] = Slice_alu_output[18];
	assign Result54[19] = Slice_alu_output[19];
	assign Result54[20] = Slice_alu_output[20];
	assign Result54[21] = Slice_alu_output[21];
	assign Result54[22] = Slice_alu_output[22];
	assign Result54[23] = Slice_alu_output[23];
	assign Result54[24] = Slice_alu_output[24];
	assign Result54[25] = Slice_alu_output[25];
	assign Result54[26] = Slice_alu_output[26];
	assign Result54[27] = Slice_alu_output[27];
	assign Result54[28] = Slice_alu_output[28];
	assign Result54[29] = Slice_alu_output[29];
	assign Result54[30] = Slice_alu_output[30];
	assign Result54[31] = Slice_alu_output[31];
	assign Result54[32] = Slice_alu_output[32];
	assign Result54[33] = Slice_alu_output[33];
	assign Result54[34] = Slice_alu_output[34];
	assign Result54[35] = Slice_alu_output[35];
	assign Result54[36] = Slice_alu_output[36];
	assign Result54[37] = Slice_alu_output[37];
	assign Result54[38] = Slice_alu_output[38];
	assign Result54[39] = Slice_alu_output[39];
	assign Result54[40] = Slice_alu_output[40];
	assign Result54[41] = Slice_alu_output[41];
	assign Result54[42] = Slice_alu_output[42];
	assign Result54[43] = Slice_alu_output[43];
	assign Result54[44] = Slice_alu_output[44];
	assign Result54[45] = Slice_alu_output[45];
	assign Result54[46] = Slice_alu_output[46];
	assign Result54[47] = Slice_alu_output[47];
	assign Result54[48] = Slice_alu_output[48];
	assign Result54[49] = Slice_alu_output[49];
	assign Result54[50] = Slice_alu_output[50];
	assign Result54[51] = Slice_alu_output[51];
	assign Result54[52] = Slice_alu_output[52];
	assign Result54[53] = Slice_alu_output[53];


endmodule
