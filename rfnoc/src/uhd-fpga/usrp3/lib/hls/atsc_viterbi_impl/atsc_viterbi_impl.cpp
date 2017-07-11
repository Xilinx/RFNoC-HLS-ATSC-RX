#include "atsc_viterbi_impl.h"
#include "atsc_types.h"
#include "atsc_viterbi_mux.h"


atsc_viterbi_decoder_impl::atsc_viterbi_decoder_impl()
{
//      set_output_multiple(NCODERS);

    /*
     * These fifo's handle the alignment problem caused by the
     * inherent decoding delay of the individual viterbi decoders.
     * The net result is that this entire block has a pipeline latency
     * of 12 complete segments.
     *
     * If anybody cares, it is possible to do it with less delay, but
     * this approach is at least somewhat understandable...
     *
     * Modified by Andrew Lanez and Sachin Bharadwaj Sundramurthy (Team Rabbit Ears)
     * for RFNoC and Vivado HLS Challenge
     */

    // the -4 is for the 4 sync symbols
    // FIFO is 797
    reset();
}

atsc_viterbi_decoder_impl::~atsc_viterbi_decoder_impl()
{
}

void
atsc_viterbi_decoder_impl::reset()
{
    for (int i = 0; i < NCODERS; i++)
    {
        fifo_pointer[i]=0;
        for(int j=0; j<797; j++)
            fifo[i][j] = 0;
    }
}

int
atsc_viterbi_decoder_impl::work(const atsc_soft_data_segment in[],
                                atsc_mpeg_packet_rs_encoded out[])
{

    // The way the fs_checker works ensures we start getting packets
    // starting with a field sync, and out input multiple is set to
    // 12, so we should always get a mod 12 numbered first packet

    int dbwhere;
    int dbindex;
    int shift;
    float symbols[NCODERS][enco_which_max];
    unsigned char dibits[NCODERS][enco_which_max];

    unsigned char out_copy[OUTPUT_SIZE];

    for (int i = 0; i < 12; i += NCODERS)
    {
        /* Build a continuous symbol buffer for each encoder */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++)
        {
        	for (unsigned int k = 0; k < enco_which_max; k++)
            {
#pragma HLS PIPELINE
            	symbols[encoder][k] = in[i + (enco_which_syms[encoder][k]/832)].data[enco_which_syms[encoder][k] % 832];
            }
        }

        /* Now run each of the 12 Viterbi decoders over their subset of
           the input symbols */
        for (unsigned int k = 0; k < enco_which_max; k++)
        {
		#pragma PIPELINE
			dibits[0][k] = decode1(symbols[0][k]);
			dibits[1][k] = decode2(symbols[1][k]);
			dibits[2][k] = decode3(symbols[2][k]);
			dibits[3][k] = decode4(symbols[3][k]);
			dibits[4][k] = decode5(symbols[4][k]);
			dibits[5][k] = decode6(symbols[5][k]);
			dibits[6][k] = decode7(symbols[6][k]);
			dibits[7][k] = decode8(symbols[7][k]);
			dibits[8][k] = decode9(symbols[8][k]);
			dibits[9][k] = decode10(symbols[9][k]);
			dibits[10][k] = decode11(symbols[10][k]);
			dibits[11][k] = decode12(symbols[11][k]);
        }

        /* Move dibits into their location in the output buffer */
        for (unsigned int encoder = 0; encoder < NCODERS; encoder++)
        {
            for (unsigned int k = 0; k < enco_which_max; k++)
            {
                /* Store the dibit into the output data segment */
                dbwhere = enco_which_dibits[encoder][k];
                dbindex = dbwhere >> 3;
                shift = dbwhere & 0x7;
                out_copy[dbindex] = (out_copy[dbindex] & ~(0x03 << shift)) | (fifo[encoder][fifo_pointer[encoder]] << shift);
                fifo[encoder][fifo_pointer[encoder]] = dibits[encoder][k];
                fifo_pointer[encoder]++;
                if(fifo_pointer[encoder]==797)
                    fifo_pointer[encoder]=0;
            } /* Symbols fed into one encoder */
        } /* Encoders */

        // copy output from contiguous temp buffer into final output
        for (int j = 0; j < NCODERS; j++)
        {
            for(int k=0;k<ATSC_MPEG_RS_ENCODED_LENGTH;k++)
            {
#pragma HLS PIPELINE
            	out[i + j].data[k] = out_copy[j * OUTPUT_SIZE/NCODERS +k];
            }

            // adjust pipeline info to reflect 12 segment delay
            plinfo::delay(out[i + j].pli, in[i + j].pli, NCODERS);
        }
    }

    return 12;
}
atsc_soft_data_segment viterbi_in[12];
atsc_mpeg_packet_rs_encoded viterbi_out[12];

atsc_viterbi_decoder_impl viterbi_decoder_impl;

void atsc_viterbi_impl(axis_cplx_in in[12*1024], axis_cplx_out out[12*64])
{
	union
	{
		unsigned int integer_value;
		float float_value;
	} converter;

	int count=0;

	unsigned int temp2;
	float temp1;
	ap_uint<1> temp_last;
    // Remove ap ctrl ports (ap_start, ap_ready, ap_idle, etc) since we only use the AXI-Stream ports
    #pragma HLS INTERFACE ap_ctrl_none port=return
    // Set ports as AXI-Stream
    #pragma HLS INTERFACE axis register depth=12288 port=in
    #pragma HLS INTERFACE axis register depth=768 port=out


	for(int i=0;i<12;i++)
	{
		converter.float_value = in[1024*i].data;
    	viterbi_in[i].pli._flags = (converter.integer_value)&0xffff ;
    	viterbi_in[i].pli._segno = (converter.integer_value)>>16 ;
		temp_last = in[1024*i].last;
    	for(int j=1;j<833;j++)
    	{
#pragma HLS PIPELINE
    		viterbi_in[i].data[j-1] = in[1024*i+j].data;
    		temp_last = in[1024*i+j].last;
    	}
    	for(int j=833;j<1024;j++)
    	{
#pragma HLS PIPELINE
    		temp1 = in[1024*i+j].data;
    		temp_last = in[1024*i+j].last;
    	}
	}

	viterbi_decoder_impl.work(&viterbi_in[0],&viterbi_out[0]);
	for(int i=0;i<12;i++)
	{
	    // For 0
	    temp2 = (unsigned int)((((int)viterbi_out[i].pli._segno)<<16)|((unsigned int)viterbi_out[i].pli._flags));
	    out[count].data = temp2;
	    out[count].last = 0;
	    count++;



	    // For 1 to 51
	    for(int j = 0; j<51; j++)
	    {
#pragma HLS PIPELINE
	    	temp2 = (((unsigned int )viterbi_out[i].data[4*j])<<0)|
	    			(((unsigned int )viterbi_out[i].data[4*j+1])<<8) |
					(((unsigned int )viterbi_out[i].data[4*j+2])<<16) |
					(((unsigned int )viterbi_out[i].data[4*j+3])<<24);
	        out[count].data=temp2;
	        out[count].last = 0;
	        count++;
	    }

	    // For 52
		temp2 = (((unsigned int )viterbi_out[i].data[204])<<0)|
				(((unsigned int )viterbi_out[i].data[205])<<8) |
				(((unsigned int )viterbi_out[i].data[206])<<16);
	    out[count].data=temp2;
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
	}

}
