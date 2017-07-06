/* -*- c++ -*- */

#define ATSC_RX_API
#define ETTUS_API

%include "gnuradio.i"/*			*/// the common stuff

//load generated python docstrings
%include "atsc_rx_swig_doc.i"
//Header from gr-ettus
%include "ettus/device3.h"
%include "ettus/rfnoc_block.h"
%include "ettus/rfnoc_block_impl.h"

%{
#include "ettus/device3.h"
#include "ettus/rfnoc_block_impl.h"
#include "atsc_rx/rxfilt.h"
#include "atsc_rx/fpll.h"
#include "atsc_rx/dcr.h"
#include "atsc_rx/agc.h"
#include "atsc_rx/fltpll.h"
#include "atsc_rx/dcragc.h"
#include "atsc_rx/deint.h"
#include "atsc_rx/depad.h"
#include "atsc_rx/viterbi.h"
#include "atsc_rx/rsdec.h"
#include "atsc_rx/rsdec.h"
%}

%include "atsc_rx/rxfilt.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, rxfilt);
%include "atsc_rx/fpll.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, fpll);
%include "atsc_rx/dcr.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, dcr);
%include "atsc_rx/agc.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, agc);
%include "atsc_rx/fltpll.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, fltpll);
%include "atsc_rx/dcragc.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, dcragc);
%include "atsc_rx/deint.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, deint);
%include "atsc_rx/depad.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, depad);
%include "atsc_rx/viterbi.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, viterbi);
%include "atsc_rx/rsdec.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, rsdec);
%include "atsc_rx/rsdec.h"
GR_SWIG_BLOCK_MAGIC2(atsc_rx, rsdec);
