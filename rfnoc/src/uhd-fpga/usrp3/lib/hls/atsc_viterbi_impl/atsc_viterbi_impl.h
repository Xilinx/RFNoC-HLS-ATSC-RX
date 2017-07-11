/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 * Modified by Andrew Lanez and Sachin Bharadwaj Sundramurthy (Team Rabbit Ears)
 * for RFNoC and Vivado HLS Challenge
 */

#ifndef INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H
#define INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H

#include "atsc_consts.h"
#include "atsc_types.h"
#include "ap_int.h"

#define	USE_SIMPLE_SLICER  0
#define NCODERS 12

#include "atsc_single_viterbi.h"


struct axis_cplx_in {
	float data;
    ap_uint<1> last;
};
struct axis_cplx_out {
	unsigned int data;
    ap_uint<1> last;
};

class atsc_viterbi_decoder_impl
{
private:
    int last_start;
    static const int SEGMENT_SIZE = ATSC_MPEG_RS_ENCODED_LENGTH;	// 207
    static const int OUTPUT_SIZE = (SEGMENT_SIZE * 12);
    static const int INPUT_SIZE = (ATSC_DATA_SEGMENT_LENGTH * 12);

    unsigned char fifo[NCODERS][797];
    int fifo_pointer[NCODERS];

public:
    atsc_viterbi_decoder_impl();
    ~atsc_viterbi_decoder_impl();

    void setup_rpc();

    void reset();

    int work(const atsc_soft_data_segment in[],
             atsc_mpeg_packet_rs_encoded out[]);
};


#endif /* INCLUDED_DTV_ATSC_VITERBI_DECODER_IMPL_H */
