`include "global.v"

module SerDes_tb;
	reg XTAL1_25MHz_i, XTAL2_25MHz_i, XTAL3_25MHz_i;
	reg XTAL1_20MHz_i;
	reg TZ_EN;

	GSR GSR_INST (.GSR (1'b1));
	PUR PUR_INST (.PUR (1'b0));

	reg[15:0] i;

	always begin
		TZ_EN = 0;
		i = 0;
		#100
		TZ_EN = 1;
		forever begin
			#20
			i <= i+1;
			if(i>=4000) $stop;
		end
	end

	//wire[15:0] next_period;
	//assign next_period = `CYCLE_PERIOD-1;
	//wire[15:0] current_period;
	//wire[15:0] local_counter;
	//wire sync_phase;
	//Local_clock	#(.INITIAL_PHASE(0)) Local_clock(.clk_i(XTAL1_25MHz_i), .next_period_i(next_period), .current_period_o(current_period), .local_counter_o(local_counter), .sync_phase_o(sync_phase),
	//.snapshot_start_i(4'b0), .snapshot_value_o()); 
	
	
	//reg[15:0] duty_r; 
	//reg[1:0] sync_reg; 
	//always @(posedge XTAL1_25MHz_i) begin
		//sync_reg <= {sync_phase, sync_reg[1]};
		//if(sync_phase)
			//duty_r <= 16'd2024;
		//else
			//duty_r <= -16'd2023;
			
		//duty_r <= 16'd0;
	//end
	
	//wire PWM_EN;
	//assign PWM_EN = 1'b1;
	//wire PWM_EN_r; 
	//FD1P3DX PWM_EN_ff(.D(PWM_EN), .SP(sync_reg[1] ^ sync_reg[0]), .CK(XTAL1_25MHz_i), .CD(!(TZ_EN & PWM_EN)), .Q(PWM_EN_r)); 

	//Symmetrical_PWM Symmetrical_PWM(.clk_i(XTAL1_25MHz_i), .enable_output_i(PWM_EN_r), .duty_i(duty_r), .next_period_i(next_period), .current_period_i(current_period), .local_counter_i(local_counter), .sync_phase_i(sync_phase), .PWM_o(PWM_o));
	
	localparam EMIF_MEMORY_WIDTH = `COMM_MEMORY_EMIF_WIDTH+3;//$clog2(2x COMM_MEMORY + MUX) = $clog2(3) = 2 
	wire EMIF_oe_i; 
	wire EMIF_we_i; 
	wire EMIF_cs_i; 
	wire[31:0] EMIF_data_i;
	wire[EMIF_MEMORY_WIDTH-1:0] EMIF_address_i; 
 
	assign clk_DSP = XTAL1_25MHz_i;
	
	wire Kalman1_WIP;
	wire Kalman1_START;
	
	wire Kalman1_Mem2_we;
	wire[8:0] Kalman1_Mem2_addrw;
	wire[35:0] Kalman1_Mem2_data;
	wire[31:0] Kalman1_data_o;
	wire [53:0] Kalman1_CIN;
	wire [53:0] Kalman1_CO;
	wire Kalman1_SIGNEDCIN;
	wire Kalman1_SIGNEDCO;

	parameter Kalman1_Mem2_key = 3'd6;
	Kalman #(.DEBUG(1)) Kalman1(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Kalman1_Mem2_key[1 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == Kalman1_Mem2_key[0]),
	.enable_i(Kalman1_START), .Mem2_addrw_o(Kalman1_Mem2_addrw), .Mem2_we_o(Kalman1_Mem2_we), .Mem2_data_o(Kalman1_Mem2_data), .WIP_flag_o(Kalman1_WIP),
	.CIN(Kalman1_CIN), .SIGNEDCIN(Kalman1_SIGNEDCIN), .CO(Kalman1_CO), .SIGNEDCO(Kalman1_SIGNEDCO));

	assign Kalman1_START = i[9];
	assign Kalman1_SIGNEDCIN = 1;
	assign Kalman1_CIN = 0;
	
	wire Res_ctrl1_WIP;
	wire Res_ctrl1_START;
	
	wire Res_ctrl1_Mem2_we;
	wire[8:0] Res_ctrl1_Mem2_addrw;
	wire[35:0] Res_ctrl1_Mem2_data;
	wire[31:0] Res_ctrl1_data_o;
	wire [53:0] Res_ctrl1_CIN;
	wire [53:0] Res_ctrl1_CO;
	wire Res_ctrl1_SIGNEDCIN;
	wire Res_ctrl1_SIGNEDCO;
	
	Res_ctrl Res_ctrl1(.clk_i(clk_DSP), .Mem1_data_i(EMIF_data_i), .Mem1_addrw_i(EMIF_address_i[`COMM_MEMORY_EMIF_WIDTH-1:0]), .Mem1_clk_w(EMIF_we_i),
	.Mem1_clk_en_w(EMIF_address_i[EMIF_MEMORY_WIDTH-2 +: 2] == Kalman1_Mem2_key[1 +: 2]), .Mem1_we_i(EMIF_address_i[EMIF_MEMORY_WIDTH-3] == Kalman1_Mem2_key[0]),
	.enable_i(Res_ctrl1_START), .Mem2_addrw_o(Res_ctrl1_Mem2_addrw), .Mem2_we_o(Res_ctrl1_Mem2_we), .Mem2_data_o(Res_ctrl1_Mem2_data), .WIP_flag_o(Res_ctrl1_WIP),
	.CIN(Res_ctrl1_CIN), .SIGNEDCIN(Res_ctrl1_SIGNEDCIN), .CO(Res_ctrl1_CO), .SIGNEDCO(Res_ctrl1_SIGNEDCO));

	assign Res_ctrl1_START = i[9];
	assign Res_ctrl1_SIGNEDCIN = 1;
	assign Res_ctrl1_CIN = 0;
	
	assign EMIF_oe_i = 1; 
	assign EMIF_we_i = 1; 
	assign EMIF_cs_i = 1; 
	assign EMIF_data_i = 0;
	assign EMIF_address_i = 0; 
	
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

