#
# Copyright 2015-2017 Ettus Research
#
include $(TOOLS_DIR)/make/viv_hls_ip_builder.mak

# Add HLS IP to build
include $(HLS_IP_DIR)/atsc_rx_filter_fpll/Makefile.inc
include $(HLS_IP_DIR)/atsc_depad_impl/Makefile.inc
include $(HLS_IP_DIR)/dc_blocker_agc/Makefile.inc
include $(HLS_IP_DIR)/atsc_viterbi_impl/Makefile.inc
include $(HLS_IP_DIR)/atsc_deinterleaver_impl/Makefile.inc
include $(HLS_IP_DIR)/atsc_rsdecoder_impl/Makefile.inc

hls_ip: $(HLS_IP_BUILD_TARGETS)

.PHONY: hls_ip
