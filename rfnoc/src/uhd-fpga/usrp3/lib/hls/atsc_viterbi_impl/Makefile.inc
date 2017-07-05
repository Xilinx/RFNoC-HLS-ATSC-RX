#
# Copyright 2015-2017 Ettus Research
#

# Set IP name
HLS_IP_NAME = atsc_viterbi_impl

# Add C/C++/tcl files relative to usrp3/lib/hls/<ip> directory
HLS_IP_$(HLS_IP_NAME)_SRCS = \
$(HLS_IP_NAME).cpp \
$(HLS_IP_NAME).h \
atsc_single_viterbi.cpp \
atsc_single_viterbi.h \
atsc_viterbi_mux.h \
atsc_consts.h \
atsc_types.h \
$(HLS_IP_NAME).tcl

# Setup build targets, don't remove this
$(eval $(call HLS_IP_GEN_TARGETS,$(HLS_IP_NAME),$(PART_ID),$(HLS_IP_$(HLS_IP_NAME)_SRCS),$(HLS_IP_DIR),$(IP_BUILD_DIR)))
