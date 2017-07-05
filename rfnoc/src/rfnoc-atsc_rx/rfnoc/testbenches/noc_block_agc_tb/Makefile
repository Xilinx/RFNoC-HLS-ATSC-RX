

# 
# Copyright 2017 Rabbit Ears, RFNoC Challenge.
# Author: Andrew Lanez
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
SIM_TOP = noc_block_agc_tb

# Add test bench, user design under test, and
# additional user created files
SIM_SRCS = \
$(abspath noc_block_agc_tb.sv) \
$(abspath ../../fpga-src/noc_block_agc.v) \
$(abspath agc_in.sv) \
$(abspath agc_out.sv) \
$(abspath ../../fpga-src/agc/agc_ap_faddfsub_6_full_dsp_32/agc_ap_faddfsub_6_full_dsp_32.xci) \
$(abspath ../../fpga-src/agc/agc_ap_fcmp_0_no_dsp_32/agc_ap_fcmp_0_no_dsp_32.xci) \
$(abspath ../../fpga-src/agc/agc_ap_fmul_2_max_dsp_32/agc_ap_fmul_2_max_dsp_32.xci) \
$(abspath ../../fpga-src/agc/agc_faddfsub_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/agc/agc_fcmp_32ns_32ns_1_1.v) \
$(abspath ../../fpga-src/agc/agc_fmul_32ns_32ns_32_4_max_dsp.v) \
$(abspath ../../fpga-src/agc/agc.v)

MODELSIM_USER_DO =

#-------------------------------------------------
# Bottom-of-Makefile
#-------------------------------------------------
# Include all simulator specific makefiles here
# Each should define a unique target to simulate
# e.g. xsim, vsim, etc and a common "clean" target
include $(BASE_DIR)/../tools/make/viv_simulator.mak
