# RFNoC-HLS-ATSC-RX

Andrew Valenzuela Lanez | <andrew.lanez@navy.mil>  
Sachin Bharadwaj Sundramurthy | <sbharad@eng.ucsd.edu>  
Alireza Khodamoradi | <alirezak@eng.ucsd.edu>

### About
This repo contains the RFNoC ```atsc_rx``` OOT module for GNU Radio. Its blocks are to be implemented onto the FPGA of an RFNoC-suppported USRP (e.g., E3xx series or X3xx series) using RFNoC and Xilinx Vivado Design Suite 2015.4. They can plug-and-play into the GNU Radio [gr-dtv][grdtv] ATSC receiver example and process an ATSC signal into a playable video file. Real time playback of a live ATSC signal processed through the ```gr-dtv``` ATSC receiver is possible on high-performance computers but not on most commodity computers. This makes ATSC receiver blocks ideal candidates for porting into RFNoC. Computation intensive tasks can be offloaded to FPGA logic while applying high-level synthesis optimization techniques to improve receiver throughput. This can bring GNU Radio ever closer to achieving real time ATSC playback on a typical commodity computer.

Example flowgraph with ```atsc_rx``` RFNoC blocks:

![ex1](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/ex1.png?raw=true)

Screenshot of video from a live ATSC signal processed through ```atsc_rx``` RFNoC blocks:

![dog](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/dog.png?raw=true)

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

## Directories
Here are descriptions of the top-level directories:

| Folder | Contents |
| --- | --- |
| [rfnoc]| RFNoC main source tree files needed to build all blocks that have been verified operational on hardware and in GNU Radio. The [src/rfnoc-atsc_rx] subfolder contains files needed for UHD and GNU Radio integration. The [src/uhd-fpga/usrp3] subfolder contains files needed for FPGA integration.
|[hls_patch] | Patch provided by the RFNoC dev team for image builder to detect various source files needed. Patch is needed for the current RFNoC release as of 7 July 2017.
|[blocks\_in\_progress]| Nearly completed blocks either more optimized for throughput or have more features than their counterparts in the main source tree but need timing, interface, or hardware performance issues resolved.

Noteworthy subdirectories:

|Subfolder|Contents|
| --- | --- |
|[hls]| C++ source and testbench files for Vivado HLS.
|[testbenches]| SystemVerilog HDL testbench directories.
|[examples]| USRP X310 HG ```.bit``` FPGA image file of some blocks (not all blocks fit in a single image due to CE count, not area utilization) and associated GRC flowgraph.

## Build
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
Note: Running Vivado HLS > RTL Export > Evaluate > Verilog can save time by predicting whether the design will meet timing rather than committing mutiple hours to build the image and then finding out. The following files specify which blocks are to be built into the FPGA. They are already populated with specified blocks for reference and can be modified to build a different combination of blocks as desired. Specified blocks must match among all files.
```sh
{USER_PREFIX}/src/rfnoc-atsc_rx/rfnoc/Makefile.srcs
{USER_PREFIX}/src/uhd-fpga/usrp3/lib/hls/Makefile.inc
{USER_PREFIX}/src/uhd-fpga/usrp3/top/x300/Makefile.srcs (or Makefile.OOT.inc in newer RFNoC releases)
{USER_PREFIX}/src/uhd-fpga/usrp3/top/x300/rfnoc_ce_auto_inst_x310.v
```
The last file assumes the image is for a USRP X310. Modify the appropriate file respective the USRP being used. Then build the image:
```sh
$ cd {USER_PREFIX}/src/uhd-fpga/usrp3/top/x300
$ make X310_RFNOC_HLS_HG
```
The last command assumes a USRP X310 and 1GigE Port0/10GigE Port1. Use a make command appropriate to the USRP and Ethernet port type being used. Use ```make cleanall``` if IP needs to be cleaned before a new build.

## Design Considerations
The decision on which blocks to port into RFNoC hinged on two factors:
 
**Frontend proximity:** Porting frontend blocks from software into hardware increases deterministic processing before the datastream falls under the whim of an operating system scheduler. RX Filter, FPLL, DC Blocker, and AGC were selected.

**Bottlenecks:** Blocks that have higher consumption of runtime resources or cause buffers to fill are ideal candidates for porting. ControlPort Performance Monitor shows runtime consumption is primarily from Viterbi Decoder and secondarily from Reed-Solomon Decoder and top buffer consumption is from RX Filter:

![gr-runtime](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/gr_runtime.png?raw=true)

![gr-buffers](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/gr_buffers.png?raw=true)

Deinterleaver, Derandomizer, and Depad were eventually targeted due to their simplicity or for the sake of completeness around the FEC (forward error correction) blocks. All targeted blocks from the gr-dtv library are boxed out in red:

![gr-dtv](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/gr-dtv.png?raw=true)

The following flowgraph shows 6 of 10 blocks implemented in RFNoC. RX Filter was combined with FPLL and DC Blocker was combined with AGC at the HLS source level so that more CEs (Computation Engines) could fit in one FPGA image (combined area utilization of all blocks was not an issue). Throttle compensates for a rate mismatch (an action item for future iterations and discussed later).

![grc](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/grc.png?raw=true)

Two key parameters in the ```gr-dtv``` implementation of the receiver that drive sample rate requirements for all blocks are the 6.25 MHz sample rate coming out of the DDC (digital down coverter) at the receiver frontend and the oversampling ratio of the first block, RX Filter. It was decided that the 6.25 MHz input rate should not be modified to pass in a 6 MHz bandwidth ATSC channel:

![spectrum](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/spectrum.png?raw=true)

Reasonable oversampling ratios were found to range between 1.1 to 2 for the ```gr-dtv``` ATSC receiver software implementation to accumulate enough data to output video for playback. For an initial pass at implementing on hardware, it was decided to define target sampling rates based on the oversampling ratio of 1.1 to make implementation less constrained. Then, time permitting, iterate from there by increasing target rates and fine tune sample rates to match between blocks.

## Implementation Notes
Workflow in the RFNoC framework is already well documented at [Getting Started with RFNoC Development][kb]. The following focuses on implementation using Vivado HLS 2015.4 shown in the top part of this high-level workflow followed by results:

![workflow](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/workflow.png?raw=true)

##### Developing HLS Source
Signal processing source files were written in C++ to be synthesizable. Vivado HLS optimization directives such as ```#pragma HLS PIPELINE```, ```#pragma HLS UNROLL```, ```#pragma HLS RESOURCE```, and ```#pragma HLS ARRAY_PARTITION``` were used, among others. The tradeoff between optimizing to increase throughput versus optimizing to reduce utilization were kept in consideration. Synthesizability was checked using ```csynth_design``` (C Synthesis) and timing could be checked using ```export_design``` (RTL Export) with the Evaluate Verilog option enabled (though this last step can be time consuming and better left as a final step before FPGA integration in RFNoC).

##### Developing HLS Testbench
A testbench was written in C++ to send input data to the DUT and compare the output against a golden output using ```csim``` (C Simulation) in the C++ domain. The golden output was captured as a binary file using File Sink on the output of the counterpart or reference block in GNU Radio. Input was captured in a similar fashion with the original input source being a live ATSC signal fed from UHD: USRP Source as shown below. In the testbench, multiple function calls to the DUT per test run is encouraged to check the boundaries between returned output data sets and accumulate initiation interval statistics.

![inout](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/inout.png?raw=true)

If ```csim``` showed passing results and ```csynth_design``` showed the block to be synthesizable, then ```cosim``` (C/RTL Cosimulation) was run to translate the C++ code into RTL (Verilog, VHDL, and/or SystemC) and apply the testbench input stimuli and output checking in the RTL domain. To synthesize the input port of the RX Filter block, for example, into the AXI Stream interface used in RFNoC, the ```#pragma HLS INTERFACE axis depth=64 port=in``` was used on the top level function of the block. The ```depth``` parameter has no bearing on synthesis. Instead, it is a control parameter for the ```cosim``` testbench to know how to size its input FIFO so it matches the RX Filter input array size which does have bearing on synthesis.

##### Iterating Between HLS and RFNoC
After a block passed ```cosim``` and met timing in ```export_design``` with the Evaluate Verilog option enabled, it was ready to test against the RFNoC HDL testbench. The C++ source files fed into ```export_design``` got converted into Verilog and Xilinx XCI IP source files. Those output files were used as the DUT in the RFNoC HDL testbench. The binary input and golden output files used in HLS were converted to ASCII representations using MATLAB (Python would have worked as well) which were then used as input and golden output array variables in the SystemVerilog HDL testbench. If the DUT had bugs revealed by the HDL testbench or by running its FPGA implementation in GNU Radio, it was debugged in HLS or HDL testbench, re-packaged with ```export_design```, then retested. This process was repeated until the RFNoC block implementation functioned as desired in GNU Radio.

##### Results
The following table shows current block output rates ("v2" blocks discussed later):

![rates](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/rates.png?raw=true)

RFNoC block utilization area reported by Vivado HLS C Synthesis in terms of units and percentage of block versions verified to function on hardware (none of these are "v2" blocks):

![area](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/area.png?raw=true)

![percentarea](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/percentarea.png?raw=true)

A small oddity was found while testing the RFNoC: ATSC Receiver FPLL block in GNU Radio. All samples were being output with a gain of 3,2767.00480546441 (almost looks like a 15-bit shift left operation) applied to their expected values. This gain did not manifest when running the HLS testbench nor the HDL testbench. The source of this mysterious gain has not yet been found. To resolve this, the FPLL HLS source file was modified to simply divide all outputs by that gain. Of course, the HLS and HDL testbenches had to be updated to multiply that gain back onto the outputs before checking them.

## In Progress
##### Settings Bus
Settings bus implementation is required to make parameters such as the oversampling ratio in RX Filter or delay line length in DC Blocker programmable. Provisions for the settings register bus were implemented in HLS source, HLS testbench, NoC block, HDL testbench, and GNU Radio and UHD integration. It was questionable whether the settings bus ports were getting synthesized correctly from HLS source files. Documentation for synthesizing the settings bus signals from Vivado HLS could not be found. The ```#pragma HLS INTERFACE ap_stable``` and ```#pragma HLS INTERFACE ap_none``` directives were experimented with to attempt synthesis of the ```set_addr```, ```set_data```, and ```set_stb``` settings bus ports from HLS source files. They are data port interface directives, however, they have no associated I/O protocol. Regardless, an FPGA image was made and tested in GNU Radio and the settings bus did not function as desired. Values were not observed to be propagating to the settings registers in Vivado Simulator:

![xsim](https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/blob/master/figures/xsim.png?raw=true)

##### Higher Throughput
As mentioned earlier, target sample rates were not met so there is room for improvement. A "v2" of the RX Filter, DC Blocker, and AGC blocks being worked on are more highly optimized but fail timing so they are not in a state to run on hardware. Reed-Solomon v2 passed all HLS timing and utilization checks but functioned erratically on hardware even when no critical warnings were reported from the build.

Current implementations and more details on the blocks discussed above are in [blocks\_in\_progress].

##### More Blocks
The X310 10 CE limit was reached. Though there are tricks around this (such as what was accomplished by combining blocks at the HLS level) it would be nice if future iterations of RFNoC can support more CEs in a single X310 FPGA image. Then the entire ATSC receiver can be ported into RFNoC! With less hardware-to-software FIFOs, real time playback (the ultimate goal of this concept) would be in closer reach.

## Conclusion
It was realized partway through the project that real time playback was an ambitious stretch goal. Although real time playback was not achieved in this iteration of development, HLS optimizations made it possible for several blocks to meet their respective targets and for all blocks to process data into playable video.

**Tip:** Instantiating many CEs increases the chance of critical warnings on timing or erratic behavior. When testing all blocks, it would be better to evenly distribute CE instances among 2 or 3 images (3 or 4 CEs per image) instead of maximizing the number of CE instances per image.

[grdtv]: <https://github.com/gnuradio/gnuradio/tree/master/gr-dtv/examples>

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
