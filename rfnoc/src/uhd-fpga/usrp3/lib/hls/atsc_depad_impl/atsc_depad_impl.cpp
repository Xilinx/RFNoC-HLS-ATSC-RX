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

#include "atsc_depad_impl.h"

void atsc_depad_impl(axis_cplx_in in[INPUT_SIZE], axis_cplx_out out[OUTPUT_SIZE])
{
	axis_cplx_out temp;
    // Remove ap ctrl ports (ap_start, ap_ready, ap_idle, etc) since we only use the AXI-Stream ports
    #pragma HLS INTERFACE ap_ctrl_none port=return
    // Set ports as AXI-Stream

#pragma HLS INTERFACE axis register port=in
#pragma HLS INTERFACE axis register port=out

    // Need to pack input into a 32-bit word
    // Otherwise, compiler complains that our AXI-Stream interfaces have two data fields (i.e. data.real, data.imag)
		for(int i=0;i<OUTPUT_SIZE-1;i++)
		{
#pragma HLS PIPELINE
//    	temp.data=in.arr_data.data;
			out[i].arr_data = in[i].arr_data;
			temp.last = in[i].last;
			out[i].last = 1;
		}
		temp.last = in[OUTPUT_SIZE-1].last;
		out[OUTPUT_SIZE-1].arr_data = in[OUTPUT_SIZE-1].arr_data;
		out[OUTPUT_SIZE-1].last = 1;
		for(int i=OUTPUT_SIZE;i<INPUT_SIZE;i++)
		{
#pragma HLS PIPELINE
			//temp.arr_data.data = in[i].arr_data.data;
			temp.last = in[i].last;
		}
}

