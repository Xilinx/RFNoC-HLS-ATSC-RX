

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
SIM_TOP = noc_block_rsdec_tb

# Add test bench, user design under test, and
# additional user created files
SIM_SRCS = \
$(abspath noc_block_rsdec_tb.sv) \
$(abspath ../../fpga-src/noc_block_rsdec.v) \
$(abspath rsdec_in.sv) \
$(abspath rsdec_out.sv) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_rs_char_alpha_to_value_rom.dat) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_rs_char_alpha_to_value.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_rs_char_index_of_value_rom.dat) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_rs_char_index_of_value.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_rs_char_lambda.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_rs_char_s.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_rs_char.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode_tmp.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_decode.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_modnn.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_mul_32s_8ns_32_5.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_rs_in_data.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl_rs_out_data_assign.v) \
$(abspath ../../fpga-src/rsdec/atsc_rsdecoder_impl.v)

MODELSIM_USER_DO =

#-------------------------------------------------
# Bottom-of-Makefile
#-------------------------------------------------
# Include all simulator specific makefiles here
# Each should define a unique target to simulate
# e.g. xsim, vsim, etc and a common "clean" target
include $(BASE_DIR)/../tools/make/viv_simulator.mak
