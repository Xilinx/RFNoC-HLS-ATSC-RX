# RFNoC-HLS-ATSC-RX

Andrew Valenzuela Lanez | <andrew.lanez@navy.mil>  
Sachin Bharadwaj Sundramurthy | <sbharad@eng.ucsd.edu>  
Alireza Khodamoradi | <alirezak@eng.ucsd.edu>

### About
This repo contains the ```atsc_rx``` RFNoC module for GNU Radio. Its blocks are to be implemented onto the FPGA of an RFNoC-suppported USRP (e.g., E310/E312 or X300/X310) using RFNoC and Xilinx Vivado Design Suite 2015.4. They can plug-and-play into the GNU Radio [gr-dtv] ATSC receiver example and process an ATSC signal into a playable video file.

The ```atsc_rx``` module includes these blocks:
- RFNoC: ATSC RX Filter
- RFNoC: ATSC Receiver FPLL
- RFNoC: DC Blocker
- RFNoC: AGC
- RFNoC: ATSC Viterbi Decoder
- RFNoC: ATSC Deinterleaver
- RFNoC: ATSC Reed-Solomon Decoder
- RFNoC: ATSC Depad
- RFNoC: ATSC RX Filter-FPLL
- RFNoC: DC Blocker-AGC

All blocks above were built with these integrated features:
- Vivado HLS Source & Testbench
- HDL Testbench
- FPGA Integration
- UHD Integration
- GNU Radio Integration

### Directories
Here are descriptions of the top-level directories:

| Folder | Contents |
| --- | --- |
| [rfnoc]| RFNoC main source tree files needed to build all blocks that have been verified operational on hardware and in GNU Radio. The [src/rfnoc-atsc_rx] subfolder contains files needed for UHD and GNU Radio integration. The [src/uhd-fpga/usrp3] subfolder contains files needed for FPGA integration.
|[hls_patch] | Patch from the RFNoC dev team for the image builder to detect various needed source files. May not be needed in current or future RFNoC releases.
|[blocks\_in\_progress]| Nearly completed blocks either more optimized for throughput or have more features than their counterparts in the main source tree but need timing, interface, or hardware performance issues resolved.

Noteworthy subdirectories:

|Subfolder|Contents|
| --- | --- |
|[hls]| C++ source and testbench files for Vivado HLS.
|[testbenches]| SystemVerilog HDL testbench directories.
|[examples]| USRP X310 HG ```.bit``` FPGA image files of built blocks (not all blocks fit in a single image and had to be split) and associated GRC flowgraphs.

### Build
Assuming RFNoC has been setup ([Getting Started with RFNoC Development][kb] covers all steps needed to build anything RFNoC in general), here are some extra build pointers:

##### Setup OOT Module and Blocks
Initialize the module:
```sh
$ cd {USER_PREFIX}/src
$ rfnocmodtool newmod atsc_rx
$ cd rfnoc-atsc_rx
```
Initialize any or all of the following blocks. Press ```Enter``` four times between each entry to accept default values:
```sh
$ rfnocmodtool add rxfilt
$ rfnocmodtool add fpll
$ rfnocmodtool add dcr
$ rfnocmodtool add agc
$ rfnocmodtool add viterbi
$ rfnocmodtool add deint
$ rfnocmodtool add rsdec
$ rfnocmodtool add depad
$ rfnocmodtool add fltpll
$ rfnocmodtool add dcragc
```

Copy/paste/replace/merge all contents of [rfnoc] into the local {USER_PREFIX} RFNoC development directory to populate files needed throughout the tree for GNU Radio, UHD, and FPGA integration.

##### GNU Radio and UHD integration
Install the files that were just copied:
```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

RFNoC: FIFO blocks can be updated with more data type options to connect with ```atsc_rx``` blocks in GNU Radio Companion. Replace [uhd\_rfnoc\_fifo.xml] into ```{USER_PREFIX}/src/gr-ettus/grc``` then:
```sh
$ cd {USER_PREFIX}/src/gr-ettus/build
$ make
$ sudo make install
```

##### HDL Testbench
The RTL Export feature in Vivado HLS 2015.4 ([user guide][ug902]) can be used to first generate Verilog and Xilinx IP source files (```*.v```, ```*.dat``` and/or XCI folders) of a block. Paths to those files need to be assigned to the ```SIM_SRCS``` variable in the respective ```{USER_PREFIX}/src/rfnoc-atsc_rx/rfnoc/testbenches/noc_block_{NAME}_tb/Makefile``` of a block. Then run the testbench from the build directory:
```sh
$ cd {USER_PREFIX}/src/rfnoc-atsc_rx/build
$ make test_tb
$ make noc_block_[NAME]_tb
```
Note: ```make test_tb``` only needs to be run once for the ```atsc_rx``` OOT module, not per RFNoC block.

##### FPGA Integration
The following files specify which blocks are to be built into the FPGA. They are already populated with specified blocks for reference and can be modified to build a different combination of blocks as desired. Specified blocks must match among all files.
```sh
{USER_PREFIX}/src/rfnoc-atsc_rx/rfnoc/Makefile.srcs
{USER_PREFIX}/src/uhd-fpga/usrp3/lib/hls/Makefile.inc
{USER_PREFIX}/src/uhd-fpga/usrp3/top/x300/Makefile.srcs
{USER_PREFIX}/src/uhd-fpga/usrp3/top/x300/rfnoc_ce_auto_inst_x310.v
```
The last file assumes the image is for a USRP X310. Modify the appropriate file respective the USRP being used. Then build the image:
```sh
$ cd {USER_PREFIX}/src/uhd-fpga/usrp3/top/x300
$ make X310_RFNOC_HLS_HG
```
The last command assumes a USRP X310 and 1GigE Port0/10GigE Port1. Use a make command appropriate to the USRP and Ethernet port type being used. Use ```make cleanall``` if IP needs to be cleaned before a new build.



[gr-dtv]: <https://github.com/gnuradio/gnuradio/tree/master/gr-dtv/examples>

[rfnoc]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/rfnoc>

[hls_patch]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/hls_patch>

[blocks\_in\_progress]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/blocks_in_progress>

[kb]: <http://kb.ettus.com/Getting_Started_with_RFNoC_Development>

[examples]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/rfnoc/src/rfnoc-atsc_rx/examples>

[hls]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/rfnoc/src/uhd-fpga/usrp3/lib/hls>

[testbenches]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/rfnoc/src/rfnoc-atsc_rx/rfnoc/testbenches>

[src/rfnoc-atsc_rx]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/rfnoc/src/rfnoc-atsc_rx>

[src/uhd-fpga/usrp3]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/rfnoc/src/uhd-fpga/usrp3>

[uhd\_rfnoc\_fifo.xml]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/rfnoc/uhd_rfnoc_fifo.xml>

[ug902]: <https://www.xilinx.com/support/documentation/sw_manuals/xilinx2015_4/ug902-vivado-high-level-synthesis.pdf>
