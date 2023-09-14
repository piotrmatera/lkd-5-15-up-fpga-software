`include "global.v"

module UART_core_TX(clk_i, baud_clock, data_i, tx_o);
	parameter BYTES = 2;
	localparam WIDTH = BYTES*8;
	input clk_i;
	input baud_clock;
	input[WIDTH-1:0] data_i;
	output tx_o;

	reg[WIDTH-1:0] data_r;
	initial data_r = 0;

	localparam STATES_WIDTH = 3;
	localparam [STATES_WIDTH-1:0]
    s_0 = 0,
    s_1 = 1,
    s_2 = 2,
	s_3 = 3,
    s_4 = 4,
    s_5 = 5,
    s_6 = 6,
	s_7 = 7;
	reg[STATES_WIDTH-1:0] state_reg, state_reg_last;
	initial begin
		state_reg = s_0;
		state_reg_last = s_0;
	end

	reg[7:0] tx_data;
	reg new_tx_data;
	initial tx_data = 0;
	initial new_tx_data = 0;

	uart_tx  uart_tx_1
	(
		.clock(clk_i),
		.ce_16(baud_clock), .tx_data(tx_data), .new_tx_data(new_tx_data),
		.ser_out(tx_o), .tx_busy(tx_busy)
	);

	wire[15:0] crc;
	crc_generator crc_generator_uut(
     .clk(clk_i),
     .rst(state_reg == s_0),
     .data_ena(new_tx_data && state_reg_last == s_1),
     .crc(crc),
     .input_data(tx_data)
	);

	reg[15:0] delay_counter;
	initial delay_counter = 0;
	reg[$clog2(BYTES+1)-1:0] data_counter;
	initial data_counter = 0;

	always @(posedge clk_i) begin
		if(baud_clock)
			delay_counter <= delay_counter + 1'b1;
		state_reg_last <= state_reg;
		new_tx_data <= 1'b0;
		case (state_reg)
			s_0 : begin
				delay_counter <= 0;
				data_counter <= 0;
				data_r <= data_i;
				state_reg <= s_1;
			end
			s_1 : begin
				tx_data <= data_r[data_counter*8 +: 8];
				if(!tx_busy & !new_tx_data) begin
					data_counter <= data_counter + 1'b1;
					new_tx_data <= 1'b1;
				end
				if(data_counter == BYTES)
					state_reg <= s_2;
			end
			s_2 : begin
				tx_data <= crc[7:0];
				if(!tx_busy & !new_tx_data) begin
					new_tx_data <= 1'b1;
					state_reg <= s_3;
				end
			end
			s_3 : begin
				tx_data <= crc[15:8];
				if(!tx_busy & !new_tx_data) begin
					new_tx_data <= 1'b1;
					state_reg <= s_4;
				end
			end
			s_4 : begin
				if(delay_counter == 16'd10000-2'd2)
					state_reg <= s_0;
			end
			s_5 : begin
				state_reg <= s_0;
			end
			s_6 : begin
				state_reg <= s_0;
			end
			s_7 : begin
				state_reg <= s_0;
			end
		endcase
	end

endmodule

module UART_core_RX(clk_i, baud_clock, no_comm_o, data_o, rx_i);
	parameter BYTES = 2;
	parameter DEFAULT_OUT = 32'h43160000;
	localparam WIDTH = BYTES*8;
	input clk_i;
	input baud_clock;
	output reg no_comm_o;
	initial no_comm_o = 0;
	output reg[WIDTH-1:0] data_o;
	initial data_o = 0;
	input rx_i;

	reg[WIDTH-1:0] data_r;
	initial data_r = 0;

	localparam STATES_WIDTH = 3;
	localparam [STATES_WIDTH-1:0]
    s_0 = 0,
    s_1 = 1,
    s_2 = 2,
	s_3 = 3,
    s_4 = 4,
    s_5 = 5,
    s_6 = 6,
	s_7 = 7;
	reg[STATES_WIDTH-1:0] state_reg;
	initial state_reg = s_0;

	wire[7:0] rx_data;
	uart_rx uart_rx_1
	(
		.clock(clk_i),
		.ce_16(baud_clock), .ser_in(rx_i),
		.rx_data(rx_data), .new_rx_data(new_rx_data)
	);

	wire[15:0] crc;
	crc_generator crc_generator_uut(
     .clk(clk_i),
     .rst(state_reg == s_0 & !new_rx_data),
     .data_ena(new_rx_data),
     .crc(crc),
     .input_data(rx_data)
	);

	reg[15:0] timeout_counter;
	initial timeout_counter = 0;
	reg[7:0] delay_counter;
	initial delay_counter = 0;
	reg[$clog2(BYTES+1)-1:0] data_counter;
	initial data_counter = 0;

	always @(posedge clk_i) begin
		if(baud_clock) begin
			delay_counter <= delay_counter + 1'b1;
			if(timeout_counter != 16'd15000)
				timeout_counter <= timeout_counter + 1'b1;
			else begin
				data_o <= DEFAULT_OUT;
				no_comm_o <= 1;
			end
		end

		case (state_reg)
			s_0 : begin
				data_counter <= 0;
				if(new_rx_data) begin
					data_counter <= data_counter + 1'b1;
					delay_counter <= 0;
					data_r[data_counter*8 +: 8] <= rx_data;
					state_reg <= s_1;
				end
			end
			s_1 : begin
				if(new_rx_data) begin
					data_counter <= data_counter + 1'b1;
					delay_counter <= 0;
					data_r[data_counter*8 +: 8] <= rx_data;
				end
				if(delay_counter == 8'hFF)
					state_reg <= s_0;
				if(data_counter == BYTES)
					state_reg <= s_2;
			end
			s_2 : begin
				if(new_rx_data) begin
					delay_counter <= 0;
					state_reg <= s_3;
				end
				if(delay_counter == 8'hFF)
					state_reg <= s_0;
			end
			s_3 : begin
				if(new_rx_data) begin
					delay_counter <= 0;
					state_reg <= s_4;
				end
				if(delay_counter == 8'hFF)
					state_reg <= s_0;
			end
			s_4 : begin
				if(crc == 0) begin
					timeout_counter <= 0;
					data_o <= data_r;
					no_comm_o <= 0;
				end
				state_reg <= s_0;
			end
			s_5 : begin
				state_reg <= s_6;
			end
			s_6 : begin
				state_reg <= s_7;
			end
			s_7 : begin
				state_reg <= s_0;
			end
		endcase
	end
endmodule