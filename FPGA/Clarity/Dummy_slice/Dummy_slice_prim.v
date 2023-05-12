// Verilog netlist produced by program LSE :  version Diamond (64-bit) 3.12.0.240.2
// Netlist written on Fri May 12 16:09:19 2023
//
// Verilog Description of module Dummy_slice
//

module Dummy_slice (CLK0, CE0, RST0, AA, AB, BA, BB, C, AMuxsel, 
            BMuxsel, CMuxsel, Opcode, Cin, SignCin, Result, SignR, 
            EQZ, EQZM, EQOM, EQPAT, EQPATB, OVER, UNDER, SROA, 
            SROB) /* synthesis NGD_DRC_MASK=1, syn_module_defined=1 */ ;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(8[8:19])
    input CLK0;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(11[16:20])
    input CE0;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(12[16:19])
    input RST0;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(13[16:20])
    input [17:0]AA;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(14[23:25])
    input [17:0]AB;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(15[23:25])
    input [17:0]BA;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(16[23:25])
    input [17:0]BB;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(17[23:25])
    input [53:0]C;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(18[23:24])
    input [1:0]AMuxsel;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(19[22:29])
    input [1:0]BMuxsel;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(20[22:29])
    input [2:0]CMuxsel;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(21[22:29])
    input [3:0]Opcode;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(22[22:28])
    input [53:0]Cin;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(23[23:26])
    input SignCin;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(24[16:23])
    output [53:0]Result;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(25[24:30])
    output SignR;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(26[17:22])
    output EQZ;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(27[17:20])
    output EQZM;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(28[17:21])
    output EQOM;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(29[17:21])
    output EQPAT;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(30[17:22])
    output EQPATB;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(31[17:23])
    output OVER;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(32[17:21])
    output UNDER;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(33[17:22])
    output [17:0]SROA;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(34[24:28])
    output [17:0]SROB;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(35[24:28])
    
    
    wire Dummy_slice_0_mult_out_rob_0_17, Dummy_slice_0_mult_out_roa_0_17, 
        Dummy_slice_0_mult_out_rob_0_16, Dummy_slice_0_mult_out_roa_0_16, 
        Dummy_slice_0_mult_out_rob_0_15, Dummy_slice_0_mult_out_roa_0_15, 
        Dummy_slice_0_mult_out_rob_0_14, Dummy_slice_0_mult_out_roa_0_14, 
        Dummy_slice_0_mult_out_rob_0_13, Dummy_slice_0_mult_out_roa_0_13, 
        Dummy_slice_0_mult_out_rob_0_12, Dummy_slice_0_mult_out_roa_0_12, 
        Dummy_slice_0_mult_out_rob_0_11, Dummy_slice_0_mult_out_roa_0_11, 
        Dummy_slice_0_mult_out_rob_0_10, Dummy_slice_0_mult_out_roa_0_10, 
        Dummy_slice_0_mult_out_rob_0_9, Dummy_slice_0_mult_out_roa_0_9, 
        Dummy_slice_0_mult_out_rob_0_8, Dummy_slice_0_mult_out_roa_0_8, 
        Dummy_slice_0_mult_out_rob_0_7, Dummy_slice_0_mult_out_roa_0_7, 
        Dummy_slice_0_mult_out_rob_0_6, Dummy_slice_0_mult_out_roa_0_6, 
        Dummy_slice_0_mult_out_rob_0_5, Dummy_slice_0_mult_out_roa_0_5, 
        Dummy_slice_0_mult_out_rob_0_4, Dummy_slice_0_mult_out_roa_0_4, 
        Dummy_slice_0_mult_out_rob_0_3, Dummy_slice_0_mult_out_roa_0_3, 
        Dummy_slice_0_mult_out_rob_0_2, Dummy_slice_0_mult_out_roa_0_2, 
        Dummy_slice_0_mult_out_rob_0_1, Dummy_slice_0_mult_out_roa_0_1, 
        Dummy_slice_0_mult_out_rob_0_0, Dummy_slice_0_mult_out_roa_0_0, 
        Dummy_slice_0_mult_out_p_0_35, Dummy_slice_0_mult_out_p_0_34, Dummy_slice_0_mult_out_p_0_33, 
        Dummy_slice_0_mult_out_p_0_32, Dummy_slice_0_mult_out_p_0_31, Dummy_slice_0_mult_out_p_0_30, 
        Dummy_slice_0_mult_out_p_0_29, Dummy_slice_0_mult_out_p_0_28, Dummy_slice_0_mult_out_p_0_27, 
        Dummy_slice_0_mult_out_p_0_26, Dummy_slice_0_mult_out_p_0_25, Dummy_slice_0_mult_out_p_0_24, 
        Dummy_slice_0_mult_out_p_0_23, Dummy_slice_0_mult_out_p_0_22, Dummy_slice_0_mult_out_p_0_21, 
        Dummy_slice_0_mult_out_p_0_20, Dummy_slice_0_mult_out_p_0_19, Dummy_slice_0_mult_out_p_0_18, 
        Dummy_slice_0_mult_out_p_0_17, Dummy_slice_0_mult_out_p_0_16, Dummy_slice_0_mult_out_p_0_15, 
        Dummy_slice_0_mult_out_p_0_14, Dummy_slice_0_mult_out_p_0_13, Dummy_slice_0_mult_out_p_0_12, 
        Dummy_slice_0_mult_out_p_0_11, Dummy_slice_0_mult_out_p_0_10, Dummy_slice_0_mult_out_p_0_9, 
        Dummy_slice_0_mult_out_p_0_8, Dummy_slice_0_mult_out_p_0_7, Dummy_slice_0_mult_out_p_0_6, 
        Dummy_slice_0_mult_out_p_0_5, Dummy_slice_0_mult_out_p_0_4, Dummy_slice_0_mult_out_p_0_3, 
        Dummy_slice_0_mult_out_p_0_2, Dummy_slice_0_mult_out_p_0_1, Dummy_slice_0_mult_out_p_0_0, 
        Dummy_slice_0_mult_out_signedp_0, Dummy_slice_0_mult_out_rob_1_17, 
        Dummy_slice_0_mult_out_roa_1_17, Dummy_slice_0_mult_out_rob_1_16, 
        Dummy_slice_0_mult_out_roa_1_16, Dummy_slice_0_mult_out_rob_1_15, 
        Dummy_slice_0_mult_out_roa_1_15, Dummy_slice_0_mult_out_rob_1_14, 
        Dummy_slice_0_mult_out_roa_1_14, Dummy_slice_0_mult_out_rob_1_13, 
        Dummy_slice_0_mult_out_roa_1_13, Dummy_slice_0_mult_out_rob_1_12, 
        Dummy_slice_0_mult_out_roa_1_12, Dummy_slice_0_mult_out_rob_1_11, 
        Dummy_slice_0_mult_out_roa_1_11, Dummy_slice_0_mult_out_rob_1_10, 
        Dummy_slice_0_mult_out_roa_1_10, Dummy_slice_0_mult_out_rob_1_9, 
        Dummy_slice_0_mult_out_roa_1_9, Dummy_slice_0_mult_out_rob_1_8, 
        Dummy_slice_0_mult_out_roa_1_8, Dummy_slice_0_mult_out_rob_1_7, 
        Dummy_slice_0_mult_out_roa_1_7, Dummy_slice_0_mult_out_rob_1_6, 
        Dummy_slice_0_mult_out_roa_1_6, Dummy_slice_0_mult_out_rob_1_5, 
        Dummy_slice_0_mult_out_roa_1_5, Dummy_slice_0_mult_out_rob_1_4, 
        Dummy_slice_0_mult_out_roa_1_4, Dummy_slice_0_mult_out_rob_1_3, 
        Dummy_slice_0_mult_out_roa_1_3, Dummy_slice_0_mult_out_rob_1_2, 
        Dummy_slice_0_mult_out_roa_1_2, Dummy_slice_0_mult_out_rob_1_1, 
        Dummy_slice_0_mult_out_roa_1_1, Dummy_slice_0_mult_out_rob_1_0, 
        Dummy_slice_0_mult_out_roa_1_0, Dummy_slice_0_mult_out_p_1_35, Dummy_slice_0_mult_out_p_1_34, 
        Dummy_slice_0_mult_out_p_1_33, Dummy_slice_0_mult_out_p_1_32, Dummy_slice_0_mult_out_p_1_31, 
        Dummy_slice_0_mult_out_p_1_30, Dummy_slice_0_mult_out_p_1_29, Dummy_slice_0_mult_out_p_1_28, 
        Dummy_slice_0_mult_out_p_1_27, Dummy_slice_0_mult_out_p_1_26, Dummy_slice_0_mult_out_p_1_25, 
        Dummy_slice_0_mult_out_p_1_24, Dummy_slice_0_mult_out_p_1_23, Dummy_slice_0_mult_out_p_1_22, 
        Dummy_slice_0_mult_out_p_1_21, Dummy_slice_0_mult_out_p_1_20, Dummy_slice_0_mult_out_p_1_19, 
        Dummy_slice_0_mult_out_p_1_18, Dummy_slice_0_mult_out_p_1_17, Dummy_slice_0_mult_out_p_1_16, 
        Dummy_slice_0_mult_out_p_1_15, Dummy_slice_0_mult_out_p_1_14, Dummy_slice_0_mult_out_p_1_13, 
        Dummy_slice_0_mult_out_p_1_12, Dummy_slice_0_mult_out_p_1_11, Dummy_slice_0_mult_out_p_1_10, 
        Dummy_slice_0_mult_out_p_1_9, Dummy_slice_0_mult_out_p_1_8, Dummy_slice_0_mult_out_p_1_7, 
        Dummy_slice_0_mult_out_p_1_6, Dummy_slice_0_mult_out_p_1_5, Dummy_slice_0_mult_out_p_1_4, 
        Dummy_slice_0_mult_out_p_1_3, Dummy_slice_0_mult_out_p_1_2, Dummy_slice_0_mult_out_p_1_1, 
        Dummy_slice_0_mult_out_p_1_0, Dummy_slice_0_mult_out_signedp_1, 
        scuba_vlo, VCC_net;
    
    VLO scuba_vlo_inst (.Z(scuba_vlo));
    MULT18X18D dsp_mult_0 (.A17(BA[17]), .A16(BA[16]), .A15(BA[15]), .A14(BA[14]), 
            .A13(BA[13]), .A12(BA[12]), .A11(BA[11]), .A10(BA[10]), 
            .A9(BA[9]), .A8(BA[8]), .A7(BA[7]), .A6(BA[6]), .A5(BA[5]), 
            .A4(BA[4]), .A3(BA[3]), .A2(BA[2]), .A1(BA[1]), .A0(BA[0]), 
            .B17(BB[17]), .B16(BB[16]), .B15(BB[15]), .B14(BB[14]), 
            .B13(BB[13]), .B12(BB[12]), .B11(BB[11]), .B10(BB[10]), 
            .B9(BB[9]), .B8(BB[8]), .B7(BB[7]), .B6(BB[6]), .B5(BB[5]), 
            .B4(BB[4]), .B3(BB[3]), .B2(BB[2]), .B1(BB[1]), .B0(BB[0]), 
            .C17(scuba_vlo), .C16(scuba_vlo), .C15(scuba_vlo), .C14(scuba_vlo), 
            .C13(scuba_vlo), .C12(scuba_vlo), .C11(scuba_vlo), .C10(scuba_vlo), 
            .C9(scuba_vlo), .C8(scuba_vlo), .C7(scuba_vlo), .C6(scuba_vlo), 
            .C5(scuba_vlo), .C4(scuba_vlo), .C3(scuba_vlo), .C2(scuba_vlo), 
            .C1(scuba_vlo), .C0(scuba_vlo), .SIGNEDA(VCC_net), .SIGNEDB(VCC_net), 
            .SOURCEA(scuba_vlo), .SOURCEB(scuba_vlo), .CLK3(scuba_vlo), 
            .CLK2(scuba_vlo), .CLK1(scuba_vlo), .CLK0(CLK0), .CE3(VCC_net), 
            .CE2(VCC_net), .CE1(VCC_net), .CE0(CE0), .RST3(scuba_vlo), 
            .RST2(scuba_vlo), .RST1(scuba_vlo), .RST0(RST0), .SRIA17(scuba_vlo), 
            .SRIA16(scuba_vlo), .SRIA15(scuba_vlo), .SRIA14(scuba_vlo), 
            .SRIA13(scuba_vlo), .SRIA12(scuba_vlo), .SRIA11(scuba_vlo), 
            .SRIA10(scuba_vlo), .SRIA9(scuba_vlo), .SRIA8(scuba_vlo), 
            .SRIA7(scuba_vlo), .SRIA6(scuba_vlo), .SRIA5(scuba_vlo), .SRIA4(scuba_vlo), 
            .SRIA3(scuba_vlo), .SRIA2(scuba_vlo), .SRIA1(scuba_vlo), .SRIA0(scuba_vlo), 
            .SRIB17(scuba_vlo), .SRIB16(scuba_vlo), .SRIB15(scuba_vlo), 
            .SRIB14(scuba_vlo), .SRIB13(scuba_vlo), .SRIB12(scuba_vlo), 
            .SRIB11(scuba_vlo), .SRIB10(scuba_vlo), .SRIB9(scuba_vlo), 
            .SRIB8(scuba_vlo), .SRIB7(scuba_vlo), .SRIB6(scuba_vlo), .SRIB5(scuba_vlo), 
            .SRIB4(scuba_vlo), .SRIB3(scuba_vlo), .SRIB2(scuba_vlo), .SRIB1(scuba_vlo), 
            .SRIB0(scuba_vlo), .SROA17(SROA[17]), .SROA16(SROA[16]), .SROA15(SROA[15]), 
            .SROA14(SROA[14]), .SROA13(SROA[13]), .SROA12(SROA[12]), .SROA11(SROA[11]), 
            .SROA10(SROA[10]), .SROA9(SROA[9]), .SROA8(SROA[8]), .SROA7(SROA[7]), 
            .SROA6(SROA[6]), .SROA5(SROA[5]), .SROA4(SROA[4]), .SROA3(SROA[3]), 
            .SROA2(SROA[2]), .SROA1(SROA[1]), .SROA0(SROA[0]), .SROB17(SROB[17]), 
            .SROB16(SROB[16]), .SROB15(SROB[15]), .SROB14(SROB[14]), .SROB13(SROB[13]), 
            .SROB12(SROB[12]), .SROB11(SROB[11]), .SROB10(SROB[10]), .SROB9(SROB[9]), 
            .SROB8(SROB[8]), .SROB7(SROB[7]), .SROB6(SROB[6]), .SROB5(SROB[5]), 
            .SROB4(SROB[4]), .SROB3(SROB[3]), .SROB2(SROB[2]), .SROB1(SROB[1]), 
            .SROB0(SROB[0]), .ROA17(Dummy_slice_0_mult_out_roa_1_17), .ROA16(Dummy_slice_0_mult_out_roa_1_16), 
            .ROA15(Dummy_slice_0_mult_out_roa_1_15), .ROA14(Dummy_slice_0_mult_out_roa_1_14), 
            .ROA13(Dummy_slice_0_mult_out_roa_1_13), .ROA12(Dummy_slice_0_mult_out_roa_1_12), 
            .ROA11(Dummy_slice_0_mult_out_roa_1_11), .ROA10(Dummy_slice_0_mult_out_roa_1_10), 
            .ROA9(Dummy_slice_0_mult_out_roa_1_9), .ROA8(Dummy_slice_0_mult_out_roa_1_8), 
            .ROA7(Dummy_slice_0_mult_out_roa_1_7), .ROA6(Dummy_slice_0_mult_out_roa_1_6), 
            .ROA5(Dummy_slice_0_mult_out_roa_1_5), .ROA4(Dummy_slice_0_mult_out_roa_1_4), 
            .ROA3(Dummy_slice_0_mult_out_roa_1_3), .ROA2(Dummy_slice_0_mult_out_roa_1_2), 
            .ROA1(Dummy_slice_0_mult_out_roa_1_1), .ROA0(Dummy_slice_0_mult_out_roa_1_0), 
            .ROB17(Dummy_slice_0_mult_out_rob_1_17), .ROB16(Dummy_slice_0_mult_out_rob_1_16), 
            .ROB15(Dummy_slice_0_mult_out_rob_1_15), .ROB14(Dummy_slice_0_mult_out_rob_1_14), 
            .ROB13(Dummy_slice_0_mult_out_rob_1_13), .ROB12(Dummy_slice_0_mult_out_rob_1_12), 
            .ROB11(Dummy_slice_0_mult_out_rob_1_11), .ROB10(Dummy_slice_0_mult_out_rob_1_10), 
            .ROB9(Dummy_slice_0_mult_out_rob_1_9), .ROB8(Dummy_slice_0_mult_out_rob_1_8), 
            .ROB7(Dummy_slice_0_mult_out_rob_1_7), .ROB6(Dummy_slice_0_mult_out_rob_1_6), 
            .ROB5(Dummy_slice_0_mult_out_rob_1_5), .ROB4(Dummy_slice_0_mult_out_rob_1_4), 
            .ROB3(Dummy_slice_0_mult_out_rob_1_3), .ROB2(Dummy_slice_0_mult_out_rob_1_2), 
            .ROB1(Dummy_slice_0_mult_out_rob_1_1), .ROB0(Dummy_slice_0_mult_out_rob_1_0), 
            .P35(Dummy_slice_0_mult_out_p_1_35), .P34(Dummy_slice_0_mult_out_p_1_34), 
            .P33(Dummy_slice_0_mult_out_p_1_33), .P32(Dummy_slice_0_mult_out_p_1_32), 
            .P31(Dummy_slice_0_mult_out_p_1_31), .P30(Dummy_slice_0_mult_out_p_1_30), 
            .P29(Dummy_slice_0_mult_out_p_1_29), .P28(Dummy_slice_0_mult_out_p_1_28), 
            .P27(Dummy_slice_0_mult_out_p_1_27), .P26(Dummy_slice_0_mult_out_p_1_26), 
            .P25(Dummy_slice_0_mult_out_p_1_25), .P24(Dummy_slice_0_mult_out_p_1_24), 
            .P23(Dummy_slice_0_mult_out_p_1_23), .P22(Dummy_slice_0_mult_out_p_1_22), 
            .P21(Dummy_slice_0_mult_out_p_1_21), .P20(Dummy_slice_0_mult_out_p_1_20), 
            .P19(Dummy_slice_0_mult_out_p_1_19), .P18(Dummy_slice_0_mult_out_p_1_18), 
            .P17(Dummy_slice_0_mult_out_p_1_17), .P16(Dummy_slice_0_mult_out_p_1_16), 
            .P15(Dummy_slice_0_mult_out_p_1_15), .P14(Dummy_slice_0_mult_out_p_1_14), 
            .P13(Dummy_slice_0_mult_out_p_1_13), .P12(Dummy_slice_0_mult_out_p_1_12), 
            .P11(Dummy_slice_0_mult_out_p_1_11), .P10(Dummy_slice_0_mult_out_p_1_10), 
            .P9(Dummy_slice_0_mult_out_p_1_9), .P8(Dummy_slice_0_mult_out_p_1_8), 
            .P7(Dummy_slice_0_mult_out_p_1_7), .P6(Dummy_slice_0_mult_out_p_1_6), 
            .P5(Dummy_slice_0_mult_out_p_1_5), .P4(Dummy_slice_0_mult_out_p_1_4), 
            .P3(Dummy_slice_0_mult_out_p_1_3), .P2(Dummy_slice_0_mult_out_p_1_2), 
            .P1(Dummy_slice_0_mult_out_p_1_1), .P0(Dummy_slice_0_mult_out_p_1_0), 
            .SIGNEDP(Dummy_slice_0_mult_out_signedp_1)) /* synthesis syn_instantiated=1 */ ;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(563[16] 634[87])
    defparam dsp_mult_0.REG_INPUTA_CLK = "CLK0";
    defparam dsp_mult_0.REG_INPUTA_CE = "CE0";
    defparam dsp_mult_0.REG_INPUTA_RST = "RST0";
    defparam dsp_mult_0.REG_INPUTB_CLK = "CLK0";
    defparam dsp_mult_0.REG_INPUTB_CE = "CE0";
    defparam dsp_mult_0.REG_INPUTB_RST = "RST0";
    defparam dsp_mult_0.REG_INPUTC_CLK = "NONE";
    defparam dsp_mult_0.REG_INPUTC_CE = "CE0";
    defparam dsp_mult_0.REG_INPUTC_RST = "RST0";
    defparam dsp_mult_0.REG_PIPELINE_CLK = "CLK0";
    defparam dsp_mult_0.REG_PIPELINE_CE = "CE0";
    defparam dsp_mult_0.REG_PIPELINE_RST = "RST0";
    defparam dsp_mult_0.REG_OUTPUT_CLK = "NONE";
    defparam dsp_mult_0.REG_OUTPUT_CE = "CE0";
    defparam dsp_mult_0.REG_OUTPUT_RST = "RST0";
    defparam dsp_mult_0.CLK0_DIV = "ENABLED";
    defparam dsp_mult_0.CLK1_DIV = "DISABLED";
    defparam dsp_mult_0.CLK2_DIV = "ENABLED";
    defparam dsp_mult_0.CLK3_DIV = "ENABLED";
    defparam dsp_mult_0.HIGHSPEED_CLK = "NONE";
    defparam dsp_mult_0.GSR = "ENABLED";
    defparam dsp_mult_0.CAS_MATCH_REG = "TRUE";
    defparam dsp_mult_0.SOURCEB_MODE = "B_SHIFT";
    defparam dsp_mult_0.MULT_BYPASS = "DISABLED";
    defparam dsp_mult_0.RESETMODE = "SYNC";
    ALU54B dsp_alu_0 (.CE3(VCC_net), .CE2(VCC_net), .CE1(VCC_net), .CE0(CE0), 
           .CLK3(scuba_vlo), .CLK2(scuba_vlo), .CLK1(CLK0), .CLK0(CLK0), 
           .RST3(scuba_vlo), .RST2(scuba_vlo), .RST1(scuba_vlo), .RST0(RST0), 
           .SIGNEDIA(Dummy_slice_0_mult_out_signedp_0), .SIGNEDIB(Dummy_slice_0_mult_out_signedp_1), 
           .SIGNEDCIN(SignCin), .A35(Dummy_slice_0_mult_out_rob_0_17), .A34(Dummy_slice_0_mult_out_rob_0_16), 
           .A33(Dummy_slice_0_mult_out_rob_0_15), .A32(Dummy_slice_0_mult_out_rob_0_14), 
           .A31(Dummy_slice_0_mult_out_rob_0_13), .A30(Dummy_slice_0_mult_out_rob_0_12), 
           .A29(Dummy_slice_0_mult_out_rob_0_11), .A28(Dummy_slice_0_mult_out_rob_0_10), 
           .A27(Dummy_slice_0_mult_out_rob_0_9), .A26(Dummy_slice_0_mult_out_rob_0_8), 
           .A25(Dummy_slice_0_mult_out_rob_0_7), .A24(Dummy_slice_0_mult_out_rob_0_6), 
           .A23(Dummy_slice_0_mult_out_rob_0_5), .A22(Dummy_slice_0_mult_out_rob_0_4), 
           .A21(Dummy_slice_0_mult_out_rob_0_3), .A20(Dummy_slice_0_mult_out_rob_0_2), 
           .A19(Dummy_slice_0_mult_out_rob_0_1), .A18(Dummy_slice_0_mult_out_rob_0_0), 
           .A17(Dummy_slice_0_mult_out_roa_0_17), .A16(Dummy_slice_0_mult_out_roa_0_16), 
           .A15(Dummy_slice_0_mult_out_roa_0_15), .A14(Dummy_slice_0_mult_out_roa_0_14), 
           .A13(Dummy_slice_0_mult_out_roa_0_13), .A12(Dummy_slice_0_mult_out_roa_0_12), 
           .A11(Dummy_slice_0_mult_out_roa_0_11), .A10(Dummy_slice_0_mult_out_roa_0_10), 
           .A9(Dummy_slice_0_mult_out_roa_0_9), .A8(Dummy_slice_0_mult_out_roa_0_8), 
           .A7(Dummy_slice_0_mult_out_roa_0_7), .A6(Dummy_slice_0_mult_out_roa_0_6), 
           .A5(Dummy_slice_0_mult_out_roa_0_5), .A4(Dummy_slice_0_mult_out_roa_0_4), 
           .A3(Dummy_slice_0_mult_out_roa_0_3), .A2(Dummy_slice_0_mult_out_roa_0_2), 
           .A1(Dummy_slice_0_mult_out_roa_0_1), .A0(Dummy_slice_0_mult_out_roa_0_0), 
           .B35(Dummy_slice_0_mult_out_rob_1_17), .B34(Dummy_slice_0_mult_out_rob_1_16), 
           .B33(Dummy_slice_0_mult_out_rob_1_15), .B32(Dummy_slice_0_mult_out_rob_1_14), 
           .B31(Dummy_slice_0_mult_out_rob_1_13), .B30(Dummy_slice_0_mult_out_rob_1_12), 
           .B29(Dummy_slice_0_mult_out_rob_1_11), .B28(Dummy_slice_0_mult_out_rob_1_10), 
           .B27(Dummy_slice_0_mult_out_rob_1_9), .B26(Dummy_slice_0_mult_out_rob_1_8), 
           .B25(Dummy_slice_0_mult_out_rob_1_7), .B24(Dummy_slice_0_mult_out_rob_1_6), 
           .B23(Dummy_slice_0_mult_out_rob_1_5), .B22(Dummy_slice_0_mult_out_rob_1_4), 
           .B21(Dummy_slice_0_mult_out_rob_1_3), .B20(Dummy_slice_0_mult_out_rob_1_2), 
           .B19(Dummy_slice_0_mult_out_rob_1_1), .B18(Dummy_slice_0_mult_out_rob_1_0), 
           .B17(Dummy_slice_0_mult_out_roa_1_17), .B16(Dummy_slice_0_mult_out_roa_1_16), 
           .B15(Dummy_slice_0_mult_out_roa_1_15), .B14(Dummy_slice_0_mult_out_roa_1_14), 
           .B13(Dummy_slice_0_mult_out_roa_1_13), .B12(Dummy_slice_0_mult_out_roa_1_12), 
           .B11(Dummy_slice_0_mult_out_roa_1_11), .B10(Dummy_slice_0_mult_out_roa_1_10), 
           .B9(Dummy_slice_0_mult_out_roa_1_9), .B8(Dummy_slice_0_mult_out_roa_1_8), 
           .B7(Dummy_slice_0_mult_out_roa_1_7), .B6(Dummy_slice_0_mult_out_roa_1_6), 
           .B5(Dummy_slice_0_mult_out_roa_1_5), .B4(Dummy_slice_0_mult_out_roa_1_4), 
           .B3(Dummy_slice_0_mult_out_roa_1_3), .B2(Dummy_slice_0_mult_out_roa_1_2), 
           .B1(Dummy_slice_0_mult_out_roa_1_1), .B0(Dummy_slice_0_mult_out_roa_1_0), 
           .C53(C[53]), .C52(C[52]), .C51(C[51]), .C50(C[50]), .C49(C[49]), 
           .C48(C[48]), .C47(C[47]), .C46(C[46]), .C45(C[45]), .C44(C[44]), 
           .C43(C[43]), .C42(C[42]), .C41(C[41]), .C40(C[40]), .C39(C[39]), 
           .C38(C[38]), .C37(C[37]), .C36(C[36]), .C35(C[35]), .C34(C[34]), 
           .C33(C[33]), .C32(C[32]), .C31(C[31]), .C30(C[30]), .C29(C[29]), 
           .C28(C[28]), .C27(C[27]), .C26(C[26]), .C25(C[25]), .C24(C[24]), 
           .C23(C[23]), .C22(C[22]), .C21(C[21]), .C20(C[20]), .C19(C[19]), 
           .C18(C[18]), .C17(C[17]), .C16(C[16]), .C15(C[15]), .C14(C[14]), 
           .C13(C[13]), .C12(C[12]), .C11(C[11]), .C10(C[10]), .C9(C[9]), 
           .C8(C[8]), .C7(C[7]), .C6(C[6]), .C5(C[5]), .C4(C[4]), 
           .C3(C[3]), .C2(C[2]), .C1(C[1]), .C0(C[0]), .CFB53(scuba_vlo), 
           .CFB52(scuba_vlo), .CFB51(scuba_vlo), .CFB50(scuba_vlo), .CFB49(scuba_vlo), 
           .CFB48(scuba_vlo), .CFB47(scuba_vlo), .CFB46(scuba_vlo), .CFB45(scuba_vlo), 
           .CFB44(scuba_vlo), .CFB43(scuba_vlo), .CFB42(scuba_vlo), .CFB41(scuba_vlo), 
           .CFB40(scuba_vlo), .CFB39(scuba_vlo), .CFB38(scuba_vlo), .CFB37(scuba_vlo), 
           .CFB36(scuba_vlo), .CFB35(scuba_vlo), .CFB34(scuba_vlo), .CFB33(scuba_vlo), 
           .CFB32(scuba_vlo), .CFB31(scuba_vlo), .CFB30(scuba_vlo), .CFB29(scuba_vlo), 
           .CFB28(scuba_vlo), .CFB27(scuba_vlo), .CFB26(scuba_vlo), .CFB25(scuba_vlo), 
           .CFB24(scuba_vlo), .CFB23(scuba_vlo), .CFB22(scuba_vlo), .CFB21(scuba_vlo), 
           .CFB20(scuba_vlo), .CFB19(scuba_vlo), .CFB18(scuba_vlo), .CFB17(scuba_vlo), 
           .CFB16(scuba_vlo), .CFB15(scuba_vlo), .CFB14(scuba_vlo), .CFB13(scuba_vlo), 
           .CFB12(scuba_vlo), .CFB11(scuba_vlo), .CFB10(scuba_vlo), .CFB9(scuba_vlo), 
           .CFB8(scuba_vlo), .CFB7(scuba_vlo), .CFB6(scuba_vlo), .CFB5(scuba_vlo), 
           .CFB4(scuba_vlo), .CFB3(scuba_vlo), .CFB2(scuba_vlo), .CFB1(scuba_vlo), 
           .CFB0(scuba_vlo), .MA35(Dummy_slice_0_mult_out_p_0_35), .MA34(Dummy_slice_0_mult_out_p_0_34), 
           .MA33(Dummy_slice_0_mult_out_p_0_33), .MA32(Dummy_slice_0_mult_out_p_0_32), 
           .MA31(Dummy_slice_0_mult_out_p_0_31), .MA30(Dummy_slice_0_mult_out_p_0_30), 
           .MA29(Dummy_slice_0_mult_out_p_0_29), .MA28(Dummy_slice_0_mult_out_p_0_28), 
           .MA27(Dummy_slice_0_mult_out_p_0_27), .MA26(Dummy_slice_0_mult_out_p_0_26), 
           .MA25(Dummy_slice_0_mult_out_p_0_25), .MA24(Dummy_slice_0_mult_out_p_0_24), 
           .MA23(Dummy_slice_0_mult_out_p_0_23), .MA22(Dummy_slice_0_mult_out_p_0_22), 
           .MA21(Dummy_slice_0_mult_out_p_0_21), .MA20(Dummy_slice_0_mult_out_p_0_20), 
           .MA19(Dummy_slice_0_mult_out_p_0_19), .MA18(Dummy_slice_0_mult_out_p_0_18), 
           .MA17(Dummy_slice_0_mult_out_p_0_17), .MA16(Dummy_slice_0_mult_out_p_0_16), 
           .MA15(Dummy_slice_0_mult_out_p_0_15), .MA14(Dummy_slice_0_mult_out_p_0_14), 
           .MA13(Dummy_slice_0_mult_out_p_0_13), .MA12(Dummy_slice_0_mult_out_p_0_12), 
           .MA11(Dummy_slice_0_mult_out_p_0_11), .MA10(Dummy_slice_0_mult_out_p_0_10), 
           .MA9(Dummy_slice_0_mult_out_p_0_9), .MA8(Dummy_slice_0_mult_out_p_0_8), 
           .MA7(Dummy_slice_0_mult_out_p_0_7), .MA6(Dummy_slice_0_mult_out_p_0_6), 
           .MA5(Dummy_slice_0_mult_out_p_0_5), .MA4(Dummy_slice_0_mult_out_p_0_4), 
           .MA3(Dummy_slice_0_mult_out_p_0_3), .MA2(Dummy_slice_0_mult_out_p_0_2), 
           .MA1(Dummy_slice_0_mult_out_p_0_1), .MA0(Dummy_slice_0_mult_out_p_0_0), 
           .MB35(Dummy_slice_0_mult_out_p_1_35), .MB34(Dummy_slice_0_mult_out_p_1_34), 
           .MB33(Dummy_slice_0_mult_out_p_1_33), .MB32(Dummy_slice_0_mult_out_p_1_32), 
           .MB31(Dummy_slice_0_mult_out_p_1_31), .MB30(Dummy_slice_0_mult_out_p_1_30), 
           .MB29(Dummy_slice_0_mult_out_p_1_29), .MB28(Dummy_slice_0_mult_out_p_1_28), 
           .MB27(Dummy_slice_0_mult_out_p_1_27), .MB26(Dummy_slice_0_mult_out_p_1_26), 
           .MB25(Dummy_slice_0_mult_out_p_1_25), .MB24(Dummy_slice_0_mult_out_p_1_24), 
           .MB23(Dummy_slice_0_mult_out_p_1_23), .MB22(Dummy_slice_0_mult_out_p_1_22), 
           .MB21(Dummy_slice_0_mult_out_p_1_21), .MB20(Dummy_slice_0_mult_out_p_1_20), 
           .MB19(Dummy_slice_0_mult_out_p_1_19), .MB18(Dummy_slice_0_mult_out_p_1_18), 
           .MB17(Dummy_slice_0_mult_out_p_1_17), .MB16(Dummy_slice_0_mult_out_p_1_16), 
           .MB15(Dummy_slice_0_mult_out_p_1_15), .MB14(Dummy_slice_0_mult_out_p_1_14), 
           .MB13(Dummy_slice_0_mult_out_p_1_13), .MB12(Dummy_slice_0_mult_out_p_1_12), 
           .MB11(Dummy_slice_0_mult_out_p_1_11), .MB10(Dummy_slice_0_mult_out_p_1_10), 
           .MB9(Dummy_slice_0_mult_out_p_1_9), .MB8(Dummy_slice_0_mult_out_p_1_8), 
           .MB7(Dummy_slice_0_mult_out_p_1_7), .MB6(Dummy_slice_0_mult_out_p_1_6), 
           .MB5(Dummy_slice_0_mult_out_p_1_5), .MB4(Dummy_slice_0_mult_out_p_1_4), 
           .MB3(Dummy_slice_0_mult_out_p_1_3), .MB2(Dummy_slice_0_mult_out_p_1_2), 
           .MB1(Dummy_slice_0_mult_out_p_1_1), .MB0(Dummy_slice_0_mult_out_p_1_0), 
           .CIN53(Cin[53]), .CIN52(Cin[52]), .CIN51(Cin[51]), .CIN50(Cin[50]), 
           .CIN49(Cin[49]), .CIN48(Cin[48]), .CIN47(Cin[47]), .CIN46(Cin[46]), 
           .CIN45(Cin[45]), .CIN44(Cin[44]), .CIN43(Cin[43]), .CIN42(Cin[42]), 
           .CIN41(Cin[41]), .CIN40(Cin[40]), .CIN39(Cin[39]), .CIN38(Cin[38]), 
           .CIN37(Cin[37]), .CIN36(Cin[36]), .CIN35(Cin[35]), .CIN34(Cin[34]), 
           .CIN33(Cin[33]), .CIN32(Cin[32]), .CIN31(Cin[31]), .CIN30(Cin[30]), 
           .CIN29(Cin[29]), .CIN28(Cin[28]), .CIN27(Cin[27]), .CIN26(Cin[26]), 
           .CIN25(Cin[25]), .CIN24(Cin[24]), .CIN23(Cin[23]), .CIN22(Cin[22]), 
           .CIN21(Cin[21]), .CIN20(Cin[20]), .CIN19(Cin[19]), .CIN18(Cin[18]), 
           .CIN17(Cin[17]), .CIN16(Cin[16]), .CIN15(Cin[15]), .CIN14(Cin[14]), 
           .CIN13(Cin[13]), .CIN12(Cin[12]), .CIN11(Cin[11]), .CIN10(Cin[10]), 
           .CIN9(Cin[9]), .CIN8(Cin[8]), .CIN7(Cin[7]), .CIN6(Cin[6]), 
           .CIN5(Cin[5]), .CIN4(Cin[4]), .CIN3(Cin[3]), .CIN2(Cin[2]), 
           .CIN1(Cin[1]), .CIN0(Cin[0]), .OP10(Opcode[3]), .OP9(Opcode[2]), 
           .OP8(Opcode[1]), .OP7(Opcode[0]), .OP6(CMuxsel[2]), .OP5(CMuxsel[1]), 
           .OP4(CMuxsel[0]), .OP3(BMuxsel[1]), .OP2(BMuxsel[0]), .OP1(AMuxsel[1]), 
           .OP0(AMuxsel[0]), .R53(Result[53]), .R52(Result[52]), .R51(Result[51]), 
           .R50(Result[50]), .R49(Result[49]), .R48(Result[48]), .R47(Result[47]), 
           .R46(Result[46]), .R45(Result[45]), .R44(Result[44]), .R43(Result[43]), 
           .R42(Result[42]), .R41(Result[41]), .R40(Result[40]), .R39(Result[39]), 
           .R38(Result[38]), .R37(Result[37]), .R36(Result[36]), .R35(Result[35]), 
           .R34(Result[34]), .R33(Result[33]), .R32(Result[32]), .R31(Result[31]), 
           .R30(Result[30]), .R29(Result[29]), .R28(Result[28]), .R27(Result[27]), 
           .R26(Result[26]), .R25(Result[25]), .R24(Result[24]), .R23(Result[23]), 
           .R22(Result[22]), .R21(Result[21]), .R20(Result[20]), .R19(Result[19]), 
           .R18(Result[18]), .R17(Result[17]), .R16(Result[16]), .R15(Result[15]), 
           .R14(Result[14]), .R13(Result[13]), .R12(Result[12]), .R11(Result[11]), 
           .R10(Result[10]), .R9(Result[9]), .R8(Result[8]), .R7(Result[7]), 
           .R6(Result[6]), .R5(Result[5]), .R4(Result[4]), .R3(Result[3]), 
           .R2(Result[2]), .R1(Result[1]), .R0(Result[0]), .EQZ(EQZ), 
           .EQZM(EQZM), .EQOM(EQOM), .EQPAT(EQPAT), .EQPATB(EQPATB), 
           .OVER(OVER), .UNDER(UNDER), .SIGNEDR(SignR)) /* synthesis syn_instantiated=1 */ ;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(287[12] 437[106])
    defparam dsp_alu_0.REG_INPUTC0_CLK = "CLK1";
    defparam dsp_alu_0.REG_INPUTC0_CE = "CE0";
    defparam dsp_alu_0.REG_INPUTC0_RST = "RST0";
    defparam dsp_alu_0.REG_INPUTC1_CLK = "CLK1";
    defparam dsp_alu_0.REG_INPUTC1_CE = "CE0";
    defparam dsp_alu_0.REG_INPUTC1_RST = "RST0";
    defparam dsp_alu_0.REG_OPCODEOP0_0_CLK = "CLK0";
    defparam dsp_alu_0.REG_OPCODEOP0_0_CE = "CE0";
    defparam dsp_alu_0.REG_OPCODEOP0_0_RST = "RST0";
    defparam dsp_alu_0.REG_OPCODEOP1_0_CLK = "CLK0";
    defparam dsp_alu_0.REG_OPCODEOP0_1_CLK = "CLK0";
    defparam dsp_alu_0.REG_OPCODEOP0_1_CE = "CE0";
    defparam dsp_alu_0.REG_OPCODEOP0_1_RST = "RST0";
    defparam dsp_alu_0.REG_OPCODEOP1_1_CLK = "CLK0";
    defparam dsp_alu_0.REG_OPCODEIN_0_CLK = "CLK0";
    defparam dsp_alu_0.REG_OPCODEIN_0_CE = "CE0";
    defparam dsp_alu_0.REG_OPCODEIN_0_RST = "RST0";
    defparam dsp_alu_0.REG_OPCODEIN_1_CLK = "CLK0";
    defparam dsp_alu_0.REG_OPCODEIN_1_CE = "CE0";
    defparam dsp_alu_0.REG_OPCODEIN_1_RST = "RST0";
    defparam dsp_alu_0.REG_OUTPUT0_CLK = "CLK0";
    defparam dsp_alu_0.REG_OUTPUT0_CE = "CE0";
    defparam dsp_alu_0.REG_OUTPUT0_RST = "RST0";
    defparam dsp_alu_0.REG_OUTPUT1_CLK = "CLK0";
    defparam dsp_alu_0.REG_OUTPUT1_CE = "CE0";
    defparam dsp_alu_0.REG_OUTPUT1_RST = "RST0";
    defparam dsp_alu_0.REG_FLAG_CLK = "CLK0";
    defparam dsp_alu_0.REG_FLAG_CE = "CE0";
    defparam dsp_alu_0.REG_FLAG_RST = "RST0";
    defparam dsp_alu_0.MCPAT_SOURCE = "STATIC";
    defparam dsp_alu_0.MASKPAT_SOURCE = "STATIC";
    defparam dsp_alu_0.MASK01 = "0x00000000000000";
    defparam dsp_alu_0.REG_INPUTCFB_CLK = "NONE";
    defparam dsp_alu_0.REG_INPUTCFB_CE = "CE0";
    defparam dsp_alu_0.REG_INPUTCFB_RST = "RST0";
    defparam dsp_alu_0.CLK0_DIV = "ENABLED";
    defparam dsp_alu_0.CLK1_DIV = "DISABLED";
    defparam dsp_alu_0.CLK2_DIV = "ENABLED";
    defparam dsp_alu_0.CLK3_DIV = "ENABLED";
    defparam dsp_alu_0.MCPAT = "0x00000000000000";
    defparam dsp_alu_0.MASKPAT = "0x00000000000000";
    defparam dsp_alu_0.RNDPAT = "0x00000000000000";
    defparam dsp_alu_0.GSR = "ENABLED";
    defparam dsp_alu_0.RESETMODE = "SYNC";
    defparam dsp_alu_0.MULT9_MODE = "DISABLED";
    defparam dsp_alu_0.LEGACY = "DISABLED";
    MULT18X18D dsp_mult_1 (.A17(AA[17]), .A16(AA[16]), .A15(AA[15]), .A14(AA[14]), 
            .A13(AA[13]), .A12(AA[12]), .A11(AA[11]), .A10(AA[10]), 
            .A9(AA[9]), .A8(AA[8]), .A7(AA[7]), .A6(AA[6]), .A5(AA[5]), 
            .A4(AA[4]), .A3(AA[3]), .A2(AA[2]), .A1(AA[1]), .A0(AA[0]), 
            .B17(AB[17]), .B16(AB[16]), .B15(AB[15]), .B14(AB[14]), 
            .B13(AB[13]), .B12(AB[12]), .B11(AB[11]), .B10(AB[10]), 
            .B9(AB[9]), .B8(AB[8]), .B7(AB[7]), .B6(AB[6]), .B5(AB[5]), 
            .B4(AB[4]), .B3(AB[3]), .B2(AB[2]), .B1(AB[1]), .B0(AB[0]), 
            .C17(scuba_vlo), .C16(scuba_vlo), .C15(scuba_vlo), .C14(scuba_vlo), 
            .C13(scuba_vlo), .C12(scuba_vlo), .C11(scuba_vlo), .C10(scuba_vlo), 
            .C9(scuba_vlo), .C8(scuba_vlo), .C7(scuba_vlo), .C6(scuba_vlo), 
            .C5(scuba_vlo), .C4(scuba_vlo), .C3(scuba_vlo), .C2(scuba_vlo), 
            .C1(scuba_vlo), .C0(scuba_vlo), .SIGNEDA(VCC_net), .SIGNEDB(VCC_net), 
            .SOURCEA(scuba_vlo), .SOURCEB(scuba_vlo), .CLK3(scuba_vlo), 
            .CLK2(scuba_vlo), .CLK1(scuba_vlo), .CLK0(CLK0), .CE3(VCC_net), 
            .CE2(VCC_net), .CE1(VCC_net), .CE0(CE0), .RST3(scuba_vlo), 
            .RST2(scuba_vlo), .RST1(scuba_vlo), .RST0(RST0), .SRIA17(scuba_vlo), 
            .SRIA16(scuba_vlo), .SRIA15(scuba_vlo), .SRIA14(scuba_vlo), 
            .SRIA13(scuba_vlo), .SRIA12(scuba_vlo), .SRIA11(scuba_vlo), 
            .SRIA10(scuba_vlo), .SRIA9(scuba_vlo), .SRIA8(scuba_vlo), 
            .SRIA7(scuba_vlo), .SRIA6(scuba_vlo), .SRIA5(scuba_vlo), .SRIA4(scuba_vlo), 
            .SRIA3(scuba_vlo), .SRIA2(scuba_vlo), .SRIA1(scuba_vlo), .SRIA0(scuba_vlo), 
            .SRIB17(scuba_vlo), .SRIB16(scuba_vlo), .SRIB15(scuba_vlo), 
            .SRIB14(scuba_vlo), .SRIB13(scuba_vlo), .SRIB12(scuba_vlo), 
            .SRIB11(scuba_vlo), .SRIB10(scuba_vlo), .SRIB9(scuba_vlo), 
            .SRIB8(scuba_vlo), .SRIB7(scuba_vlo), .SRIB6(scuba_vlo), .SRIB5(scuba_vlo), 
            .SRIB4(scuba_vlo), .SRIB3(scuba_vlo), .SRIB2(scuba_vlo), .SRIB1(scuba_vlo), 
            .SRIB0(scuba_vlo), .ROA17(Dummy_slice_0_mult_out_roa_0_17), 
            .ROA16(Dummy_slice_0_mult_out_roa_0_16), .ROA15(Dummy_slice_0_mult_out_roa_0_15), 
            .ROA14(Dummy_slice_0_mult_out_roa_0_14), .ROA13(Dummy_slice_0_mult_out_roa_0_13), 
            .ROA12(Dummy_slice_0_mult_out_roa_0_12), .ROA11(Dummy_slice_0_mult_out_roa_0_11), 
            .ROA10(Dummy_slice_0_mult_out_roa_0_10), .ROA9(Dummy_slice_0_mult_out_roa_0_9), 
            .ROA8(Dummy_slice_0_mult_out_roa_0_8), .ROA7(Dummy_slice_0_mult_out_roa_0_7), 
            .ROA6(Dummy_slice_0_mult_out_roa_0_6), .ROA5(Dummy_slice_0_mult_out_roa_0_5), 
            .ROA4(Dummy_slice_0_mult_out_roa_0_4), .ROA3(Dummy_slice_0_mult_out_roa_0_3), 
            .ROA2(Dummy_slice_0_mult_out_roa_0_2), .ROA1(Dummy_slice_0_mult_out_roa_0_1), 
            .ROA0(Dummy_slice_0_mult_out_roa_0_0), .ROB17(Dummy_slice_0_mult_out_rob_0_17), 
            .ROB16(Dummy_slice_0_mult_out_rob_0_16), .ROB15(Dummy_slice_0_mult_out_rob_0_15), 
            .ROB14(Dummy_slice_0_mult_out_rob_0_14), .ROB13(Dummy_slice_0_mult_out_rob_0_13), 
            .ROB12(Dummy_slice_0_mult_out_rob_0_12), .ROB11(Dummy_slice_0_mult_out_rob_0_11), 
            .ROB10(Dummy_slice_0_mult_out_rob_0_10), .ROB9(Dummy_slice_0_mult_out_rob_0_9), 
            .ROB8(Dummy_slice_0_mult_out_rob_0_8), .ROB7(Dummy_slice_0_mult_out_rob_0_7), 
            .ROB6(Dummy_slice_0_mult_out_rob_0_6), .ROB5(Dummy_slice_0_mult_out_rob_0_5), 
            .ROB4(Dummy_slice_0_mult_out_rob_0_4), .ROB3(Dummy_slice_0_mult_out_rob_0_3), 
            .ROB2(Dummy_slice_0_mult_out_rob_0_2), .ROB1(Dummy_slice_0_mult_out_rob_0_1), 
            .ROB0(Dummy_slice_0_mult_out_rob_0_0), .P35(Dummy_slice_0_mult_out_p_0_35), 
            .P34(Dummy_slice_0_mult_out_p_0_34), .P33(Dummy_slice_0_mult_out_p_0_33), 
            .P32(Dummy_slice_0_mult_out_p_0_32), .P31(Dummy_slice_0_mult_out_p_0_31), 
            .P30(Dummy_slice_0_mult_out_p_0_30), .P29(Dummy_slice_0_mult_out_p_0_29), 
            .P28(Dummy_slice_0_mult_out_p_0_28), .P27(Dummy_slice_0_mult_out_p_0_27), 
            .P26(Dummy_slice_0_mult_out_p_0_26), .P25(Dummy_slice_0_mult_out_p_0_25), 
            .P24(Dummy_slice_0_mult_out_p_0_24), .P23(Dummy_slice_0_mult_out_p_0_23), 
            .P22(Dummy_slice_0_mult_out_p_0_22), .P21(Dummy_slice_0_mult_out_p_0_21), 
            .P20(Dummy_slice_0_mult_out_p_0_20), .P19(Dummy_slice_0_mult_out_p_0_19), 
            .P18(Dummy_slice_0_mult_out_p_0_18), .P17(Dummy_slice_0_mult_out_p_0_17), 
            .P16(Dummy_slice_0_mult_out_p_0_16), .P15(Dummy_slice_0_mult_out_p_0_15), 
            .P14(Dummy_slice_0_mult_out_p_0_14), .P13(Dummy_slice_0_mult_out_p_0_13), 
            .P12(Dummy_slice_0_mult_out_p_0_12), .P11(Dummy_slice_0_mult_out_p_0_11), 
            .P10(Dummy_slice_0_mult_out_p_0_10), .P9(Dummy_slice_0_mult_out_p_0_9), 
            .P8(Dummy_slice_0_mult_out_p_0_8), .P7(Dummy_slice_0_mult_out_p_0_7), 
            .P6(Dummy_slice_0_mult_out_p_0_6), .P5(Dummy_slice_0_mult_out_p_0_5), 
            .P4(Dummy_slice_0_mult_out_p_0_4), .P3(Dummy_slice_0_mult_out_p_0_3), 
            .P2(Dummy_slice_0_mult_out_p_0_2), .P1(Dummy_slice_0_mult_out_p_0_1), 
            .P0(Dummy_slice_0_mult_out_p_0_0), .SIGNEDP(Dummy_slice_0_mult_out_signedp_0)) /* synthesis syn_instantiated=1 */ ;   // c:/users/mrtea/workspace_v10/skjee/fpga/clarity/dummy_slice/dummy_slice.v(464[16] 532[52])
    defparam dsp_mult_1.REG_INPUTA_CLK = "CLK0";
    defparam dsp_mult_1.REG_INPUTA_CE = "CE0";
    defparam dsp_mult_1.REG_INPUTA_RST = "RST0";
    defparam dsp_mult_1.REG_INPUTB_CLK = "CLK0";
    defparam dsp_mult_1.REG_INPUTB_CE = "CE0";
    defparam dsp_mult_1.REG_INPUTB_RST = "RST0";
    defparam dsp_mult_1.REG_INPUTC_CLK = "NONE";
    defparam dsp_mult_1.REG_INPUTC_CE = "CE0";
    defparam dsp_mult_1.REG_INPUTC_RST = "RST0";
    defparam dsp_mult_1.REG_PIPELINE_CLK = "CLK0";
    defparam dsp_mult_1.REG_PIPELINE_CE = "CE0";
    defparam dsp_mult_1.REG_PIPELINE_RST = "RST0";
    defparam dsp_mult_1.REG_OUTPUT_CLK = "NONE";
    defparam dsp_mult_1.REG_OUTPUT_CE = "CE0";
    defparam dsp_mult_1.REG_OUTPUT_RST = "RST0";
    defparam dsp_mult_1.CLK0_DIV = "ENABLED";
    defparam dsp_mult_1.CLK1_DIV = "DISABLED";
    defparam dsp_mult_1.CLK2_DIV = "ENABLED";
    defparam dsp_mult_1.CLK3_DIV = "ENABLED";
    defparam dsp_mult_1.HIGHSPEED_CLK = "NONE";
    defparam dsp_mult_1.GSR = "ENABLED";
    defparam dsp_mult_1.CAS_MATCH_REG = "FALSE";
    defparam dsp_mult_1.SOURCEB_MODE = "B_SHIFT";
    defparam dsp_mult_1.MULT_BYPASS = "DISABLED";
    defparam dsp_mult_1.RESETMODE = "SYNC";
    GSR GSR_INST (.GSR(VCC_net));
    PUR PUR_INST (.PUR(VCC_net));
    defparam PUR_INST.RST_PULSE = 1;
    VHI i301 (.Z(VCC_net));
    
endmodule
//
// Verilog Description of module PUR
// module not written out since it is a black-box. 
//

