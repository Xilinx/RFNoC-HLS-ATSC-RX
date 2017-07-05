

# 
# Copyright 2017 Rabbit Ears, RFNoC Challenge.
# Andrew Lanez
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
SIM_TOP = noc_block_deint_tb

# Add test bench, user design under test, and
# additional user created files
SIM_SRCS = \
$(abspath noc_block_deint_tb.sv) \
$(abspath ../../fpga-src/noc_block_deint.v) \
$(abspath deint_in.sv) \
$(abspath deint_out.sv) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_in1_data.v) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_mac_muladd_9ns_15s_15ns_15_2.v) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_out1_data.v) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl.v) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_work_alignment_fifo_ram.dat) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_work_alignment_fifo.v) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_work_fifo_ram.dat) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_work_fifo.v) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_work_m_fifo_pointer_ram.dat) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_work_m_fifo_pointer.v) \
$(abspath ../../fpga-src/deint/atsc_deinterleaver_impl_work.v)

MODELSIM_USER_DO =

#-------------------------------------------------
# Bottom-of-Makefile
#-------------------------------------------------
# Include all simulator specific makefiles here
# Each should define a unique target to simulate
# e.g. xsim, vsim, etc and a common "clean" target
include $(BASE_DIR)/../tools/make/viv_simulator.mak
