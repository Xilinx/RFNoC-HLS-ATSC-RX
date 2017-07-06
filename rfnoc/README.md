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

