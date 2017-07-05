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

#include "atsc_deinterleaver_impl.h"

unsigned char alignment_fifo[156] = {0};
int alignment_fifo_pointer = 0;

int m_commutator = 0;
int m_fifo_pointer[51] = {0};
unsigned char fifo[51][204] = {0};


unsigned char transform(unsigned char input)
{
    unsigned char retval;
    if(m_commutator==51)
    {
    	retval = input;
    }
    else
    {
        retval = fifo[m_commutator][m_fifo_pointer[m_commutator]];
        fifo[m_commutator][m_fifo_pointer[m_commutator]] = input;
        m_fifo_pointer[m_commutator]++;
        if(m_fifo_pointer[m_commutator]>=((204-4*m_commutator)))
//        if(m_fifo_pointer[m_commutator]==(16))
            m_fifo_pointer[m_commutator]=0;
    }
    m_commutator++;
    if (m_commutator == 52)
        m_commutator = 0;
    return retval;
}


void work(const atsc_mpeg_packet_rs_encoded &in1,
                                   atsc_mpeg_packet_rs_encoded &out1)
{

    // reset commutator if required using INPUT pipeline info
    if (in1.pli._flags & 0x0008)
    {
        m_commutator = 0;
    }


    // now do the actual deinterleaving
    for (unsigned int j = 0; j < 207; j++)
    {
        out1.data[j] = alignment_fifo[alignment_fifo_pointer];
        alignment_fifo[alignment_fifo_pointer] = transform(in1.data[j]);
        alignment_fifo_pointer++;
        if(alignment_fifo_pointer==156)
            alignment_fifo_pointer=0;
    }
}
void atsc_deinterleaver_impl(axis_cplx in[64], axis_cplx out[64])
{
    ap_uint<1> temp_last;
	unsigned int  temp;
	int count=0;
	atsc_mpeg_packet_rs_encoded in1;
	atsc_mpeg_packet_rs_encoded out1;
    // Remove ap ctrl ports (ap_start, ap_ready, ap_idle, etc) since we only use the AXI-Stream ports
    #pragma HLS INTERFACE ap_ctrl_none port=return
    // Set ports as AXI-Stream
    #pragma HLS INTERFACE axis register depth=64 port=in
    #pragma HLS INTERFACE axis register depth=64 port=out

	temp = in[0].data;
	temp_last = in[0].last;
    in1.pli._flags = (temp&0xFFFF);
    in1.pli._segno = ((temp>>16)&0xFFFF);

    for(int i = 1; i<52; i++)
    {
#pragma HLS PIPELINE
    	temp = in[i].data;
    	temp_last = in[i].last;
        in1.data[4*i-4]=(temp>>0)&0xFF;
        in1.data[4*i-3]=(temp>>8)&0xFF;
        in1.data[4*i-2]=(temp>>16)&0xFF;
        in1.data[4*i-1]=(temp>>24)&0xFF;
    }
    temp = in[52].data;
	temp_last = in[52].last;
    in1.data[204]=(temp>>0)&0xFF;
    in1.data[205]=(temp>>8)&0xFF;
    in1.data[206]=(temp>>16)&0xFF;

    for(int i=53;i<64;i++)
    {
#pragma HLS PIPELINE
        temp = in[i].data;
    	temp_last = in[i].last;
    }

    work(in1,out1);

    // remap OUTPUT pipeline info to reflect 52 data segment end-to-end delay
    int	s = in1.pli._segno;
    if (in1.pli._flags & 0x0010)
        s += 312;

    s -= 52;
    if (s < 0)
        s += 624;

    if (s < 312)
    {
    	//out1.set_regular_seg (false, s);				// field 1
        out1.pli._segno = s;
        out1.pli._flags = 0x0001;
        if (s == 0)
        	out1.pli._flags |= 0x0008;
        if (s >= 312)
        	out1.pli._flags |= 0x0020;
    }
    else
    {
    	//out1.set_regular_seg (true, s - ATSC_DSEGS_PER_FIELD);	// field 2
    	out1.pli._segno = (s - 312);
    	out1.pli._flags = 0x0001;
        if (s == 312)
        	out1.pli._flags |= 0x0008;
        if (s >= 624)
        	out1.pli._flags |= 0x0020;
        out1.pli._flags |= 0x0010;

    }

    // For 0
    temp = (unsigned int)((((int)out1.pli._segno)<<16)|((unsigned int)out1.pli._flags));
    out[count].data = temp;
    out[count].last = 0;
    count++;

    // For 1 to 51
    for(int i = 0; i<51; i++)
    {
#pragma HLS PIPELINE
    	temp = (((unsigned int )out1.data[4*i])<<0)|
    			(((unsigned int )out1.data[4*i+1])<<8) |
				(((unsigned int )out1.data[4*i+2])<<16) |
				(((unsigned int )out1.data[4*i+3])<<24);
        out[count].data=temp;
        out[count].last = 0;
        count++;
    }

    // For 52
	temp = (((unsigned int )out1.data[204])<<0)|
			(((unsigned int )out1.data[205])<<8) |
			(((unsigned int )out1.data[206])<<16);
    out[count].data=temp;
    out[count].last = 0;
    count++;

    // For 53 to 63
    for(int i = 53; i<63; i++)
    {
#pragma HLS PIPELINE
        out[count].data=(unsigned int)0;
        out[count].last = 0;
        count++;
    }

    // For 64
    out[count].data=(unsigned int)0;
    out[count].last = 1;
    count++;
    count=0;
}
