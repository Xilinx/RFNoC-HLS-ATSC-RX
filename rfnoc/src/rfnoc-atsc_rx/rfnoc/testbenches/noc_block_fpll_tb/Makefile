

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
SIM_TOP = noc_block_fpll_tb

# Add test bench, user design under test, and
# additional user created files
SIM_SRCS = \
$(abspath noc_block_fpll_tb.sv) \
$(abspath ../../fpga-src/noc_block_fpll.v) \
$(abspath fpll_in.sv) \
$(abspath fpll_out.sv) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_dadd_6_full_dsp_64/atsc_fpll_ap_dadd_6_full_dsp_64.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_dcmp_0_no_dsp_64/atsc_fpll_ap_dcmp_0_no_dsp_64.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_ddiv_41_no_dsp_64/atsc_fpll_ap_ddiv_41_no_dsp_64.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_fadd_6_full_dsp_32/atsc_fpll_ap_fadd_6_full_dsp_32.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_faddfsub_6_full_dsp_32/atsc_fpll_ap_faddfsub_6_full_dsp_32.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_fcmp_0_no_dsp_32/atsc_fpll_ap_fcmp_0_no_dsp_32.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_fdiv_14_no_dsp_32/atsc_fpll_ap_fdiv_14_no_dsp_32.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_fmul_2_max_dsp_32/atsc_fpll_ap_fmul_2_max_dsp_32.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_fpext_0_no_dsp_32/atsc_fpll_ap_fpext_0_no_dsp_32.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_fptrunc_0_no_dsp_64/atsc_fpll_ap_fptrunc_0_no_dsp_64.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_fsub_6_full_dsp_32/atsc_fpll_ap_fsub_6_full_dsp_32.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ap_sitofp_4_no_dsp_64/atsc_fpll_ap_sitofp_4_no_dsp_64.xci) \
$(abspath ../../fpga-src/fpll/atsc_fpll_atan2_cordic_floatdouble.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_atan2_cordic_float_s.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_atan2_range_redux_cordic_hls_cordic_ctab_table_128_V_rom.dat) \
$(abspath ../../fpga-src/fpll/atsc_fpll_atan2_range_redux_cordic_hls_cordic_ctab_table_128_V.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_atan2_range_redux_cordic.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_big_mult_v3small_71_24_17_s_pp_V.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_big_mult_v3small_71_24_17_s.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_dadd_64ns_64ns_64_8_full_dsp.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_dcmp_64ns_64ns_1_1.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_ddiv_64ns_64ns_64_43.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_fadd_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_faddfsub_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_fcmp_32ns_32ns_1_1.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_fdiv_32ns_32ns_32_16.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_fmul_32ns_32ns_32_4_max_dsp.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_fpext_32ns_64_1.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_fptrunc_64ns_32_1.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_fsub_32ns_32ns_32_8_full_dsp.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_mul_32s_31ns_62_7.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_mul_41s_24ns_41_6.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_mul_9ns_9ns_18_3.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_mul_mul_17ns_15s_32_3.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_mul_mul_23s_17ns_40_3.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_mux_16to1_sel4_1_1.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_mux_8to1_sel3_1_1.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_my_to_float_31_1_s.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_ref_4oPi_table_100_V_rom.dat) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_ref_4oPi_table_100_V.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_sin_cos_K0_V_rom.dat) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_sin_cos_K0_V.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_sin_cos_K1_V_rom.dat) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_sin_cos_K1_V.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_sin_cos_K2_V_rom.dat) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf_hls_sin_cos_K2_V.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sinf_or_cosf.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_sitofp_64s_32_6.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll.v) \
$(abspath ../../fpga-src/fpll/atsc_fpll_work.v)

MODELSIM_USER_DO =

#-------------------------------------------------
# Bottom-of-Makefile
#-------------------------------------------------
# Include all simulator specific makefiles here
# Each should define a unique target to simulate
# e.g. xsim, vsim, etc and a common "clean" target
include $(BASE_DIR)/../tools/make/viv_simulator.mak
