############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2015 Xilinx Inc. All rights reserved.
############################################################
open_project viterbi_decoder
set_top decode
add_files atsc_single_viterbi.cpp
add_files atsc_viterbi_decoder_impl.cpp
add_files -tb tb_main.cpp
add_files -tb viterbi_in_f
add_files -tb viterbi_out_di_in_uchar
open_solution "solution1"
set_part {xc7k410tffg900-2}
create_clock -period 10 -name default
#source "./viterbi_decoder/solution1/directives.tcl"
#csim_design -clean -setup
#csynth_design
#cosim_design
#export_design -format ip_catalog
