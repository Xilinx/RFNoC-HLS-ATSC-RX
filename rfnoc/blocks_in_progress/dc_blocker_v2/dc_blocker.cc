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
 */

#include "dc_blocker.h"
#include <stdio.h>

void dc_blocker(axis_float in[D], axis_float out[D])
{
#pragma HLS INTERFACE axis depth=128 port=in
#pragma HLS INTERFACE axis depth=128 port=out
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS PIPELINE II=2

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

float
moving_averager_f::filter(float x)
{
	d_out_d1 = d_out;
	d_out = d_delay_line[0];
	for(int i = 0; i < D-2; i++)
	#pragma HLS UNROLL factor=16
		d_delay_line[i] = d_delay_line[i+1];
	d_delay_line[D-2] = x;

	float y = x - d_out_d1 + d_out_d2;
	#pragma HLS RESOURCE variable=y core=FAddSub_fulldsp latency=0
	d_out_d2 = y;

	return (y / (float)(D));
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

		float diff = d_ma_0.delayed_sig() - y2;
		#pragma HLS RESOURCE variable=diff core=FAddSub_fulldsp latency=0
		out[i] = diff;
	}
}
