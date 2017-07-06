# gr-atsc_rx
Integrating UHD and fpga files into one directory

Based on copy block - Bypassing input into output

To run testbenches:
Must use Vivado HLS to build .v and .dat files then assign their paths to SIM_SRCS variable in {USER_PREFIX}/src/rfnoc-atsc_rx/rfnoc/testbenchs/noc_block{BLOCK NAME}_tb/Makefile files.
