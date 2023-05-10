/* synthesis translate_off*/
`define SBP_SIMULATION
/* synthesis translate_on*/
`ifndef SBP_SIMULATION
`define SBP_SYNTHESIS
`endif

//
// Verific Verilog Description of module Clarity
//
module Clarity (IDDR_datain, IDDR_q, PLL_SerDes_PHASESEL, IDDR_alignwd, 
            IDDR_clkin, IDDR_ready, IDDR_sclk, IDDR_start, IDDR_sync_clk, 
            IDDR_sync_reset, PLL_DSP_CLKI, PLL_DSP_CLKOP, PLL_SerDes_CLKI, 
            PLL_SerDes_CLKOP, PLL_SerDes_CLKOS, PLL_SerDes_CLKOS2, PLL_SerDes_CLKOS3, 
            PLL_SerDes_LOCK, PLL_SerDes_PHASEDIR, PLL_SerDes_PHASELOADREG, 
            PLL_SerDes_PHASESTEP) /* synthesis sbp_module=true */ ;
    input [0:0]IDDR_datain;
    output [3:0]IDDR_q;
    input [1:0]PLL_SerDes_PHASESEL;
    input IDDR_alignwd;
    input IDDR_clkin;
    output IDDR_ready;
    output IDDR_sclk;
    input IDDR_start;
    input IDDR_sync_clk;
    input IDDR_sync_reset;
    input PLL_DSP_CLKI;
    output PLL_DSP_CLKOP;
    input PLL_SerDes_CLKI;
    output PLL_SerDes_CLKOP;
    output PLL_SerDes_CLKOS;
    output PLL_SerDes_CLKOS2;
    output PLL_SerDes_CLKOS3;
    output PLL_SerDes_LOCK;
    input PLL_SerDes_PHASEDIR;
    input PLL_SerDes_PHASELOADREG;
    input PLL_SerDes_PHASESTEP;
    
    
    IDDR IDDR_inst (.datain({IDDR_datain}), .q({IDDR_q}), .alignwd(IDDR_alignwd), 
         .clkin(IDDR_clkin), .ready(IDDR_ready), .sclk(IDDR_sclk), .start(IDDR_start), 
         .sync_clk(IDDR_sync_clk), .sync_reset(IDDR_sync_reset));
    PLL_DSP PLL_DSP_inst (.CLKI(PLL_DSP_CLKI), .CLKOP(PLL_DSP_CLKOP));
    PLL_SerDes PLL_SerDes_inst (.PHASESEL({PLL_SerDes_PHASESEL}), .CLKI(PLL_SerDes_CLKI), 
            .CLKOP(PLL_SerDes_CLKOP), .CLKOS(PLL_SerDes_CLKOS), .CLKOS2(PLL_SerDes_CLKOS2), 
            .CLKOS3(PLL_SerDes_CLKOS3), .LOCK(PLL_SerDes_LOCK), .PHASEDIR(PLL_SerDes_PHASEDIR), 
            .PHASELOADREG(PLL_SerDes_PHASELOADREG), .PHASESTEP(PLL_SerDes_PHASESTEP));
    
endmodule

