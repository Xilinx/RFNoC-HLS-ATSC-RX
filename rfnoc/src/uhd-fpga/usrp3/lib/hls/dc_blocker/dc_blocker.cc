/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#include "dc_blocker.h"
#include <stdio.h>

void dc_blocker(axis_float in[D], axis_float out[D])
{
#pragma HLS INTERFACE axis depth=128 port=in
#pragma HLS INTERFACE axis depth=128 port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

	float dcr_in[D] = {0};
	float dcr_out[D] = {0};
	bool last[D];

	for(int i = 0; i < D; i++) {
	#pragma HLS PIPELINE
	#pragma HLS UNROLL
		dcr_in[i] = in[i].data;
		last[i] = in[i].last;
	}

	static dc_blocker_ff_impl dcr;
	#pragma HLS ARRAY_PARTITION variable=dcr.d_ma_0.d_delay_line cyclic factor=16 dim=1
	#pragma HLS ARRAY_PARTITION variable=dcr.d_ma_1.d_delay_line cyclic factor=16 dim=1
	dcr.work(dcr_in, dcr_out);

	for(int i = 0; i < D; i++) {
	#pragma HLS PIPELINE
	#pragma HLS UNROLL
		out[i].data = dcr_out[i];
		out[i].last = last[i];
	}
}

moving_averager_f::moving_averager_f()
  : d_length(D), d_out(0), d_out_d1(0), d_out_d2(0)
{
}

moving_averager_f::~moving_averager_f()
{
}

float
moving_averager_f::filter(float x)
{
	d_out_d1 = d_out;
	d_out = d_delay_line[0];
	for(int i = 0; i < d_length-2; i++)
	#pragma HLS UNROLL factor=16
		d_delay_line[i] = d_delay_line[i+1];
	d_delay_line[d_length-2] = x;

	float y = x - d_out_d1 + d_out_d2;
	d_out_d2 = y;

	return (y / (float)(d_length));
}

dc_blocker_ff_impl::dc_blocker_ff_impl()
	: d_length(D)
{
}

dc_blocker_ff_impl::~dc_blocker_ff_impl()
{
}

int
dc_blocker_ff_impl::group_delay()
{
	return (2*d_length-2);
}

void
dc_blocker_ff_impl::work(float *in,
						 float *out)
{
	float y1, y2;
	for(int i = 0; i < D; i++) {
	#pragma HLS UNROLL factor=16
		y1 = d_ma_0.filter(in[i]);
		y2 = d_ma_1.filter(y1);

		out[i] = d_ma_0.delayed_sig() - y2;
	}
}
