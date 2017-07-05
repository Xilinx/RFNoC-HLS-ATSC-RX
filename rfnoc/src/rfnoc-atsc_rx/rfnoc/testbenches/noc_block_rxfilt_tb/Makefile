

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
SIM_TOP = noc_block_rxfilt_tb

# Add test bench, user design under test, and
# additional user created files
SIM_SRCS = \
$(abspath noc_block_rxfilt_tb.sv) \
$(abspath ../../fpga-src/noc_block_rxfilt.v) \
$(abspath rxfilt_in.sv) \
$(abspath rxfilt_out.sv) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_dadddsub_6_full_dsp_64/atsc_rx_filter_ap_dadddsub_6_full_dsp_64.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_fadd_6_full_dsp_32/atsc_rx_filter_ap_fadd_6_full_dsp_32.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_faddfsub_6_full_dsp_32/atsc_rx_filter_ap_faddfsub_6_full_dsp_32.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_fdiv_14_no_dsp_32/atsc_rx_filter_ap_fdiv_14_no_dsp_32.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_fmul_2_max_dsp_32/atsc_rx_filter_ap_fmul_2_max_dsp_32.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_fpext_0_no_dsp_32/atsc_rx_filter_ap_fpext_0_no_dsp_32.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_fptrunc_0_no_dsp_64/atsc_rx_filter_ap_fptrunc_0_no_dsp_64.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_ap_sitofp_4_no_dsp_64/atsc_rx_filter_ap_sitofp_4_no_dsp_64.xci) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_dadddsub_64ns_64ns_64_8_full_dsp.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_fadd_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_faddfsub_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_fdiv_32ns_32ns_32_16.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_0_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_0.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_10_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_10.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_11_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_11.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_12_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_12.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_13_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_13.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_14_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_14.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_15_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_15.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_16_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_16.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_17_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_17.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_18_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_18.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_19_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_19.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_1_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_1.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_20_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_20.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_21_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_21.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_22_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_22.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_23_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_23.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_24_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_24.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_25_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_25.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_26_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_26.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_27_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_27.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_28_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_28.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_29_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_29.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_2_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_2.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_30_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_30.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_31_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_31.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_3_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_3.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_4_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_4.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_5_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_5.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_6_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_6.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_7_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_7.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_8_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_8.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_9_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_d_filters_9.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_input_M_real.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_mask_table2_rom.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_mask_table2.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_prev_tail_M_real_ram.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter_prev_tail_M_real.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_filter.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_fmul_32ns_32ns_32_4_max_dsp.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_fpext_32ns_64_1.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_fptrunc_64ns_32_1.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_mux_16to1_sel4_32_1.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_mux_32to1_sel5_32_1.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_pfb_in_M_real.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_pfb_out_M_real_ram.dat) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_pfb_out_M_real.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter_sitofp_64ns_32_6.v) \
$(abspath ../../fpga-src/rxfilt/atsc_rx_filter.v)

MODELSIM_USER_DO =

#-------------------------------------------------
# Bottom-of-Makefile
#-------------------------------------------------
# Include all simulator specific makefiles here
# Each should define a unique target to simulate
# e.g. xsim, vsim, etc and a common "clean" target
include $(BASE_DIR)/../tools/make/viv_simulator.mak
