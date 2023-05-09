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
			if(i>=20000) $stop;
		end
	end

	wire[180:1] Master_FIO;
	wire[180:1] Slave1_FIO;
	wire[180:1] Slave2_FIO;
	wire[180:1] Slave3_FIO;
	wire[168:0] Master_CIO;
	wire[168:0] Slave1_CIO;
	wire[168:0] Slave2_CIO;
	wire[168:0] Slave3_CIO;

	assign Master_CIO[`SYNC_SD_CM] = Master_CIO[`SYNC_PWM_CM];

	wire[1:0] Slave1_tx;
	wire[1:0] Slave1_rx;
	wire[1:0] Slave2_tx;
	wire[1:0] Slave2_rx;
	wire[1:0] Slave3_tx;
	wire[1:0] Slave3_rx;
	wire[1:0] Master_tx;
	wire[1:0] Master_rx;

	assign Slave3_tx = {Slave3_FIO[`TX2_FS], Slave3_FIO[`TX1_FS]};
	assign {Slave3_FIO[`RX2_FS], Slave3_FIO[`RX1_FS]} = Slave3_rx;
	assign Slave2_tx = {Slave2_FIO[`TX2_FS], Slave2_FIO[`TX1_FS]};
	assign {Slave2_FIO[`RX2_FS], Slave2_FIO[`RX1_FS]} = Slave2_rx;
	assign Slave1_tx = {Slave1_FIO[`TX2_FS], Slave1_FIO[`TX1_FS]};
	assign {Slave1_FIO[`RX2_FS], Slave1_FIO[`RX1_FS]} = Slave1_rx;
	assign Master_tx = {Master_FIO[`TX2_FM], Master_FIO[`TX1_FM]};
	assign {Master_FIO[`RX2_FM], Master_FIO[`RX1_FM]} = Master_rx;

	assign Master_rx[0] = ~Slave1_tx[0];
	assign Master_rx[1] = ~1'b0;
	
	assign Slave1_rx[0] = ~Master_tx[0];
	assign Slave1_rx[1] = ~Slave2_tx[0];
	
	assign Slave2_rx[0] = ~Slave1_tx[1];
	assign Slave2_rx[1] = ~Slave3_tx[0];
	
	assign Slave3_rx[0] = ~Slave2_tx[1];
	assign Slave3_rx[1] = ~1'b0;
		
	assign Master_FIO[`DATA_U1_FM] = 1'b1;
/*
	SerDes_master SerDes_master(.XTAL_20MHz_i(XTAL1_20MHz_i), .XTAL_25MHz_i(XTAL1_25MHz_i), .CPU_io(Master_CIO), .FPGA_conn_io(Master_FIO),
	.rst_cpu_o(), .clk_cpu_o());

	SerDes_slave SerDes_slave1(.XTAL_20MHz_i(XTAL1_20MHz_i), .XTAL_25MHz_i(XTAL2_25MHz_i), .CPU_io(Slave1_CIO), .FPGA_conn_io(Slave1_FIO),
	.rst_cpu_o(), .clk_cpu_o());

	SerDes_slave SerDes_slave2(.XTAL_20MHz_i(XTAL1_20MHz_i), .XTAL_25MHz_i(XTAL3_25MHz_i), .CPU_io(Slave2_CIO), .FPGA_conn_io(Slave2_FIO),
	.rst_cpu_o(), .clk_cpu_o());

	SerDes_slave SerDes_slave3(.XTAL_20MHz_i(XTAL1_20MHz_i), .XTAL_25MHz_i(XTAL1_25MHz_i), .CPU_io(Slave3_CIO), .FPGA_conn_io(Slave3_FIO),
	.rst_cpu_o(), .clk_cpu_o());
*/
	wire[15:0] next_period;
	assign next_period = `CYCLE_PERIOD-1;
	wire[15:0] current_period;
	wire[15:0] local_counter;
	wire sync_phase;
	Local_clock	#(.INITIAL_PHASE(0)) Local_clock(.clk_i(XTAL1_25MHz_i), .next_period_i(next_period), .current_period_o(current_period), .local_counter_o(local_counter), .sync_phase_o(sync_phase),
	.snapshot_start_i(4'b0), .snapshot_value_o()); 
	
	
	reg[15:0] duty_r; 
	reg[1:0] sync_reg; 
	always @(posedge XTAL1_25MHz_i) begin
		sync_reg <= {sync_phase, sync_reg[1]};
		if(sync_phase)
			duty_r <= 16'd2024;
		else
			duty_r <= -16'd2023;
			
		duty_r <= 16'd0;
	end
	
	wire PWM_EN;
	assign PWM_EN = 1'b1;
	wire PWM_EN_r; 
	FD1P3DX PWM_EN_ff(.D(PWM_EN), .SP(sync_reg[1] ^ sync_reg[0]), .CK(XTAL1_25MHz_i), .CD(!(TZ_EN & PWM_EN)), .Q(PWM_EN_r)); 

	Symmetrical_PWM Symmetrical_PWM(.clk_i(XTAL1_25MHz_i), .enable_output_i(PWM_EN_r), .duty_i(duty_r), .next_period_i(next_period), .current_period_i(current_period), .local_counter_i(local_counter), .sync_phase_i(sync_phase), .PWM_o(PWM_o));

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

