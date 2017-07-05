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

#include "atsc_sync.h"

static const double LOOP_FILTER_TAP = 0.0005;			// 0.0005 works
static const double ADJUSTMENT_GAIN = 1.0e-5 / (10 * ATSC_DATA_SEGMENT_LENGTH);
static const int	SYMBOL_INDEX_OFFSET = 3;
static const int	MIN_SEG_LOCK_CORRELATION_VALUE = 5;
static const char   SSI_MIN = -16;
static const char   SSI_MAX =  15;

void atsc_sync(float *in, float *out)
{
#pragma HLS INTERFACE axis depth=4096 port=in
#pragma HLS INTERFACE axis depth=4096 port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

	float btl_in[COUNT] = {0};
	float btl_out[COUNT] = {0};

	for(int i = 0; i < COUNT; i++)
		btl_in[i] = *in++;

//	static
	atsc_sync::atsc_sync_impl* btl;

	btl->general_work(btl_in, btl_out);

	for(int i = 0; i < COUNT; i++)
		*out++ = btl_out[i];
}

//    atsc_sync::sptr
//    atsc_sync::make(float RATE)
//    {
//      return gnuradio::get_initial_sptr
//	(new atsc_sync_impl(RATE));
//    }

    atsc_sync::atsc_sync_impl::atsc_sync_impl()
		: d_next_input(0), d_rx_clock_to_symbol_freq(RATE/ATSC_SYMBOL_RATE),
		  d_si(0)
    {
      d_loop.set_taps(LOOP_FILTER_TAP);
      reset();
    }

    void
	atsc_sync::atsc_sync_impl::reset()
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

    atsc_sync::atsc_sync_impl::~atsc_sync_impl()
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

    void
	atsc_sync::atsc_sync_impl::general_work(float *in,
                                 float *out)
    {
//	atsc_soft_data_segment *soft_data_segment_out = (atsc_soft_data_segment *) output_items[0];
	atsc_soft_data_segment *soft_data_segment_out = (atsc_soft_data_segment *) out;

	float interp_sample;

	// amount actually consumed
	d_si = 0;

//	for (d_output_produced = 0; d_output_produced < noutput_items && (d_si + (int)d_interp.ntaps()) < ninput_items[0];) {
	for (d_output_produced = 0; d_output_produced < COUNT && (d_si + (int)d_interp.ntaps()) < COUNT;) {
          // First we interpolate a sample from input to work with
          interp_sample = d_interp.interpolate(&in[d_si], d_mu);

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
              if (d_integrator[i] > best_correlation_value) {
                best_correlation_value = d_integrator[i];
                best_correlation_index = i;
              }

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

        atsc_sync::fir_filter_fff::fir_filter_fff(int decimation,
    				     const std::vector<float> &taps)
        {
//    	d_align = volk_get_alignment();
//    	d_naligned = std::max((size_t)1, d_align / sizeof(float));
//
//    	d_aligned_taps = NULL;
    	set_taps(taps);

//    	// Make sure the output sample is always aligned, too.
//    	d_output = (float*)volk_malloc(1*sizeof(float), d_align);
        }

        atsc_sync::fir_filter_fff::~fir_filter_fff()
        {
//    	// Free all aligned taps
//    	if(d_aligned_taps != NULL) {
//    	  for(int i = 0; i < d_naligned; i++) {
//    	    volk_free(d_aligned_taps[i]);
//    	  }
//    	  ::free(d_aligned_taps);
//    	  d_aligned_taps = NULL;
//    	}
//
//    	// Free output sample
//    	volk_free(d_output);
        }

        void
		atsc_sync::fir_filter_fff::set_taps(const std::vector<float> &taps)
        {
//    	// Free the taps if already allocated
//    	if(d_aligned_taps!= NULL) {
//    	  for(int i = 0; i < d_naligned; i++) {
//    	    volk_free(d_aligned_taps[i]);
//    	  }
//    	  ::free(d_aligned_taps);
//    	  d_aligned_taps = NULL;
//    	}

    	d_ntaps = (int)taps.size();
    	d_taps = taps;
    	reverse(d_taps.begin(), d_taps.end());

//    	// Make a set of taps at all possible arch alignments
//    	d_aligned_taps = (float**)malloc(d_naligned*sizeof(float*));
//    	for(int i = 0; i < d_naligned; i++) {
//            d_aligned_taps[i] = (float*)volk_malloc((d_ntaps+d_naligned-1)*sizeof(float), d_align);
//    	  memset(d_aligned_taps[i], 0, sizeof(float)*(d_ntaps+d_naligned-1));
//    	  for(unsigned int j = 0; j < d_ntaps; j++)
//    	    d_aligned_taps[i][i+j] = d_taps[j];
//    	}
        }

        void
		atsc_sync::fir_filter_fff::update_tap(float t, unsigned int index)
        {
    	d_taps[index] = t;
    	for(int i = 0; i < d_naligned; i++) {
    	  d_aligned_taps[i][i+index] = t;
    	}
        }

        std::vector<float>
        atsc_sync::fir_filter_fff::taps() const
        {
    	std::vector<float> t = d_taps;
    	reverse(t.begin(), t.end());
    	return t;
        }

        unsigned int
		atsc_sync::fir_filter_fff::ntaps() const
        {
    	return d_ntaps;
        }

        float
		atsc_sync::fir_filter_fff::filter(const float input[])
        {
    //		const float *ar = (float *)((size_t) input & ~(d_align-1));
    //		unsigned al = input - ar;

    		float dotProduct = 0;
    		unsigned int number = 0;

    		for(number = 0; number < d_ntaps; number++){
    		  dotProduct += ((input[number]) * d_taps[number]);
    		}

    		return dotProduct;

    //	volk_32f_x2_dot_prod_32f_a(d_output, ar,
    //				   d_aligned_taps[al],
    //				   d_ntaps+al);
    //	return *d_output;
        }

        void
		atsc_sync::fir_filter_fff::filterN(float output[],
    			      const float input[],
    			      unsigned long n)
        {
    	for(unsigned long i = 0; i < n; i++) {
    	  output[i] = filter(&input[i]);
    	}
        }

        void
		atsc_sync::fir_filter_fff::filterNdec(float output[],
    				 const float input[],
    				 unsigned long n,
    				 unsigned int decimate)
        {
    	unsigned long j = 0;
    	for(unsigned long i = 0; i < n; i++) {
    	  output[i] = filter(&input[j]);
    	  j += decimate;
    	}
        }

        atsc_sync::mmse_fir_interpolator_ff::mmse_fir_interpolator_ff()
		{
		  filters.resize(NSTEPS + 1);

		  for(int i = 0; i < NSTEPS + 1; i++) {
		std::vector<float> t(&interpolator_taps[i][0], &interpolator_taps[i][NTAPS]);
		filters[i] = new fir_filter_fff(1, t);
		  }
		}

        atsc_sync::mmse_fir_interpolator_ff::~mmse_fir_interpolator_ff()
		{
		  for(int i = 0; i < NSTEPS + 1; i++)
		delete filters[i];
		}

		unsigned
		atsc_sync::mmse_fir_interpolator_ff::ntaps() const
		{
		  return NTAPS;
		}

		unsigned
		atsc_sync::mmse_fir_interpolator_ff::nsteps() const
		{
		  return NSTEPS;
		}

		float
		atsc_sync::mmse_fir_interpolator_ff::interpolate(const float input[],
						  float mu) const
		{
		  int imu = (int)rint(mu * NSTEPS);

//              if((imu < 0) || (imu > NSTEPS)) {
//        	throw std::runtime_error("mmse_fir_interpolator_ff: imu out of bounds.\n");
//              }

		  float r = filters[imu]->filter(input);
		  return r;
		}
