title: The ATSC_RX OOT Module
brief: RFNoC implementation of ATSC receiver blocks in GNU Radio
tags: # Tags are arbitrary, but look at CGRAN what other authors are using
  - sdr
  - rfnoc
  - atsc
author:
  Andrew Lanez <alanez@eng.ucsd.edu>
  Sachin Bharadwaj Sundramurthy <sabharad@eng.ucsd.edu>
  Alireza Khodamoradi <alirezak@eng.ucsd.edu>
#copyright_owner:
#  - Copyright Owner 1
#license:
repo: https://github.com/Xilinx/RFNoC-HLS-ATSC-RX
#website: <module_website> # If you have a separate project website, put it here
#icon: <icon_url> # Put a URL to a square image here that will be used as an icon on CGRAN
---
The atsc_rx RFNoC module for GNU Radio consists of blocks implemented on FPGA that process an ATSC signal with its final output being a playable video file. These blocks are intended to plug-and-play into GNU Radio's gr-dtv ATSC receiver example.
