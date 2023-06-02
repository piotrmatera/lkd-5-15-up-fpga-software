`include "global.v" 
 
module Symmetrical_PWM_full(clk_i, enable_output_i, override_i, duty_i, next_period_i, current_period_i, local_counter_i, sync_phase_i, PWM_o);
	parameter[15:0] DEADTIME = 65;
	parameter NEGATE_OUTPUT = 0;
	localparam DEADTIME_WIDTH = $clog2(DEADTIME+2);
	input clk_i;
	input enable_output_i;
	input[1:0] override_i;
	input[15:0] duty_i;
	input[15:0] next_period_i;
	input[15:0] current_period_i;
	input[15:0] local_counter_i;
	input sync_phase_i;
	output[1:0] PWM_o;

	reg[1:0] PWM_r[1:0];
	reg[1:0] PWM_r2[1:0];

	reg[15:0] duty_r[1:0];
	reg[15:0] local_counter_r;
	
	reg[DEADTIME_WIDTH-1:0] deadtime1_counter_r;
	reg[DEADTIME_WIDTH-1:0] deadtime2_counter_r;
	
	reg sync_phase_r;

	always @(posedge clk_i) begin 
		sync_phase_r <= sync_phase_i; 
		 
		if(sync_phase_i ^ sync_phase_r) begin 
			duty_r[0] = duty_i - (DEADTIME>>1); 
			duty_r[1] = duty_i + (DEADTIME>>1); 
		end 
 
		PWM_r[0][0] <= $signed(duty_r[0]) >= $signed({local_counter_r[15:1], ~sync_phase_r}); 
		PWM_r[0][1] <= $signed(duty_r[0]) >= $signed({local_counter_r[15:1], sync_phase_r}); 
		PWM_r[1][0] <= $signed(duty_r[1]) >= $signed({local_counter_r[15:1], ~sync_phase_r}); 
		PWM_r[1][1] <= $signed(duty_r[1]) >= $signed({local_counter_r[15:1], sync_phase_r}); 
		 
		if(sync_phase_i) 
			local_counter_r <= {local_counter_i[14:0], 1'b1} - current_period_i;
		else
			local_counter_r <= current_period_i - {local_counter_i[14:0], 1'b1} ; 
		 
		if(PWM_r[0] == 2'b11 || PWM_r[0] == 2'b10) 
			deadtime1_counter_r <= {DEADTIME_WIDTH{1'b0}}; 
		else if(deadtime1_counter_r < DEADTIME) 
			deadtime1_counter_r <= deadtime1_counter_r + {{DEADTIME_WIDTH-1{1'b0}}, ~PWM_r[0][0]} + {{DEADTIME_WIDTH-1{1'b0}}, ~PWM_r[0][1]}; 
			 
		if(PWM_r[1] == 2'b00 || PWM_r[1] == 2'b01) 
			deadtime2_counter_r <= {DEADTIME_WIDTH{1'b0}}; 
		else if(deadtime2_counter_r < DEADTIME) 
			deadtime2_counter_r <= deadtime2_counter_r + {{DEADTIME_WIDTH-1{1'b0}}, PWM_r[1][0]} + {{DEADTIME_WIDTH-1{1'b0}}, PWM_r[1][1]}; 
			 
		PWM_r2[0][0] <= deadtime2_counter_r >= DEADTIME && PWM_r[0][0];
		PWM_r2[0][1] <= deadtime2_counter_r + PWM_r[0][0] >= DEADTIME && PWM_r[0][1];
		PWM_r2[1][0] <= deadtime1_counter_r >= DEADTIME && !PWM_r[1][0];
		PWM_r2[1][1] <= deadtime1_counter_r + !PWM_r[1][0] >= DEADTIME && !PWM_r[1][1];  
	end 
	
	if(NEGATE_OUTPUT) begin	
		ODDRX1F PWM_ODDR1(.D0(!PWM_r2[0][0] || !enable_output_i), .D1(!PWM_r2[0][1] || !enable_output_i), .SCLK(clk_i), .RST(override_i[0]), .Q(PWM_o[0]));
		ODDRX1F PWM_ODDR2(.D0(!PWM_r2[1][0] || !enable_output_i), .D1(!PWM_r2[1][1] || !enable_output_i), .SCLK(clk_i), .RST(override_i[1]), .Q(PWM_o[1]));
 	end else begin
		ODDRX1F PWM_ODDR1(.D0(PWM_r2[0][0] || override_i[0]), .D1(PWM_r2[0][1] || override_i[0]), .SCLK(clk_i), .RST(!enable_output_i && !override_i[0]), .Q(PWM_o[0]));
		ODDRX1F PWM_ODDR2(.D0(PWM_r2[1][0] || override_i[1]), .D1(PWM_r2[1][1] || override_i[1]), .SCLK(clk_i), .RST(!enable_output_i && !override_i[1]), .Q(PWM_o[1]));		
	end

	initial begin
		deadtime1_counter_r = 0;
		deadtime2_counter_r = 0;
		duty_r[0] = 0;
		duty_r[1] = 0;
		local_counter_r = 0;
		sync_phase_r = 0;
		PWM_r[0] = 0;
		PWM_r[1] = 0;
		PWM_r2[0] = 0;
		PWM_r2[1] = 0;
	end
endmodule 

module Symmetrical_PWM(clk_i, enable_output_i, override_i, duty_i, next_period_i, current_period_i, local_counter_i, sync_phase_i, PWM_o);
	parameter[15:0] DEADTIME = 50;
	parameter NEGATE_OUTPUT = 0;
	localparam DEADTIME_WIDTH = $clog2(DEADTIME+2);
	input clk_i;
	input enable_output_i;
	input[1:0] override_i;
	input[15:0] duty_i;
	input[15:0] next_period_i;
	input[15:0] current_period_i;
	input[15:0] local_counter_i;
	input sync_phase_i;
	output[1:0] PWM_o;

	reg[1:0] PWM_r;
	reg[1:0] PWM_r2[1:0];

	reg[15:0] duty_r;
	reg[15:0] local_counter_r;
	
	reg[DEADTIME_WIDTH-1:0] deadtime1_counter_r;
	reg[DEADTIME_WIDTH-1:0] deadtime2_counter_r;
	
	reg sync_phase_r;

	always @(posedge clk_i) begin
		sync_phase_r <= sync_phase_i;
		
		if(sync_phase_i ^ sync_phase_r)
			duty_r = duty_i;

		PWM_r[0] <= $signed(duty_r) >= $signed({local_counter_r[15:1], ~sync_phase_r});
		PWM_r[1] <= $signed(duty_r) >= $signed({local_counter_r[15:1], sync_phase_r});
		
		if(sync_phase_i)
			local_counter_r <= {local_counter_i[14:0], 1'b1} - current_period_i;
		else
			local_counter_r <= current_period_i - {local_counter_i[14:0], 1'b1} ;
		
		if(PWM_r == 2'b11 || PWM_r == 2'b10)
			deadtime1_counter_r <= {DEADTIME_WIDTH{1'b0}};
		else if(deadtime1_counter_r < DEADTIME)
			deadtime1_counter_r <= deadtime1_counter_r + {{DEADTIME_WIDTH-1{1'b0}}, ~PWM_r[0]} + {{DEADTIME_WIDTH-1{1'b0}}, ~PWM_r[1]};
			
		if(PWM_r == 2'b00 || PWM_r == 2'b01)
			deadtime2_counter_r <= {DEADTIME_WIDTH{1'b0}};
		else if(deadtime2_counter_r < DEADTIME)
			deadtime2_counter_r <= deadtime2_counter_r + {{DEADTIME_WIDTH-1{1'b0}}, PWM_r[0]} + {{DEADTIME_WIDTH-1{1'b0}}, PWM_r[1]};
			
		PWM_r2[0][0] <= deadtime2_counter_r >= DEADTIME && PWM_r[0];
		PWM_r2[0][1] <= deadtime2_counter_r + PWM_r[0] >= DEADTIME && PWM_r[1];
		PWM_r2[1][0] <= deadtime1_counter_r >= DEADTIME && !PWM_r[0];
		PWM_r2[1][1] <= deadtime1_counter_r + !PWM_r[0] >= DEADTIME && !PWM_r[1];	
	end
	
	if(NEGATE_OUTPUT) begin	
		ODDRX1F PWM_ODDR1(.D0(!PWM_r2[0][0] || !enable_output_i), .D1(!PWM_r2[0][1] || !enable_output_i), .SCLK(clk_i), .RST(override_i[0]), .Q(PWM_o[0]));
		ODDRX1F PWM_ODDR2(.D0(!PWM_r2[1][0] || !enable_output_i), .D1(!PWM_r2[1][1] || !enable_output_i), .SCLK(clk_i), .RST(override_i[1]), .Q(PWM_o[1]));
 	end else begin
		ODDRX1F PWM_ODDR1(.D0(PWM_r2[0][0] || override_i[0]), .D1(PWM_r2[0][1] || override_i[0]), .SCLK(clk_i), .RST(!enable_output_i && !override_i[0]), .Q(PWM_o[0]));
		ODDRX1F PWM_ODDR2(.D0(PWM_r2[1][0] || override_i[1]), .D1(PWM_r2[1][1] || override_i[1]), .SCLK(clk_i), .RST(!enable_output_i && !override_i[1]), .Q(PWM_o[1]));		
	end

	initial begin
		deadtime1_counter_r = 0;
		deadtime2_counter_r = 0;
		duty_r = 0;
		local_counter_r = 0;
		sync_phase_r = 0;
		PWM_r = 0;
		PWM_r2[0] = 0;
		PWM_r2[1] = 0;
	end
endmodule 
 
module double_pulse(clk_i, start_i, length0_i, length1_i, PWM_o); 
	input clk_i; 
	input start_i; 
	input[15:0] length0_i; 
	input[15:0] length1_i; 
	output reg PWM_o; 
	 
	reg[15:0] counter_r; 
	reg[1:0] state_reg; 
 
	reg[2:0] start_r; 
	reg[15:0] length0_r; 
	reg[15:0] length1_r; 
 
	always @(posedge clk_i) begin 
		start_r <= {start_i, start_r[2:1]}; 
		case (state_reg) 
			0 : begin 
				PWM_o <= 1'b0; 
				counter_r <= 16'b0; 
				if(start_r[1:0] == 2'b10) begin 
					state_reg <= 1; 
					length0_r <= length0_i; 
					length1_r <= length1_i; 
				end 
			end 
			1 : begin 
				PWM_o <= 1'b1; 
				counter_r <= counter_r + 1'b1; 
				if(counter_r == length0_r) begin 
					counter_r <= 16'b0; 
					state_reg <= 2; 
				end 
			end 
			2 : begin 
				PWM_o <= 1'b0; 
				counter_r <= counter_r + 1'b1; 
				if(counter_r == length1_r) begin 
					counter_r <= 16'b0; 
					state_reg <= 3; 
				end 
			end 
			3 : begin 
				PWM_o <= 1'b1; 
				counter_r <= counter_r + 1'b1; 
				if(counter_r == length1_r) begin 
					counter_r <= 16'b0; 
					state_reg <= 0; 
				end 
			end 
		endcase 
	end 
	 
	initial begin 
		length0_r = 0; 
		length1_r = 0; 
		start_r = 0; 
		counter_r = 0; 
		state_reg = 0; 
		PWM_o = 0; 
	end 
	 
endmodule 
 
module deadtime(clk_i, PWM_i, override_i, enable_i, PWM_o);
	parameter DEADTIME = 5; 
	parameter [0:0] NEGATE_OUTPUT = 0;
	parameter [0:0] SWAP = 1;
	localparam DT_WIDTH = $clog2(DEADTIME+1);
	input clk_i;
	input PWM_i; 
	input [1:0] override_i;
	input enable_i;
	output [1:0] PWM_o;
	
	reg[DT_WIDTH-1:0] PWM_RED;
	reg[DT_WIDTH-1:0] PWM_FED; 
	reg PWM_r; 
	reg enable_r;
	 
	wire [1:0] PWM_w; 
	
	always @(posedge clk_i) begin 
		PWM_r <= PWM_i; 
		enable_r <= enable_i;
		if(PWM_r) begin 
			if(PWM_RED != DEADTIME) 
				PWM_RED <= PWM_RED + 1'b1; 
		end 
		else 
			PWM_RED <= {DT_WIDTH{1'b0}}; 
			
		if(!PWM_r) begin
			if(PWM_FED != DEADTIME)
				PWM_FED <= PWM_FED + 1'b1;
		end
		else
			PWM_FED <= {DT_WIDTH{1'b0}};
	end 
	
	if(SWAP) begin
		assign PWM_w[1] = (PWM_r && PWM_RED == DEADTIME) ^ NEGATE_OUTPUT;
		assign PWM_w[0] = (!(PWM_r || PWM_FED != DEADTIME)) ^ NEGATE_OUTPUT; 
	end
	else begin
		assign PWM_w[0] = (PWM_r && PWM_RED == DEADTIME) ^ NEGATE_OUTPUT;
		assign PWM_w[1] = (!(PWM_r || PWM_FED != DEADTIME)) ^ NEGATE_OUTPUT; 
	end
	if(NEGATE_OUTPUT) begin 
		OFS1P3BX OSDR[1:0](.D(PWM_w & ~override_i), .SP(1'b1), .SCLK(clk_i), .PD(~{2{enable_r}} & ~override_i), .Q(PWM_o)); 
	end 
	else begin 
		OFS1P3DX OSDR[1:0](.D(PWM_w | override_i), .SP(1'b1), .SCLK(clk_i), .CD(~{2{enable_r}} & ~override_i), .Q(PWM_o)); 
	end 
	
	initial begin
		enable_r = 0; 
		PWM_r = 0;
		PWM_RED = 0; 
		PWM_FED = 0;
	end
endmodule