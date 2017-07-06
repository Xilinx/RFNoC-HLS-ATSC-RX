title: The ATSC_RX OOT Module
brief: RFNoC implementation of ATSC receiver blocks in GNU Radio
tags:
  - sdr
  - rfnoc
  - atsc
author:
  - Andrew Valenzuela Lanez <andrew.lanez@navy.mil>
  - Sachin Bharadwaj Sundramurthy <sabharad@eng.ucsd.edu>
  - Alireza Khodamoradi <alirezak@eng.ucsd.edu>
copyright_owner:
  - Copyright Owner 1
repo: https://github.com/Xilinx/RFNoC-HLS-ATSC-RX
dependencies:
  - gnuradio 3.7.12
  - Xilinx Vivado Design Suite 2015.4
  - UHD 4.0.0
  - rfnoc
---
This directory contains the ```atsc_rx``` RFNoC module for GNU Radio. Its blocks are to be implemented onto the FPGA of an RFNoC-suppported USRP (e.g., E310/E312 or X300/X310) using RFNoC and Xilinx Vivado Design Suite 2015.4. They can plug-and-play into the GNU Radio [gr-dtv] ATSC receiver example and process an ATSC signal into a playable video file.

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

All blocks above have these features:
- Vivado HLS Source & Testbench
- NoC Block & HDL Testbench
- UHD, GNU Radio, & FPGA Integration



[gr-dtv]: <https://github.com/gnuradio/gnuradio/tree/master/gr-dtv/examples>
