/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

`timescale 1ns/1ps
`define NS_PER_TICK 1
`define NUM_TEST_CASES 5

`include "sim_exec_report.vh"
`include "sim_clks_rsts.vh"
`include "sim_rfnoc_lib.svh"
`include "viterbi_in.sv"
`include "viterbi_out.sv"

module noc_block_viterbi_tb();
  `TEST_BENCH_INIT("noc_block_viterbi",`NUM_TEST_CASES,`NS_PER_TICK);
  localparam BUS_CLK_PERIOD = $ceil(1e9/166.67e6);
  localparam CE_CLK_PERIOD  = $ceil(1e9/200e6);
  localparam NUM_CE         = 1;  // Number of Computation Engines / User RFNoC blocks to simulate
  localparam NUM_STREAMS    = 1;  // Number of test bench streams
  `RFNOC_SIM_INIT(NUM_CE, NUM_STREAMS, BUS_CLK_PERIOD, CE_CLK_PERIOD);
  `RFNOC_ADD_BLOCK(noc_block_viterbi, 0);

  localparam SPP = 16; // Samples per packet

  /********************************************************
  ** Verification
  ********************************************************/
  initial begin : tb_main
    string s;
    logic [31:0] random_word;
    logic [63:0] readback;

    /********************************************************
    ** Test 1 -- Reset
    ********************************************************/
    `TEST_CASE_START("Wait for Reset");
    while (bus_rst) @(posedge bus_clk);
    while (ce_rst) @(posedge ce_clk);
    `TEST_CASE_DONE(~bus_rst & ~ce_rst);

    /********************************************************
    ** Test 2 -- Check for correct NoC IDs
    ********************************************************/
    `TEST_CASE_START("Check NoC ID");
    // Read NOC IDs
    tb_streamer.read_reg(sid_noc_block_viterbi, RB_NOC_ID, readback);
    $display("Read viterbi NOC ID: %16x", readback);
    `ASSERT_ERROR(readback == noc_block_viterbi.NOC_ID, "Incorrect NOC ID");
    `TEST_CASE_DONE(1);

    /********************************************************
    ** Test 3 -- Connect RFNoC blocks
    ********************************************************/
    `TEST_CASE_START("Connect RFNoC blocks");
    `RFNOC_CONNECT(noc_block_tb,noc_block_viterbi,SC16,SPP);
    `RFNOC_CONNECT(noc_block_viterbi,noc_block_tb,SC16,SPP);
    `TEST_CASE_DONE(1);

    /********************************************************
    ** Test 4 -- Write / readback user registers
    ********************************************************/
    `TEST_CASE_START("Write / readback user registers");
    random_word = $random();
    tb_streamer.write_user_reg(sid_noc_block_viterbi, noc_block_viterbi.SR_TEST_REG_0, random_word);
    tb_streamer.read_user_reg(sid_noc_block_viterbi, 0, readback);
    $sformat(s, "User register 0 incorrect readback! Expected: %0d, Actual %0d", readback[31:0], random_word);
    `ASSERT_ERROR(readback[31:0] == random_word, s);
    random_word = $random();
    tb_streamer.write_user_reg(sid_noc_block_viterbi, noc_block_viterbi.SR_TEST_REG_1, random_word);
    tb_streamer.read_user_reg(sid_noc_block_viterbi, 1, readback);
    $sformat(s, "User register 1 incorrect readback! Expected: %0d, Actual %0d", readback[31:0], random_word);
    `ASSERT_ERROR(readback[31:0] == random_word, s);
    `TEST_CASE_DONE(1);

    /********************************************************
    ** Test 5 -- Test sequence
    ********************************************************/
    // viterbi's user code is tested
    `TEST_CASE_START("Test sequence");
    fork
      begin
        cvita_payload_t send_payload;
        for (int i = 0; i < 1024; i+=2) begin
          send_payload.push_back((32'(shortreal(i)),32'(shortreal(i+1)));
        end
        tb_streamer.send(send_payload);
      end
      begin
        cvita_payload_t recv_payload;
        cvita_metadata_t md;
        logic [63:0] expected_value;
        tb_streamer.recv(recv_payload,md);
        for (int i = 0; i < 64; i++) begin
          //expected_value = i;
	  read_value = recv_payload[i];
          $display("Value received! %d, Received: %f,   %f", i, bitstoshortreal(read_value[31:0]),bitstoshortreal(read_value[63:32]));
          //`ASSERT_ERROR(recv_payload[i] == expected_value, s);
        end
      end
    join
/*
    begin
      localparam PKTS = 12; // Number of pkts (1 pkt=64 words, 1 word=4 bytes)
      localparam WPL = 64; // Words per LAST assertion
      localparam THRESH = 0.0001;
      logic [31:0] exp_val, recv_val, diff;
      logic last;
      fork
      begin
        for (int i=0, j=0; i < PKTS*WPL; i++, j+=4) begin
          tb_streamer.push_word({in[j],in[j+1],in[j+2],in[j+3]}, (i+1)%WPL == 0);
        end
      end
      begin
        for (int i=0, j=0; i < PKTS*WPL; i++, j+=4) begin
          exp_val = {out[j],out[j+1],out[j+2],out[j+3]};
          tb_streamer.pull_word(recv_val, last);
          diff = exp_val - recv_val;
          $sformat(s, "Word #%0d: Incorrect value received! Expected: %0d %0d %0d %0d, Received: %0d %0d %0d %0d", i, {24'b0,exp_val[31:24]},{24'b0,exp_val[23:16]},{24'b0,exp_val[15:8]},{24'b0,exp_val[7:0]}, {24'b0,recv_val[31:24]},{24'b0,recv_val[23:16]},{24'b0,recv_val[15:8]},{24'b0,recv_val[7:0]});
          `ASSERT_ERROR(-1*THRESH<diff && diff<THRESH, s);
        end
      end
      join
    end
*/
    `TEST_CASE_DONE(1);
    `TEST_BENCH_DONE;

  end
endmodule
