#
# Copyright 2015-2017 Ettus Research
#
include $(TOOLS_DIR)/make/viv_hls_ip_builder.mak

# Add HLS IP to build
include $(HLS_IP_DIR)/atsc_rx_filter/Makefile.inc
include $(HLS_IP_DIR)/atsc_fpll/Makefile.inc
include $(HLS_IP_DIR)/dc_blocker/Makefile.inc
include $(HLS_IP_DIR)/agc/Makefile.inc

hls_ip: $(HLS_IP_BUILD_TARGETS)

.PHONY: hls_ip
