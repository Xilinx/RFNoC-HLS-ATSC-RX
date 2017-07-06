##### SystemVerilog Testbench

The RTL Export feature in Vivado HLS 2015.4 ([user guide][ug902]) can be used to first generate Verilog and Xilinx IP source files (```*.v```, ```*.dat``` and/or XCI folders) of a block. Paths to those files need to be assigned to the ```SIM_SRCS``` variable in the respective ```{USER_PREFIX}/src/rfnoc-atsc_rx/rfnoc/testbenches/noc_block_{NAME}_tb/Makefile``` of a block.  Then run the testbench from the build directory:
```sh
$ cd {USER_PREFIX}/src/rfnoc-atsc_rx/build
$ make test_tb
$ make noc_block_[NAME]_tb
```
Note: ```make test_tb``` only needs to be run once for the ```atsc_rx``` OOT module, not per RFNoC block.


[ug902]: <https://www.xilinx.com/support/documentation/sw_manuals/xilinx2015_4/ug902-vivado-high-level-synthesis.pdf>
