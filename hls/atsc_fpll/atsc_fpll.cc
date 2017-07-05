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
 */

#include "atsc_fpll.h"

// To undo mysterious gain on FPGA output
static const float FPGA_GAIN = 32767.00480546441;

using namespace std;

void atsc_fpll(axis_float in[2], axis_float &out)
{
#pragma HLS INTERFACE axis depth=2 port=in
#pragma HLS INTERFACE axis depth=1 port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

	complex<float> pll_in;
	float pll_out;

	pll_in.real() = in[0].data;
	bool last0 = in[0].last;
	pll_in.imag() = in[1].data;
	bool last1 = in[1].last;

	static atsc_fpll_impl pll;
	pll.work(pll_in, pll_out);

	out.data = pll_out/FPGA_GAIN;
	out.last = last0 || last1;
}

atsc_fpll_impl::atsc_fpll_impl()
{
	d_afc.set_taps(1.0-exp((float)(-1.0/RATE/5e-6)));
	d_nco.set_freq((-3e6 + 0.309e6)/RATE*2*M_PI);
	d_nco.set_phase(0.0);
}

atsc_fpll_impl::~atsc_fpll_impl()
{
}

void
atsc_fpll_impl::work(std::complex<float> &in,
					 float &out)
{
	float a_cos, a_sin;

	d_nco.step();	// increment phase
	a_sin = d_nco.sin();
	a_cos = d_nco.cos();

	// Mix out carrier and output I-only signal
	complex<float> result = in*complex<float>(a_sin, a_cos);
	out = result.real();

	// Update phase/freq error
	complex<float> filtered = d_afc.filter(result);
	float x = atan2(filtered.imag(), filtered.real());

	// avoid slamming filter with big transitions
	static const float limit = M_PI/2.0;
	#pragma HLS RESOURCE variable=limit core=FDiv latency=0
	if (x > limit)
		x = limit;
	else if (x < -limit)
		x = -limit;

	static const float alpha = 0.01;
	static const float beta = alpha*alpha/4.0;
	d_nco.adjust_phase(alpha*x);
	d_nco.adjust_freq(beta*x);
}
