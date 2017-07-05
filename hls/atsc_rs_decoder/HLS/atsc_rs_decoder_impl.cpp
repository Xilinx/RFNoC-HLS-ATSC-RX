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
 * GNU Radio is distributed rs_in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include <iostream>
#include "atsc_rs_decoder_impl.h"
#include "char.h"

static const int rs_init_symsize =     8;
static const int rs_init_nroots  =    20;

static const int N = (1 << rs_init_symsize) - 1;	// 255
static const int K = N - rs_init_nroots;		// 235

static const int amount_of_pad = N - ATSC_MPEG_RS_ENCODED_LENGTH;	  // 48

unsigned char alpha_to_value[256]={1,2,4,8,16,32,64,128,29,58,116,232,205,135,19,38,76,152,45,90,180,117,234,201,143,3,6,12,24,48,96,192,157,39,78,156,37,74,148,53,106,212,181,119,238,193,159,35,70,140,5,10,20,40,80,160,93,186,105,210,185,111,222,161,95,190,97,194,153,47,94,188,101,202,137,15,30,60,120,240,253,231,211,187,107,214,177,127,254,225,223,163,91,182,113,226,217,175,67,134,17,34,68,136,13,26,52,104,208,189,103,206,129,31,62,124,248,237,199,147,59,118,236,197,151,51,102,204,133,23,46,92,184,109,218,169,79,158,33,66,132,21,42,84,168,77,154,41,82,164,85,170,73,146,57,114,228,213,183,115,230,209,191,99,198,145,63,126,252,229,215,179,123,246,241,255,227,219,171,75,150,49,98,196,149,55,110,220,165,87,174,65,130,25,50,100,200,141,7,14,28,56,112,224,221,167,83,166,81,162,89,178,121,242,249,239,195,155,43,86,172,69,138,9,18,36,72,144,61,122,244,245,247,243,251,235,203,139,11,22,44,88,176,125,250,233,207,131,27,54,108,216,173,71,142,0};
unsigned char index_of_value[256]={255,0,1,25,2,50,26,198,3,223,51,238,27,104,199,75,4,100,224,14,52,141,239,129,28,193,105,248,200,8,76,113,5,138,101,47,225,36,15,33,53,147,142,218,240,18,130,69,29,181,194,125,106,39,249,185,201,154,9,120,77,228,114,166,6,191,139,98,102,221,48,253,226,152,37,179,16,145,34,136,54,208,148,206,143,150,219,189,241,210,19,92,131,56,70,64,30,66,182,163,195,72,126,110,107,58,40,84,250,133,186,61,202,94,155,159,10,21,121,43,78,212,229,172,115,243,167,87,7,112,192,247,140,128,99,13,103,74,222,237,49,197,254,24,227,165,153,119,38,184,180,124,17,68,146,217,35,32,137,46,55,63,209,91,149,188,207,205,144,135,151,178,220,252,190,97,242,86,211,171,20,42,93,158,132,60,57,83,71,109,65,162,31,45,67,216,183,123,164,118,196,23,73,236,127,12,111,246,108,161,59,82,41,157,85,170,251,96,134,177,187,204,62,90,203,89,95,176,156,169,160,81,11,245,22,235,122,117,44,215,79,174,213,233,230,231,173,232,116,214,244,234,168,80,88,175};
unsigned char genpoly_value[21]={190,188,212,212,164,156,239,83,225,221,180,202,187,26,163,61,50,79,60,17,0};
int modnn_table_value[512]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,0,1};

struct rs rs_value = {8,
255,
&alpha_to_value[0],
&index_of_value[0],
&genpoly_value[0],
20,
0,
1,
1,
&modnn_table_value[0]
};

atsc_rs_decoder_impl::atsc_rs_decoder_impl()
{
    d_nerrors_corrrected_count = 0;
    d_bad_packet_count = 0;
    d_total_packets = 0;
    d_total_bits = 0;
}

int atsc_rs_decoder_impl::decode (atsc_mpeg_packet_no_sync &rs_out, const atsc_mpeg_packet_rs_encoded &rs_in)
{
    unsigned char tmp[N];
    int ncorrections;

    assert((int)(amount_of_pad + sizeof(rs_in.data)) == N);

    // add missing prefix zero padding to message
    for(int i=0;i<amount_of_pad;i++)
        tmp[i]=0;
    for(int i=0;i<207;i++)
        tmp[amount_of_pad+i]=rs_in.data[i];

    // correct message...
    ncorrections = decode_rs_char(&rs_value, tmp, 0, 0);

    // copy corrected message to output, skipping prefix zero padding
    for(int i=0;i<187;i++)
        rs_out.data[i]=tmp[amount_of_pad+i];
    return ncorrections;
}

atsc_rs_decoder_impl::~atsc_rs_decoder_impl()
{
}


void atsc_rs_decoder_impl::work(const atsc_mpeg_packet_rs_encoded &rs_in,atsc_mpeg_packet_no_sync &rs_out)
{
    assert(rs_in.pli.regular_seg_p());
    rs_out.pli = rs_in.pli;			// copy pipeline info...

    int nerrors_corrrected = decode(rs_out, rs_in);
    rs_out.pli.set_transport_error(nerrors_corrrected == -1);
    if (nerrors_corrrected == -1)
    {
        d_bad_packet_count++;
        d_nerrors_corrrected_count += 10; // lower bound estimate; most this RS can fix
    }
    else
    {
        d_nerrors_corrrected_count += nerrors_corrrected;
    }
    d_total_packets++;
}


void atsc_rsdecoder_impl(axis_cplx_in in[64], axis_cplx_out out[64])
{
	unsigned int temp;
	ap_uint<1> last_temp;
    static atsc_rs_decoder_impl rs;
	atsc_mpeg_packet_no_sync rs_out;
	atsc_mpeg_packet_rs_encoded rs_in;

    // Remove ap ctrl ports (ap_start, ap_ready, ap_idle, etc) since we only use the AXI-Stream ports
    #pragma HLS INTERFACE ap_ctrl_none port=return
    // Set ports as AXI-Stream
    #pragma HLS INTERFACE axis register depth=64 port=in
    #pragma HLS INTERFACE axis register depth=64 port=out
    // Need to pack input into a 32-bit word
    // Otherwise, compiler complains that our AXI-Stream interfaces have two data fields (i.e. data.real, data.imag)
//    #pragma HLS DATA_PACK variable=rs_in.data
//    #pragma HLS DATA_PACK variable=rs_out.data

	temp = in[0].data;
    last_temp = in[0].last;
    rs_in.pli._flags = (temp&0xFFFF);
    rs_in.pli._segno = ((temp>>16)&0xFFFF);

    for(int i = 1; i<52; i++)
    {
    	temp = in[i].data;
        last_temp = in[i].last;
        rs_in.data[4*i-4]=(temp>>0)&0xFF;
        rs_in.data[4*i-3]=(temp>>8)&0xFF;
        rs_in.data[4*i-2]=(temp>>16)&0xFF;
        rs_in.data[4*i-1]=(temp>>24)&0xFF;
    }

    temp = in[52].data;
    last_temp = in[52].last;
    rs_in.data[204]=(temp>>0)&0xFF;
    rs_in.data[205]=(temp>>8)&0xFF;
    rs_in.data[206]=(temp>>16)&0xFF;

    for(int i = 53; i<64; i++)
    {
    	temp = in[i].data;
    	last_temp = in[i].last;
    }

    rs.work(rs_in,rs_out);

    temp = (unsigned int)((((unsigned int)rs_out.pli._segno)<<16)|((unsigned int)rs_out.pli._flags));
    out[0].data = temp;
    out[0].last = 0;

    for(int i = 1; i<47; i++)
    {
    	temp = (((unsigned int )rs_out.data[4*i-4])<<0)|
    			(((unsigned int )rs_out.data[4*i-3])<<8) |
				(((unsigned int )rs_out.data[4*i-2])<<16) |
				(((unsigned int )rs_out.data[4*i-1])<<24);
        out[i].data=temp;
        out[i].last = 0;
    }

	temp = (((unsigned int )rs_out.data[184])<<0)|
			(((unsigned int )rs_out.data[185])<<8) |
			(((unsigned int )rs_out.data[186])<<16);
    out[47].data=temp;
    out[47].last = 0;

    for(int i = 48; i<63; i++)
    {
        out[i].data=(unsigned int)0;
        out[i].last = 0;
    }
    out[63].data=(unsigned int)0;
    out[63].last = 1;
}


