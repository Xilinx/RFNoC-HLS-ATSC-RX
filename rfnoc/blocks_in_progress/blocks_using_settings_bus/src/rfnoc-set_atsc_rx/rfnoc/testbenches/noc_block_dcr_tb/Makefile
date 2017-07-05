

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
SIM_TOP = noc_block_dcr_tb

# Add test bench, user design under test, and
# additional user created files
SIM_SRCS = \
$(abspath noc_block_dcr_tb.sv) \
$(abspath dcr_in.sv) \
$(abspath dcr_out.sv) \
$(abspath ../../fpga-src/noc_block_dcr.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_ap_faddfsub_6_full_dsp_32) \
$(abspath ../../fpga-src/dcr/dc_blocker_ap_fdiv_14_no_dsp_32) \
$(abspath ../../fpga-src/dcr/dc_blocker_ap_fsub_6_full_dsp_32) \
$(abspath ../../fpga-src/dcr/dc_blocker_dcr_in.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_faddfsub_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_fdiv_32ns_32ns_32_16.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_filter.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_fsub_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_last.v) \
$(abspath ../../fpga-src/dcr/dc_blocker.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_work_dcr_d_delay_line_ram.dat) \
$(abspath ../../fpga-src/dcr/dc_blocker_work_dcr_d_delay_line.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_work_dcr_d_ma_0_d_delay_line_0_rom.dat) \
$(abspath ../../fpga-src/dcr/dc_blocker_work_dcr_d_ma_0_d_delay_line_0.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_work_dcr_d_ma_0_d_delay_line_14_ram.dat) \
$(abspath ../../fpga-src/dcr/dc_blocker_work_dcr_d_ma_0_d_delay_line_14.v) \
$(abspath ../../fpga-src/dcr/dc_blocker_work.v)

MODELSIM_USER_DO =

#-------------------------------------------------
# Bottom-of-Makefile
#-------------------------------------------------
# Include all simulator specific makefiles here
# Each should define a unique target to simulate
# e.g. xsim, vsim, etc and a common "clean" target
include $(BASE_DIR)/../tools/make/viv_simulator.mak
