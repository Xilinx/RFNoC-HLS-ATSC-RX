# gr-atsc_rx
Integrating UHD and fpga files into one directory

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


[rfnoc]: <https://github.com/Xilinx/RFNoC-HLS-ATSC-RX/tree/master/rfnoc>

[kb]: <http://kb.ettus.com/Getting_Started_with_RFNoC_Development>
