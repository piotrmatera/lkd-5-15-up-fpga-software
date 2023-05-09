`include "global.v"

module SED_machine(clk_i, enable_i, err_o);
	input clk_i;
	input enable_i;
	output err_o;
	
	reg[7:0] enable_counter;
	wire sed_enable;
	reg sed_start;
	wire sed_err;
	wire sed_done;
	wire sed_inprog;
	wire sed_clkout;
	SEDGA #
	(.SED_CLK_FREQ("2.4"), // 2.4, 4.8, 9.7, 19.4, 38.8, 62.0
	.CHECKALWAYS("DISABLED"),
	.DEV_DENSITY("25KU")) // 12KU, 12KUM, 25KU, 25KUM, 45KU, 45KUM, 85KU, 85KUM
	sed_ip_inst(
	.SEDENABLE(sed_enable),
	.SEDSTART(sed_start),
	.SEDFRCERR(1'b0),
	.SEDERR(sed_err),
	.SEDDONE(sed_done),
	.SEDINPROG(sed_inprog),
	.SEDCLKOUT(sed_clkout)
	);
	
	reg enable_r;
	reg sed_done_r;
	reg[3:0] sed_done_counter;
	always @(posedge clk_i) begin
		sed_done_r <= sed_done;
		enable_r <= enable_i;
		
		if(sed_done_r) sed_done_counter <= sed_done_counter + 1'b1;
		else sed_done_counter <= 0;
			
		if(sed_done_counter == 4'hF || !enable_r) enable_counter <= 0;
		else if(!enable_counter[7]) enable_counter <= enable_counter + 1'b1;
	end
	
	reg [3:0] sed_counter;
	
	always @(posedge sed_clkout) begin
		if(sed_done) begin
			sed_counter <= 0;
			sed_start <= 0;
		end
		else begin
			sed_counter <= sed_counter + 1'b1;
			if(sed_counter == 4'hF) sed_start <= !sed_start && !sed_inprog;
		end
	end
	
	assign sed_enable = enable_counter[7] && enable_i;

	initial sed_counter = 0;
	initial enable_counter = 0;
	initial sed_start = 0;
	initial sed_done_counter = 0;
	
	assign err_o = sed_err;
endmodule
	