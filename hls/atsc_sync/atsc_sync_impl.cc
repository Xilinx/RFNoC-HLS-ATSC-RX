/* -*- c++ -*- */
/*
 * Copyright 2014,2016 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_sync_impl.h"
#include "atsc_types.h"
//#include <gnuradio/io_signature.h>
#include <stdio.h>

// ATSC specific constants
static const double ATSC_SYMBOL_RATE        = 4.5e6/286*684;        // ~10.76 MHz
static const int ATSC_DATA_SEGMENT_LENGTH   = 832;

static unsigned long long dnext_input = 0;
static double drx_clock_to_symbol_freq = RATE/ATSC_SYMBOL_RATE;
static int dsi = 0;
static double dw = drx_clock_to_symbol_freq;
static double dmu = 0.5;
static int dincr = 0;
static double dtiming_adjust = 0;
static int dcounter = 0;
static int dsymbol_index = 0;
static bool dseg_locked = false;
static unsigned char dsr = 0;
static int doutput_produced = 0;
static float dsample_mem[ATSC_DATA_SEGMENT_LENGTH] = {0};
static float ddata_mem[ATSC_DATA_SEGMENT_LENGTH] = {0};
static signed char dintegrator[ATSC_DATA_SEGMENT_LENGTH]
 = {-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,
	-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16,-16};

void atsc_sync(float *in, float *out)
{
#pragma HLS INTERFACE axis depth=4096 port=in
#pragma HLS INTERFACE axis depth=4096 port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

	float btl_in[COUNT] = {0};
	float btl_out[COUNT] = {0};

	for(int i = 0; i < COUNT; i++)
		btl_in[i] = *in++;

	gr::dtv::atsc_sync_impl btl = gr::dtv::atsc_sync_impl(RATE);
//	btl.set_mem();
	btl.general_work(btl_in, btl_out);
//	btl.store_mem();

	for(int i = 0; i < COUNT; i++)
		*out++ = btl_out[i];
}

namespace gr {
  namespace dtv {

    static const double LOOP_FILTER_TAP = 0.0005;	// 0.0005 works
    static const double ADJUSTMENT_GAIN = 1.0e-5 / (10 * ATSC_DATA_SEGMENT_LENGTH);
    static const int	SYMBOL_INDEX_OFFSET = 3;
    static const int	MIN_SEG_LOCK_CORRELATION_VALUE = 5;
    static const char   SSI_MIN = -16;
    static const char   SSI_MAX =  15;

    void
	atsc_sync_impl::set_mem()
    {
//    	d_next_input = 0;
//    	d_rx_clock_to_symbol_freq = RATE/ATSC_SYMBOL_RATE;
//    	d_si = 0;

//        d_w = d_rx_clock_to_symbol_freq;
        d_mu = 0.5;//

        d_timing_adjust = 0;//
//     	//d_counter = 0;
//        d_symbol_index = 0;
        d_seg_locked = false;//

//        d_sr = 0;
//
//        memset(d_sample_mem, 0,       ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_sample_mem));  // (float)0 = 0x00000000
        memset(d_data_mem,   0,       ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_data_mem));    // (float)0 = 0x00000000
        memset(d_integrator, SSI_MIN, ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_integrator));  // signed char

//    	d_incr = dincr;
//    	d_output_produced = doutput_produced;

//    	d_next_input = dnext_input;
//    	d_rx_clock_to_symbol_freq = drx_clock_to_symbol_freq;
//    	d_si = dsi;


//    	d_w = dw;
//    	d_mu = dmu;
//    	d_timing_adjust = dtiming_adjust;
//    	d_counter = dcounter;
//    	d_symbol_index = dsymbol_index;
//    	d_seg_locked = dseg_locked;
//    	d_sr = dsr;
//    	memcpy(d_sample_mem, dsample_mem, ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_sample_mem));  // (float)0 = 0x00000000
//    	memcpy(d_data_mem,   ddata_mem,   ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_data_mem));    // (float)0 = 0x00000000
//     	memcpy(d_integrator, dintegrator, ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_integrator));  // signed char
    }

    void
	atsc_sync_impl::store_mem()
    {
    	dnext_input = d_next_input;
		drx_clock_to_symbol_freq = d_rx_clock_to_symbol_freq;
		dsi = d_si;
		dw = d_w;
		dmu = d_mu;
		dincr = d_incr;

		dtiming_adjust = d_timing_adjust;
    	dcounter = d_counter;
    	dsymbol_index = d_symbol_index;
    	dseg_locked = d_seg_locked;
    	dsr = d_sr;
		doutput_produced = d_output_produced;

		memcpy(dsample_mem, d_sample_mem, ATSC_DATA_SEGMENT_LENGTH * sizeof(*dsample_mem));  // (float)0 = 0x00000000
		memcpy(ddata_mem,   d_data_mem,   ATSC_DATA_SEGMENT_LENGTH * sizeof(*ddata_mem));    // (float)0 = 0x00000000
		memcpy(dintegrator, d_integrator, ATSC_DATA_SEGMENT_LENGTH * sizeof(*dintegrator));  // signed char
    }

//    atsc_sync::sptr
//    atsc_sync::make(float rate)
//    {
//      return gnuradio::get_initial_sptr
//	(new atsc_sync_impl(rate));
//    }

    atsc_sync_impl::atsc_sync_impl(float rate)
//      : gr::block("dtv_atsc_sync",
//                  io_signature::make(1, 1, sizeof(float)),
//                  io_signature::make(1, 1, sizeof(atsc_soft_data_segment))),
		: d_next_input(0), d_rx_clock_to_symbol_freq(rate/ATSC_SYMBOL_RATE), d_si(0)
    {
      d_loop.set_taps(LOOP_FILTER_TAP);
      reset();
    }

    void
    atsc_sync_impl::reset()
    {
      d_w = d_rx_clock_to_symbol_freq;
      d_mu = 0.5;

      d_timing_adjust = 0;
      d_counter = 0;
      d_symbol_index = 0;
      d_seg_locked = false;

      d_sr = 0;

      memset(d_sample_mem, 0,       ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_sample_mem));  // (float)0 = 0x00000000
      memset(d_data_mem,   0,       ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_data_mem));    // (float)0 = 0x00000000
      memset(d_integrator, SSI_MIN, ATSC_DATA_SEGMENT_LENGTH * sizeof(*d_integrator));  // signed char
    }

    atsc_sync_impl::~atsc_sync_impl()
    {
    }

//    void
//    atsc_sync_impl::forecast(int noutput_items,
//                             gr_vector_int &ninput_items_required)
//    {
//      unsigned ninputs = ninput_items_required.size();
//      for (unsigned i = 0; i < ninputs; i++)
//        ninput_items_required[i] = static_cast<int>(noutput_items * d_rx_clock_to_symbol_freq * ATSC_DATA_SEGMENT_LENGTH) + 1500 - 1;
//    }

//    int
//    atsc_sync_impl::general_work(int noutput_items,
//                                 gr_vector_int &ninput_items,
//                                 gr_vector_const_void_star &input_items,
//                                 gr_vector_void_star &output_items)    void
	void
    atsc_sync_impl::general_work(float *in,
                                 float *out)
    {
//	const float *in = (const float *) input_items[0];
//	atsc_soft_data_segment *soft_data_segment_out = (atsc_soft_data_segment *) output_items[0];
	atsc_soft_data_segment *soft_data_segment_out = (atsc_soft_data_segment *) out;
	float interp_sample;

	// amount actually consumed
	d_si = 0;

//	for (d_output_produced = 0; d_output_produced < noutput_items && (d_si + (int)d_interp.ntaps()) < ninput_items[0];) {
	for (d_output_produced = 0; d_output_produced < COUNT && (d_si + (int)d_interp.ntaps()) < COUNT;) {
	    // First we interpolate a sample from input to work with
          interp_sample = d_interp.interpolate(&in[d_si], d_mu);
//printf("## d_output_produced=%d, d_si=%d, ntaps=%d\n",d_output_produced,d_si,(int)d_interp.ntaps());

          // Apply our timing adjustment slowly over several samples
          d_mu += ADJUSTMENT_GAIN * 1e3 * d_timing_adjust;

          double s = d_mu + d_w;
          double float_incr = floor(s);
          d_mu = s - float_incr;
          d_incr = (int)float_incr;

          assert (d_incr >= 1 && d_incr <= 3);
          d_si += d_incr;

          // Remember the sample at this count position
          d_sample_mem[d_counter] = interp_sample;

          // Is the sample positive or negative?
          int bit = (interp_sample < 0 ? 0 : 1);

          // Put the sign bit into our shift register
          d_sr = ((bit & 1) << 3) | (d_sr >> 1);

          // When +,-,-,+ (0x9, 1001) samples show up we have likely found a segment
          // sync, it is more likely the segment sync will show up at about the same
          // spot every ATSC_DATA_SEGMENT_LENGTH samples so we add some weight
          // to this spot every pass to prevent random +,-,-,+ symbols from
          // confusing our synchronizer
          d_integrator[d_counter] += ((d_sr == 0x9) ? +2 : -1);
          if(d_integrator[d_counter] < SSI_MIN) d_integrator[d_counter] = SSI_MIN;
          if(d_integrator[d_counter] > SSI_MAX) d_integrator[d_counter] = SSI_MAX;

          d_symbol_index++;
          if( d_symbol_index >= ATSC_DATA_SEGMENT_LENGTH )
            d_symbol_index = 0;

          d_counter++;
          if( d_counter >= ATSC_DATA_SEGMENT_LENGTH ) {	// counter just wrapped...
            int best_correlation_value = d_integrator[0];
            int best_correlation_index = 0;

            for(int i = 1; i < ATSC_DATA_SEGMENT_LENGTH; i++)
//{printf("## d_integrator[i]:%d > best_correlation_value:%d\n",d_integrator[i], best_correlation_value);
            	if (d_integrator[i] > best_correlation_value) {
                best_correlation_value = d_integrator[i];
                best_correlation_index = i;
              }
//}
            d_seg_locked = best_correlation_value >= MIN_SEG_LOCK_CORRELATION_VALUE;

            // the coefficients are -1,-1,+1,+1
            //d_timing_adjust = d_sample_mem[best_correlation_index - 3] +
            //                   d_sample_mem[best_correlation_index - 2] -
            //                   d_sample_mem[best_correlation_index - 1] -
            //                   d_sample_mem[best_correlation_index];

            //printf( "d_timing_adjust = %f\n", d_timing_adjust );

            int corr_count = best_correlation_index;

            d_timing_adjust = -d_sample_mem[corr_count--];
            if( corr_count < 0 ) corr_count = ATSC_DATA_SEGMENT_LENGTH - 1;
            d_timing_adjust -= d_sample_mem[corr_count--];
            if( corr_count < 0 ) corr_count = ATSC_DATA_SEGMENT_LENGTH - 1;
            d_timing_adjust += d_sample_mem[corr_count--];
            if( corr_count < 0 ) corr_count = ATSC_DATA_SEGMENT_LENGTH - 1;
            d_timing_adjust += d_sample_mem[corr_count--];

            d_symbol_index = SYMBOL_INDEX_OFFSET - 1 - best_correlation_index;
            if (d_symbol_index < 0)
              d_symbol_index += ATSC_DATA_SEGMENT_LENGTH;

            d_counter = 0;
          }

          // If we are locked we can start filling and producing data packets
          // Due to the way we lock the first data packet will almost always be
          // half full, this is OK becouse the fs_checker will not let packets though
          // untill a non-corrupted field packet is found
          if( d_seg_locked ) {
            d_data_mem[d_symbol_index] = interp_sample;

            if( d_symbol_index >= (ATSC_DATA_SEGMENT_LENGTH - 1) )
              {
                for( int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++ )
                  soft_data_segment_out[d_output_produced].data[i] = d_data_mem[i];
                d_output_produced++;
              }
          }
	}

//	consume_each(d_si);
//	return d_output_produced;

    }

  } /* namespace dtv */
} /* namespace gr */
