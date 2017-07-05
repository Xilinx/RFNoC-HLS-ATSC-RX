#
# Copyright 2015-2017 Ettus Research
#

# Set IP name
HLS_IP_NAME = atsc_depad_impl

# Add C/C++/tcl files relative to usrp3/lib/hls/<ip> directory
HLS_IP_$(HLS_IP_NAME)_SRCS = \
atsc_depad_impl.cpp \
atsc_depad_impl.h \
atsc_depad_impl.tcl

# Setup build targets, don't remove this
$(eval $(call HLS_IP_GEN_TARGETS,$(HLS_IP_NAME),$(PART_ID),$(HLS_IP_$(HLS_IP_NAME)_SRCS),$(HLS_IP_DIR),$(IP_BUILD_DIR)))
