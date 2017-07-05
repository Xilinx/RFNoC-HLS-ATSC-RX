`timescale 1ns/1ps
`define NS_PER_TICK 1
`define NUM_TEST_CASES 5

`include "sim_exec_report.vh"
`include "sim_clks_rsts.vh"
`include "sim_rfnoc_lib.svh"

module noc_block_depad_tb();
  `TEST_BENCH_INIT("noc_block_depad",`NUM_TEST_CASES,`NS_PER_TICK);
  localparam BUS_CLK_PERIOD = $ceil(1e9/166.67e6);
  localparam CE_CLK_PERIOD  = $ceil(1e9/200e6);
  localparam NUM_CE         = 1;  // Number of Computation Engines / User RFNoC blocks to simulate
  localparam NUM_STREAMS    = 1;  // Number of test bench streams
  `RFNOC_SIM_INIT(NUM_CE, NUM_STREAMS, BUS_CLK_PERIOD, CE_CLK_PERIOD);
  `RFNOC_ADD_BLOCK(noc_block_depad, 0);

  localparam INPUT_SPP  = 64; // Samples per packet
  localparam OUTPUT_SPP = 47; // Samples per packet

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
    tb_streamer.read_reg(sid_noc_block_depad, RB_NOC_ID, readback);
    $display("Read DEPAD NOC ID: %16x", readback);
    `ASSERT_ERROR(readback == noc_block_depad.NOC_ID, "Incorrect NOC ID");
    `TEST_CASE_DONE(1);

    /********************************************************
    ** Test 3 -- Connect RFNoC blocks
    ********************************************************/
    `TEST_CASE_START("Connect RFNoC blocks");
    `RFNOC_CONNECT(noc_block_tb,noc_block_depad,U32,INPUT_SPP);
    `RFNOC_CONNECT(noc_block_depad,noc_block_tb,U32,OUTPUT_SPP);
    `TEST_CASE_DONE(1);

    /********************************************************
    ** Test 4 -- Write / readback user registers
    ********************************************************/
    `TEST_CASE_START("Test sequence 1");
    fork
      begin
        cvita_payload_t send_payload;
        cvita_metadata_t tx_md;
        for (int i = 0; i < INPUT_SPP; i+=2) begin
          send_payload.push_back({32'(i),32'(i+1)});
        end
        $display("Writing Data to Bus");
        tx_md.eob = 1'b1;
        tb_streamer.send(send_payload);
      end
      begin
        cvita_payload_t recv_payload;
        cvita_metadata_t md;
        logic [63:0] expected_value;
        $display("Starting to read data");
        tb_streamer.recv(recv_payload,md);
        for (longint i = 0; i < OUTPUT_SPP; i+=2) begin
          expected_value = (i<<32)+i+1;
          $display("Expected: %x, Received: %x", expected_value, recv_payload[i/2]);
        end
      end
    join
    `TEST_CASE_DONE(1);

    /********************************************************
    ** Test 5 -- Test sequence
    ********************************************************/
    // depad's user code is a loopback, so we should receive
    // back exactly what we send
    `TEST_CASE_START("Test sequence 2");
    fork
      begin
        cvita_payload_t send_payload;
        cvita_metadata_t tx_md;
        for (int i = 0; i < INPUT_SPP; i+=2) begin
          send_payload.push_back({32'(i),32'(i)});
        end
        $display("Writing Data to Bus");
        tx_md.eob = 1'b1;
        tb_streamer.send(send_payload);
      end
      begin
        cvita_payload_t recv_payload;
        cvita_metadata_t md;
        logic [63:0] expected_value;
        $display("Starting to read data");
        tb_streamer.recv(recv_payload,md);
        for (longint i = 0; i < OUTPUT_SPP; i+=2) begin
          expected_value = (i<<32)+i;
          $display("Expected: %x, Received: %x", expected_value, recv_payload[i/2]);
        end
      end
    join
    `TEST_CASE_DONE(1);
    `TEST_BENCH_DONE;

  end
endmodule

