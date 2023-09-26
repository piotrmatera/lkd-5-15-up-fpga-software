`include "global.v" 

module TX_core(core_clk_i, hipri_msg_en_i, lopri_msg_en_i,
fifo_clk_o, fifo_we_o, fifo_o,
mem_addr_o, mem_data_i,
tx_hipri_msg_start_i, tx_hipri_msg_wip_o, tx_lopri_msg_start_i, tx_lopri_msg_wip_o,
tx_code_start_i, tx_code_i); 
	input core_clk_i; 
 	input hipri_msg_en_i; 
 	input lopri_msg_en_i; 
 
	output fifo_clk_o; 
	output reg [1:0] fifo_we_o; 
	output reg [8:0] fifo_o;
 
	output reg[`POINTER_WIDTH-1:0] mem_addr_o; 
	input[7:0] mem_data_i; 
 
	input[`HIPRI_MAILBOXES_NUMBER-1:0] tx_hipri_msg_start_i; 
	output[`HIPRI_MAILBOXES_NUMBER-1:0] tx_hipri_msg_wip_o; 
 
	input[`LOPRI_MAILBOXES_NUMBER-1:0] tx_lopri_msg_start_i; 
	output[`LOPRI_MAILBOXES_NUMBER-1:0] tx_lopri_msg_wip_o; 

	input tx_code_start_i; 
	input [8:0] tx_code_i; 
	 			
	assign fifo_clk_o = core_clk_i;
	
///////////////////////////////////////////////////////////////////// 
	
	localparam STATES_WIDTH = 4; 
	localparam [STATES_WIDTH-1:0] 
    S_TX_idle = 0, 
    S_TX_hipri_start = 1, 
    S_TX_hipri_header = 2, 
	S_TX_hipri_length = 3, 
    S_TX_hipri_data = 4, 
    S_TX_hipri_crc1 = 5, 
    S_TX_hipri_crc2 = 6, 
	S_TX_hipri_end = 7,
    S_TX_lopri_start = 8, 
    S_TX_lopri_header = 9, 
	S_TX_lopri_length = 10, 
    S_TX_lopri_data = 11, 
    S_TX_lopri_crc1 = 12, 
    S_TX_lopri_crc2 = 13, 
	S_TX_lopri_end = 14,
	S_TX_code = 15; 
	reg[STATES_WIDTH-1:0] state_reg; 
	
 	reg[1:0] hipri_msg_en_r; 
 	reg[1:0] lopri_msg_en_r; 
	
	reg [`HIPRI_MAILBOXES_WIDTH-1:0] current_hipri_msg;
	wire[31:0] reset_hipri_flag; 
	assign reset_hipri_flag = (state_reg == S_TX_hipri_end ? (1'b1 << current_hipri_msg) : 0) | {32{!hipri_msg_en_r[0]}}; 
	Sync_latch_input #(.OUT_POLARITY(1), .STEPS(2)) sync_msg_start[`HIPRI_MAILBOXES_NUMBER-1:0](.clk_i(core_clk_i), .in(tx_hipri_msg_start_i), .out(tx_hipri_msg_wip_o), .reset_i(reset_hipri_flag[`HIPRI_MAILBOXES_NUMBER-1:0]), .set_i(1'b0)); 

	reg [`LOPRI_MAILBOXES_WIDTH-1:0] current_lopri_msg;
	wire[31:0] reset_lopri_flag; 
	assign reset_lopri_flag = (state_reg == S_TX_lopri_end ? (1'b1 << current_lopri_msg) : 0) | {32{!lopri_msg_en_r[0]}}; 	 
	Sync_latch_input #(.OUT_POLARITY(1), .STEPS(2)) async_msg_start[`LOPRI_MAILBOXES_NUMBER-1:0](.clk_i(core_clk_i), .in(tx_lopri_msg_start_i), .out(tx_lopri_msg_wip_o), .reset_i(reset_lopri_flag[`LOPRI_MAILBOXES_NUMBER-1:0]), .set_i(1'b0));

	wire tx_code_wip; 
	Sync_latch_input #(.OUT_POLARITY(1), .STEPS(2)) code_start(.clk_i(core_clk_i), .in(tx_code_start_i), .out(tx_code_wip), .reset_i(state_reg == S_TX_code), .set_i(1'b0));
	
/////////////////////////////////////////////////////////////////////
	
	wire[15:0] crc_hipri;
	crc_generator sync_msg_crc_generator(
     .clk(core_clk_i),
     .rst(state_reg == S_TX_hipri_start),
     .data_ena(state_reg == S_TX_hipri_header | state_reg == S_TX_hipri_length | state_reg == S_TX_hipri_data),
     .crc(crc_hipri),
     .input_data(mem_data_i)
	); 
	
	wire[15:0] crc_lopri;
	crc_generator async_msg_crc_generator(
     .clk(core_clk_i),
     .rst(state_reg == S_TX_lopri_start),
     .data_ena(state_reg == S_TX_lopri_header | state_reg == S_TX_lopri_length | state_reg == S_TX_lopri_data),
     .crc(crc_lopri),
     .input_data(mem_data_i)
	); 

	reg[8:0] sync_data_counter;
	reg[8:0] sync_message_length;
	reg[8:0] async_data_counter;
	reg[8:0] async_message_length;

	reg[STATES_WIDTH-1:0] state_reg_lopri_last;
	reg[`POINTER_WIDTH-1:0] mem_addr_o_lopri_last;
	reg [8:0] tx_code_r; 
	
	integer i;	 
	 
	always @(posedge core_clk_i) begin 
		hipri_msg_en_r <= {hipri_msg_en_i, hipri_msg_en_r[1]}; 
		lopri_msg_en_r <= {lopri_msg_en_i, lopri_msg_en_r[1]}; 
		tx_code_r <= tx_code_i;
		
		mem_addr_o <= mem_addr_o + 1'b1;
		fifo_o <= {1'b0, mem_data_i}; 
		fifo_we_o <= 2'b0;
		
		case (state_reg) 
			S_TX_idle : begin				
				if(state_reg_lopri_last > S_TX_lopri_start && state_reg_lopri_last <= S_TX_lopri_end)
					state_reg <= state_reg_lopri_last;
				else begin
					for (i = `LOPRI_MAILBOXES_NUMBER-1; i >= 0; i = i - 1)
						if (tx_lopri_msg_wip_o[i]) begin
							async_data_counter <= 0;
							current_lopri_msg <= i[`LOPRI_MAILBOXES_WIDTH-1:0];
							mem_addr_o <= i[`LOPRI_MAILBOXES_WIDTH-1:0]*`LOPRI_MSG_LENGTH; 
							state_reg <= S_TX_lopri_start;
						end
				end

				for (i = `HIPRI_MAILBOXES_NUMBER-1; i >= 0; i = i - 1)
					if (tx_hipri_msg_wip_o[i]) begin
						sync_data_counter <= 0;
						current_hipri_msg <= i[`HIPRI_MAILBOXES_WIDTH-1:0];
						mem_addr_o <= i[`HIPRI_MAILBOXES_WIDTH-1:0]*`HIPRI_MSG_LENGTH + `LOPRI_MAILBOXES_NUMBER*`LOPRI_MSG_LENGTH; 
						state_reg <= S_TX_hipri_start;
					end 
					
				if (tx_code_wip)
					state_reg <= S_TX_code;
			end 
			S_TX_hipri_start : begin
				fifo_o <= `K_Start_Hipri_Packet; 
				state_reg <= S_TX_hipri_header;
			end 
			S_TX_hipri_header : begin
				state_reg <= S_TX_hipri_length; 
			end 
			S_TX_hipri_length : begin
				sync_message_length <= {mem_data_i, 1'b0}; 
				if(mem_data_i == 0) 
					state_reg <= S_TX_hipri_end; 
				else if(mem_data_i == 1) 
					state_reg <= S_TX_hipri_crc1; 
				else 
					state_reg <= S_TX_hipri_data; 
			end 
			S_TX_hipri_data : begin
				if(sync_data_counter == sync_message_length) 
					state_reg <= S_TX_hipri_crc1; 
			end 
			S_TX_hipri_crc1 : begin
				fifo_o <= {1'b0, crc_hipri[7:0]}; 
				state_reg <= S_TX_hipri_crc2; 
			end 
			S_TX_hipri_crc2 : begin
				fifo_o <= {1'b0, crc_hipri[15:8]}; 
				state_reg <= S_TX_hipri_end; 
			end 
			S_TX_hipri_end : begin
				mem_addr_o <= mem_addr_o_lopri_last; 
				fifo_o <= `K_End_Hipri_Packet; 
				state_reg <= S_TX_idle; 
			end 

			S_TX_lopri_start : begin
				fifo_o <= `K_Start_Lopri_Packet; 
				state_reg <= S_TX_lopri_header;
				state_reg_lopri_last <= S_TX_lopri_header;
			end 
			S_TX_lopri_header : begin
				state_reg <= S_TX_lopri_length; 
				state_reg_lopri_last <= S_TX_lopri_length;
			end 
			S_TX_lopri_length : begin
				async_message_length <= {mem_data_i, 1'b0}; 
				if(mem_data_i == 0) begin
					state_reg <= S_TX_lopri_end;
					state_reg_lopri_last <= S_TX_lopri_end;
				end
				else if(mem_data_i == 1) begin
					state_reg <= S_TX_lopri_crc1; 
					state_reg_lopri_last <= S_TX_lopri_crc1; 
				end
				else begin
					state_reg <= S_TX_lopri_data; 
					state_reg_lopri_last <= S_TX_lopri_data; 
				end
			end 
			S_TX_lopri_data : begin
				if(async_data_counter == async_message_length) begin
					state_reg <= S_TX_lopri_crc1; 
					state_reg_lopri_last <= S_TX_lopri_crc1; 
				end
			end 
			S_TX_lopri_crc1 : begin
				fifo_o <= {1'b0, crc_lopri[7:0]}; 
				state_reg <= S_TX_lopri_crc2; 
				state_reg_lopri_last <= S_TX_lopri_crc2;
			end 
			S_TX_lopri_crc2 : begin
				fifo_o <= {1'b0, crc_lopri[15:8]}; 
				state_reg <= S_TX_lopri_end; 
				state_reg_lopri_last <= S_TX_lopri_end; 

			end 
			S_TX_lopri_end : begin
				fifo_o <= `K_End_Lopri_Packet; 
				state_reg <= S_TX_idle; 
				state_reg_lopri_last <= S_TX_idle; 
			end 
			
			S_TX_code : begin 
				mem_addr_o <= mem_addr_o_lopri_last; 
				fifo_we_o[0] <= 1'b1; 
				fifo_o <= tx_code_r;
				state_reg <= S_TX_idle; 
			end 
		endcase 
		
		if(state_reg >= S_TX_hipri_start && state_reg <= S_TX_hipri_end) begin
			fifo_we_o[0] <= 1'b1;
			sync_data_counter <= sync_data_counter + 1'b1;
		end
		
		if(state_reg >= S_TX_lopri_start && state_reg <= S_TX_lopri_end) begin
			fifo_we_o[1] <= 1'b1;
			mem_addr_o_lopri_last <= mem_addr_o;
			async_data_counter <= async_data_counter + 1'b1;
			if(tx_code_wip || (|tx_hipri_msg_wip_o))
				state_reg <= S_TX_idle;
		end
	end 
	
	initial begin
		tx_code_r = 0; 
		hipri_msg_en_r = 0; 
		lopri_msg_en_r = 0; 
		mem_addr_o = 0;
		current_hipri_msg = 0; 
		current_lopri_msg = 0; 
		sync_data_counter = 0; 
		async_data_counter = 0; 
		sync_message_length = 0; 
		async_message_length = 0;
		fifo_we_o = 0; 
		fifo_o = 0; 
		state_reg = 0; 
		state_reg_lopri_last = 0; 
		mem_addr_o_lopri_last = 0; 
	end 
endmodule 
 
module RX_core(core_clk_i, hipri_msg_en_i, lopri_msg_en_i,
fifo_clk_i, fifo_we_i, fifo_i,
mem_addr_o, mem_we_o, mem_data_o,
rx_hipri_msg_ack_i, rx_hipri_msg_wip_o, rx_hipri_msg_rdy_o,
rx_lopri_msg_ack_i, rx_lopri_msg_wip_o, rx_lopri_msg_rdy_o,
rx_crc_error_o, rx_overrun_error_o, rx_frame_error_o,
fifo_rx_o, fifo_rx_dv, state_reg); 
	input core_clk_i;
 	input hipri_msg_en_i; 
 	input lopri_msg_en_i; 

	input fifo_clk_i; 
	input[1:0] fifo_we_i; 
	input[8:0] fifo_i; 
 
	output reg mem_we_o; 
	output reg[`POINTER_WIDTH-1:0] mem_addr_o; 
	output reg[7:0] mem_data_o; 
 
	input[`HIPRI_MAILBOXES_NUMBER-1:0] rx_hipri_msg_ack_i; 
	output reg[`HIPRI_MAILBOXES_NUMBER-1:0] rx_hipri_msg_wip_o; 
	output[`HIPRI_MAILBOXES_NUMBER-1:0] rx_hipri_msg_rdy_o;
	
	input[`LOPRI_MAILBOXES_NUMBER-1:0] rx_lopri_msg_ack_i; 
	output reg[`LOPRI_MAILBOXES_NUMBER-1:0] rx_lopri_msg_wip_o; 
	output[`LOPRI_MAILBOXES_NUMBER-1:0] rx_lopri_msg_rdy_o;

	output reg rx_crc_error_o; 
	output reg rx_overrun_error_o; 
	output reg rx_frame_error_o;
	 
///////////////////////////////////////////////////////////////////// 
	
	output[8:0] fifo_rx_o; 
	wire fifo_rx_re; 
	output reg fifo_rx_dv; 
	wire fifo_rx_empty; 
 
	pmi_fifo_dc 
	#(.pmi_data_width_w(9),
	.pmi_data_width_r(9),
	.pmi_data_depth_w(512),
	.pmi_data_depth_r(512),
	.pmi_full_flag(512),
	.pmi_empty_flag(0),
	.pmi_almost_full_flag(508),
	.pmi_almost_empty_flag(4),
	.pmi_regmode("noreg"),		//"reg", "noreg"
	.pmi_resetmode("sync"),		//"async", "sync"
	.pmi_family("ECP5U"),
	.pmi_implementation("EBR")		//"EBR", "LUT"
	) FIFO_RX_CORE(.Data(fifo_i), .WrClock(fifo_clk_i), .RdClock(core_clk_i), .WrEn(|fifo_we_i), .RdEn(fifo_rx_re), 
	.Reset(1'b0), .RPReset(1'b0), .Q(fifo_rx_o), .Empty(fifo_rx_empty), .Full(), .AlmostEmpty(), .AlmostFull());

 	localparam STATES_WIDTH = 3; 
	localparam [STATES_WIDTH-1:0] 
    S_RX_idle = 0, 
    S_RX_hipri_mailbox = 1, 
    S_RX_hipri_length = 2, 
	S_RX_hipri_data_crc = 3,
    S_RX_lopri_mailbox = 4, 
    S_RX_lopri_length = 5, 
    S_RX_lopri_data_crc = 6, 
    S_RX_7 = 7; 
	output reg[STATES_WIDTH-1:0] state_reg; 

	assign K_Start_Hipri_Packet = fifo_rx_o == `K_Start_Hipri_Packet;
	assign K_End_Hipri_Packet = fifo_rx_o == `K_End_Hipri_Packet;
	assign K_Start_Lopri_Packet = fifo_rx_o == `K_Start_Lopri_Packet;
	assign K_End_Lopri_Packet = fifo_rx_o == `K_End_Lopri_Packet;
	assign K_code = fifo_rx_o[8];
	assign fifo_rx_re = !fifo_rx_empty;
	 
///////////////////////////////////////////////////////////////////// 

	wire[15:0] crc_hipri; 
	crc_generator sync_msg_crc_generator(
     .clk(core_clk_i),
     .rst(K_Start_Hipri_Packet),
     .data_ena(fifo_rx_dv),
     .crc(crc_hipri),
     .input_data(fifo_rx_o[7:0])
	); 

	wire[15:0] crc_lopri; 
	crc_generator async_msg_crc_generator(
     .clk(core_clk_i),
     .rst(K_Start_Lopri_Packet),
     .data_ena(fifo_rx_dv && !K_code && (state_reg == S_RX_lopri_mailbox || state_reg == S_RX_lopri_length || state_reg == S_RX_lopri_data_crc)),
     .crc(crc_lopri),
     .input_data(fifo_rx_o[7:0])
	); 

 	reg[1:0] hipri_msg_en_r; 
 	reg[1:0] lopri_msg_en_r; 
	
	reg[9:0] sync_data_counter;
	reg[9:0] sync_message_length;

	reg [`HIPRI_MAILBOXES_WIDTH-1:0] current_hipri_msg;
	wire [31:0] reset_hipri_flag; 
	wire [31:0] set_hipri_flag; 
	assign set_hipri_flag = (state_reg == S_RX_hipri_data_crc && crc_hipri == 16'b0 && sync_data_counter == sync_message_length && K_End_Hipri_Packet) ? 1'b1 << current_hipri_msg : 0; 
	assign reset_hipri_flag = ((fifo_rx_dv && state_reg == S_RX_hipri_mailbox && !K_code) ? 1'b1 << fifo_rx_o[`HIPRI_MAILBOXES_WIDTH-1:0] : 0) | {32{!hipri_msg_en_r[0]}}; 
	Sync_latch_input  #(.OUT_POLARITY(0), .STEPS(2)) sync_msg_start[`HIPRI_MAILBOXES_NUMBER-1:0](.clk_i(core_clk_i), .in(rx_hipri_msg_ack_i), .out(rx_hipri_msg_rdy_o), .reset_i(reset_hipri_flag[`HIPRI_MAILBOXES_NUMBER-1:0]), .set_i(set_hipri_flag[`HIPRI_MAILBOXES_NUMBER-1:0])); 
 	 
	reg[9:0] async_data_counter;
	reg[9:0] async_message_length;

	reg [`LOPRI_MAILBOXES_WIDTH-1:0] current_lopri_msg;
	wire [31:0] reset_lopri_flag; 
	wire [31:0] set_lopri_flag; 
	assign set_lopri_flag = (state_reg == S_RX_lopri_data_crc && crc_lopri == 16'b0 && async_data_counter == async_message_length && K_End_Lopri_Packet) ? 1'b1 << current_lopri_msg : 0; 
	assign reset_lopri_flag = ((fifo_rx_dv && state_reg == S_RX_lopri_mailbox && !K_code) ? 1'b1 << fifo_rx_o[`LOPRI_MAILBOXES_WIDTH-1:0] : 0) | {32{!lopri_msg_en_r[0]}}; 
	Sync_latch_input  #(.OUT_POLARITY(0), .STEPS(2)) async_msg_start[`LOPRI_MAILBOXES_NUMBER-1:0](.clk_i(core_clk_i), .in(rx_lopri_msg_ack_i), .out(rx_lopri_msg_rdy_o), .reset_i(reset_lopri_flag[`LOPRI_MAILBOXES_NUMBER-1:0]), .set_i(set_lopri_flag[`LOPRI_MAILBOXES_NUMBER-1:0])); 
 
/////////////////////////////////////////////////////////////////////
  
	reg[3:0] empty_counter;
	reg[STATES_WIDTH-1:0] state_reg_lopri_last;
	reg[`POINTER_WIDTH-1:0] mem_addr_o_lopri_last;
	reg[31:0] dummy;
	
	always @(posedge core_clk_i) begin
		hipri_msg_en_r <= {hipri_msg_en_i, hipri_msg_en_r[1]}; 
		lopri_msg_en_r <= {lopri_msg_en_i, lopri_msg_en_r[1]}; 
	
		fifo_rx_dv <= fifo_rx_re; 
		mem_we_o <= 1'b0;
		mem_data_o <= fifo_rx_o[7:0];
		if(mem_we_o)
			mem_addr_o <= mem_addr_o + 1'b1;
		
		rx_hipri_msg_wip_o <= (state_reg == S_RX_hipri_length || state_reg == S_RX_hipri_data_crc) << current_hipri_msg; 
		rx_lopri_msg_wip_o <= (state_reg_lopri_last == S_RX_lopri_length || state_reg_lopri_last == S_RX_lopri_data_crc) << current_lopri_msg; 

		if(fifo_rx_empty) 
			empty_counter <= empty_counter + 1'b1; 
		else 
			empty_counter <= 0; 
 	
		rx_crc_error_o <= 0; 
		rx_overrun_error_o <= 0; 
		rx_frame_error_o <= 0;
	
		if(fifo_rx_dv) begin
			case (state_reg) 
				S_RX_idle : begin 
					if(K_Start_Lopri_Packet) begin 
						async_data_counter <= 0; 
						state_reg <= S_RX_lopri_mailbox; 
						state_reg_lopri_last <= S_RX_lopri_mailbox; 
					end 
					if(K_Start_Hipri_Packet) begin 
						sync_data_counter <= 0; 
						state_reg <= S_RX_hipri_mailbox; 
					end 
				end 
				S_RX_hipri_mailbox : begin 
					if(K_code) begin 
						rx_frame_error_o <= 1'b1; 
						state_reg <= S_RX_idle; 
						state_reg_lopri_last <= S_RX_idle; 
					end 
					else begin 
						if((1'b1 << fifo_rx_o[`HIPRI_MAILBOXES_WIDTH-1:0]) & rx_hipri_msg_rdy_o) 
							rx_overrun_error_o <= 1'b1; 
						mem_we_o <= 1'b1; 
						{dummy[32-`POINTER_WIDTH-1:0], mem_addr_o} <= fifo_rx_o[`HIPRI_MAILBOXES_WIDTH-1:0]*`HIPRI_MSG_LENGTH + `LOPRI_MAILBOXES_NUMBER*`LOPRI_MSG_LENGTH;
						current_hipri_msg <= fifo_rx_o[`HIPRI_MAILBOXES_WIDTH-1:0]; 
						state_reg <= S_RX_hipri_length; 
					end			
				end 
				S_RX_hipri_length : begin 
					if(K_code || fifo_rx_o[7:0] >= `HIPRI_MSG_LENGTH>>1) begin 
						rx_frame_error_o <= 1'b1; 
						state_reg <= S_RX_idle; 
						state_reg_lopri_last <= S_RX_idle; 
					end 
					else begin 
						mem_we_o <= 1'b1; 
						sync_message_length <= {{1'b0, fifo_rx_o[7:0]} + 1'b1, 1'b0}; 
						state_reg <= S_RX_hipri_data_crc; 
					end 
				end 
				S_RX_hipri_data_crc : begin 				 
					if(K_code) begin 
						if(K_End_Hipri_Packet) begin
							state_reg <= state_reg_lopri_last; 
							mem_addr_o <= mem_addr_o_lopri_last;
							if(crc_hipri != 16'b0) begin
								rx_crc_error_o <= 1'b1; 
								state_reg <= S_RX_idle;
								state_reg_lopri_last <= S_RX_idle; 
							end
							if(sync_data_counter != sync_message_length) begin
								rx_frame_error_o <= 1'b1; 
								state_reg <= S_RX_idle;
								state_reg_lopri_last <= S_RX_idle; 
							end
						end else begin
							rx_frame_error_o <= 1'b1; 
							state_reg <= S_RX_idle;
							state_reg_lopri_last <= S_RX_idle; 
						end
					end 
					else
						if(sync_data_counter < sync_message_length)
							mem_we_o <= 1'b1;
						else begin
							rx_frame_error_o <= 1'b1;
							state_reg <= S_RX_idle;
							state_reg_lopri_last <= S_RX_idle; 
						end
				end 

				S_RX_lopri_mailbox : begin 
					if(K_End_Lopri_Packet) begin
						rx_frame_error_o <= 1'b1; 
						state_reg <= S_RX_idle; 
						state_reg_lopri_last <= S_RX_idle; 
					end
					else if(!K_code) begin 
						if((1'b1 << fifo_rx_o[`LOPRI_MAILBOXES_WIDTH-1:0]) & rx_lopri_msg_rdy_o) 
							rx_overrun_error_o <= 1'b1; 
						mem_we_o <= 1'b1; 
						mem_addr_o <= fifo_rx_o[`LOPRI_MAILBOXES_WIDTH-1:0]*`LOPRI_MSG_LENGTH;
						current_lopri_msg <= fifo_rx_o[`LOPRI_MAILBOXES_WIDTH-1:0]; 
						state_reg <= S_RX_lopri_length; 
						state_reg_lopri_last <= S_RX_lopri_length; 
					end 
				end 
				S_RX_lopri_length : begin 
					if(K_End_Lopri_Packet || (!K_code && fifo_rx_o[7:0] >= `LOPRI_MSG_LENGTH>>1)) begin
						rx_frame_error_o <= 1'b1; 
						state_reg <= S_RX_idle; 
						state_reg_lopri_last <= S_RX_idle; 
					end
					else if(!K_code) begin 
						mem_we_o <= 1'b1; 
						async_message_length <= {{1'b0, fifo_rx_o[7:0]} + 1'b1, 1'b0}; 
						state_reg <= S_RX_lopri_data_crc; 
						state_reg_lopri_last <= S_RX_lopri_data_crc; 
					end 
				end 
				S_RX_lopri_data_crc : begin
					if(K_End_Lopri_Packet) begin
						if(crc_lopri != 16'b0) 
							rx_crc_error_o <= 1'b1; 
						if(async_data_counter != async_message_length)
							rx_frame_error_o <= 1'b1; 
						state_reg <= S_RX_idle; 
						state_reg_lopri_last <= S_RX_idle; 
					end
					else if(!K_code)
						if(async_data_counter < async_message_length)
							mem_we_o <= 1'b1;
						else begin
							rx_frame_error_o <= 1'b1;
							state_reg <= S_RX_idle;
							state_reg_lopri_last <= S_RX_idle;
						end
				end 
				default : state_reg <= S_RX_idle; 
			endcase 
		end
		
		if(state_reg == S_RX_hipri_mailbox || state_reg == S_RX_hipri_length || state_reg == S_RX_hipri_data_crc) begin
			if(fifo_rx_dv && !K_code)
				sync_data_counter <= sync_data_counter + 1'b1;

			if(empty_counter[2]) begin 
				state_reg <= S_RX_idle; 
				rx_frame_error_o <= 1'b1; 
			end 
		end

		if(state_reg == S_RX_lopri_mailbox || state_reg == S_RX_lopri_length || state_reg == S_RX_lopri_data_crc) begin
			mem_addr_o_lopri_last <= mem_addr_o + mem_we_o;
			if(fifo_rx_dv && !K_code)
				async_data_counter <= async_data_counter + 1'b1;

			if(empty_counter[3]) begin 
				state_reg <= S_RX_idle; 
				state_reg_lopri_last <= S_RX_idle; 
				rx_frame_error_o <= 1'b1; 
			end 

			if(fifo_rx_dv && K_Start_Hipri_Packet) begin 
				state_reg <= S_RX_hipri_mailbox; 
				sync_data_counter <= 0; 
			end 
		end
	end 

	initial begin 
		hipri_msg_en_r = 0; 
		lopri_msg_en_r = 0; 
		rx_hipri_msg_wip_o = 0; 
		rx_lopri_msg_wip_o = 0;
		rx_crc_error_o = 0; 
		rx_overrun_error_o = 0; 
		rx_frame_error_o = 0; 
		mem_addr_o = 0; 
		mem_data_o = 0; 
		mem_we_o = 0; 
		fifo_rx_dv = 0; 
		current_hipri_msg = 0;
		current_lopri_msg = 0;
		sync_message_length = 0; 
		sync_data_counter = 0; 
		async_message_length = 0; 
		async_data_counter = 0; 
		empty_counter = 0; 
		state_reg = 0; 
		state_reg_lopri_last = 0; 
		mem_addr_o_lopri_last = 0; 
	end 
endmodule 
