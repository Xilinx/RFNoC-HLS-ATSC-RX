/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#include "agc.h"

void agc(axis_float &in, axis_float &out, float set_data, unsigned char set_addr, bool set_stb)
{
#pragma HLS INTERFACE depth=1 axis port=in
#pragma HLS INTERFACE depth=1 axis port=out
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE ap_stable port=set_data
#pragma HLS INTERFACE ap_stable port=set_addr
#pragma HLS INTERFACE ap_stable port=set_stb

	static float rate;
	static float reference;
	static float gain;
	static float max_gain;
	static bool flag128, flag129, flag130, flag131;
	if (set_stb) {
		if (set_addr == 128) {
			rate = set_data;
			flag128 = true;
		}
		if (set_addr == 129) {
			reference = set_data;
			flag128 = true;
		}
		if (set_addr == 130) {
			gain = set_data;
			flag128 = true;
		}
		if (set_addr == 131) {
			max_gain = set_data;
			flag128 = true;
		}
	}

	else {
		// Set default values if none specified by user
		if (!flag128) rate = 0.00001;
		if (!flag129) reference = 4.0;
		if (!flag130) gain = 1.0;
		if (!flag131) max_gain = 65536;

		agc_ff agc = agc_ff(rate, reference, gain, max_gain);
		out.data = agc.scale(in.data);
		out.last = in.last;
	}
}
