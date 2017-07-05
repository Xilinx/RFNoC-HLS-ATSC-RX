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

#ifndef INCLUDED_DTV_ATSC_DEPAD_IMPL_H
#define INCLUDED_DTV_ATSC_DEPAD_IMPL_H

#include "ap_int.h"

#define ATSC_MPEG_PKT_LENGTH 188
#define INPUT_SIZE 64
#define OUTPUT_SIZE 47

struct atsc_mpeg_packet_none
{
    public:
    unsigned int	data;
};
struct atsc_mpeg_packet
{
public:
    unsigned int	data;
};
struct axis_cplx_in {
	unsigned int arr_data;
    ap_uint<1> last;
};
struct axis_cplx_out {
	unsigned int arr_data;
    ap_uint<1> last;
};

#endif /* INCLUDED_DTV_ATSC_DEPAD_IMPL_H */
