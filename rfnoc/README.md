### Connect With RFNoC: FIFO Blocks

RFNoC: FIFO blocks can be updated with more data type options to connect with ```atsc_rx``` blocks in GNU Radio Companion. Replace [uhd\_rfnoc\_fifo.xml] into ```{USER_PREFIX}/src/gr-ettus/grc``` then:
```sh
$ cd {USER_PREFIX}/src/gr-ettus/build
$ make
$ sudo make install
```


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

