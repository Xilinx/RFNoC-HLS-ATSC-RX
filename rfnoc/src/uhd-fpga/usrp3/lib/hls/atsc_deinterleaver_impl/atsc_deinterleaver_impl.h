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

#ifndef INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H
#define INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H

#include "ap_int.h"
#include <cassert>

static const int ATSC_DSEGS_PER_FIELD		= 312;	// regular data segs / field
static const int ATSC_MPEG_RS_ENCODED_LENGTH 	= 207;
static const int NPAD = 45;


struct plinfo
{

    int	_flags;		// bitmask
    int	_segno;		// segment number [-1,311] -1 is the field sync segment

};


struct atsc_mpeg_packet_rs_encoded
{
    plinfo	pli;
    unsigned char	data[ATSC_MPEG_RS_ENCODED_LENGTH];
    unsigned char _pad_[NPAD];				// pad to power of 2 (256)
};



struct axis_cplx {
	unsigned int data;
    ap_uint<1> last;
};

#endif /* INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H */
