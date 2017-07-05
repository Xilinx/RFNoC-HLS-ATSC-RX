

# 
# Copyright 2017 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

#-------------------------------------------------
# Top-of-Makefile
#-------------------------------------------------
# Define BASE_DIR to point to the "top" dir
BASE_DIR = $(FPGA_TOP_DIR)/usrp3/top
# Include viv_sim_preample after defining BASE_DIR
include $(BASE_DIR)/../tools/make/viv_sim_preamble.mak

#-------------------------------------------------
# Testbench Specific
#-------------------------------------------------
# Define only one toplevel module
SIM_TOP = noc_block_viterbi_tb

# Add test bench, user design under test, and
# additional user created files
SIM_SRCS = \
$(abspath noc_block_viterbi_tb.sv) \
$(abspath ../../fpga-src/noc_block_viterbi.v) \
$(abspath viterbi_in.sv) \
$(abspath viterbi_out.sv) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_ap_fadd_6_full_dsp_32) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_ap_faddfsub_6_full_dsp_32) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_ap_fcmp_0_no_dsp_32) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_distances.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_d_transition_table_rom.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_d_transition_table.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_d_was_sent_rom.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_d_was_sent.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_viterbi_decoder_impl_viterbi_d_1_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_viterbi_decoder_impl_viterbi_d_1.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_viterbi_decoder_impl_viterbi_d_3_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_decode_viterbi_decoder_impl_viterbi_d_3.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_fadd_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_faddfsub_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_fcmp_32ns_32ns_1_1.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_mul_4ns_11ns_14_3.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_mul_4ns_13ns_15_3.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_mul_5ns_11ns_15_3.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_mul_5ns_9ns_13_3.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_mul_mul_14ns_16ns_30_3.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_mul_mul_15ns_17ns_32_3.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_urem_14ns_11ns_14_18.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_viterbi_in_data_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_viterbi_in_data.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_viterbi_in_pli_flags_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_viterbi_in_pli_flags.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_viterbi_out_data_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_viterbi_out_data.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_dibits.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_enco_which_dibits_rom.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_enco_which_dibits.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_enco_which_syms_rom.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_enco_which_syms.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_out_copy.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_symbols.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_fifo_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_fifo.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_viterbi_d_4_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_viterbi_d_4.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_viterbi_d_5_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_viterbi_d_5.v) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_viterbi_d_ram.dat) \
$(abspath ../../fpga-src/viterbi/atsc_viterbi_impl_work_viterbi_decoder_impl_viterbi_d.v)

MODELSIM_USER_DO =

#-------------------------------------------------
# Bottom-of-Makefile
#-------------------------------------------------
# Include all simulator specific makefiles here
# Each should define a unique target to simulate
# e.g. xsim, vsim, etc and a common "clean" target
include $(BASE_DIR)/../tools/make/viv_simulator.mak
