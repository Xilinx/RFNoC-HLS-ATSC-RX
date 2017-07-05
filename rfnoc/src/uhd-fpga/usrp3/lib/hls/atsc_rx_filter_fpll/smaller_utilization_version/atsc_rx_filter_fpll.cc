#include "atsc_rx_filter_fpll.h"
#include <stdio.h>

// Mysterious gain on FPGA output
#define FPGA_GAIN 32767.00480546441

using std::complex;

void atsc_rx_filter_fpll(axis_float in[IN_SIZE], axis_float out[OUT_SIZE])
{
#pragma HLS INTERFACE axis depth=64 port=in
#pragma HLS INTERFACE axis depth=61 port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

	int nitems_read, n_out;
	static float acc_hist = 0, ph = 1.9634954929351806640625;

	complex<float> pfb_in[IN_SIZE/2] = {0};
	complex<float> pfb_out[OUT_SIZE] = {0};
	for(int i=0; i<IN_SIZE; i+=2) {
		pfb_in[i/2].real() = in[i].data;
		pfb_in[i/2].imag() = in[i+1].data;
	}

    static atsc_rx_filter::pfb_arb_resampler_ccf pfb = \
    		atsc_rx_filter::pfb_arb_resampler_ccf(interp, rrc_taps, FILTER_SIZE);

    pfb.set_phase(ph);
	pfb.set_acc(acc_hist);
	n_out = pfb.filter(pfb_out, pfb_in, IN_SIZE/2, nitems_read);
    ph = pfb.phase();
	acc_hist = pfb.acc();

	float fpll_out;
	for(int i=0; i<n_out; i++) {
		atsc_fpll(pfb_out[i], fpll_out);
		out[i].data = fpll_out/FPGA_GAIN;
    	out[i].last = i == (n_out-1);
	}
}

	atsc_rx_filter::pfb_arb_resampler_ccf::pfb_arb_resampler_ccf(float rate,
                                                   const float taps[],
                                                   unsigned int filter_size)
      {
        d_acc = 0; // start accumulator at 0

        /* The number of filters is specified by the user as the
           filter size; this is also the interpolation rate of the
           filter. We use it and the rate provided to determine the
           decimation rate. This acts as a rational resampler. The
           flt_rate is calculated as the residual between the integer
           decimation rate and the real decimation rate and will be
           used to determine to interpolation point of the resampling
           process.
        */
        d_int_rate = filter_size;
        set_rate(rate);

        d_last_filter = (RRC_NTAPS/2) % filter_size;

        // Now, actually set the filters' taps
        set_taps(taps);

        // Delay is based on number of taps per filter arm. Round to
        // the nearest integer.
        float delay = rate * (taps_per_filter() - 1.0) / 2.0;
        d_delay = static_cast<int>(round(delay));//(boost::math::iround(delay));

        // This calculation finds the phase offset induced by the
        // arbitrary resampling. It's based on which filter arm we are
        // at the filter's group delay plus the fractional offset
        // between the samples. Calculated here based on the rotation
        // around nfilts starting at start_filter.
        float accum = d_delay * d_flt_rate;
        int   accum_int = static_cast<int>(accum);
        float accum_frac = accum - accum_int;
        int end_filter = static_cast<int>
    		(round(mod(d_last_filter + d_delay * d_dec_rate + accum_int, \
                               static_cast<float>(d_int_rate))));

        d_est_phase_change = d_last_filter - (end_filter + accum_frac);
      }

	atsc_rx_filter::pfb_arb_resampler_ccf::~pfb_arb_resampler_ccf()
      {
      }

      void
	  atsc_rx_filter::pfb_arb_resampler_ccf::create_taps(const float newtaps[], int size,
                                         float ourtaps[FILTER_SIZE][RRC_NTAPS/FILTER_SIZE+1],
                                         fir_filter_ccf ourfilter[])
      {
        unsigned int ntaps = size;
        float tmp_taps[FILTER_SIZE + RRC_NTAPS];
        d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_int_rate);

        // Make a vector of the taps plus fill it out with 0's to fill
        // each polyphase filter with exactly d_taps_per_filter
        for(int i=0; i < size; i++)
        	tmp_taps[i] = newtaps[i];

        for(int i=size; i < d_int_rate*d_taps_per_filter; i++)
        	tmp_taps[i] = 0.0;

        for(unsigned int i = 0; i < d_int_rate; i++) {
          // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
          for(unsigned int j = 0; j < d_taps_per_filter; j++) {
            ourtaps[i][j] = tmp_taps[i + j*d_int_rate];
          }

          // Build a filter for each channel and add it's taps to it
          ourfilter[i].set_taps(ourtaps[i],d_taps_per_filter);
        }
      }

      void
	  atsc_rx_filter::pfb_arb_resampler_ccf::create_diff_taps(const float newtaps[],
                                              float difftaps[])
      {
        // Calculate the differential taps using a derivative filter
        float diff_filter[2];
        diff_filter[0] = -1;
        diff_filter[1] = 1;

        for(unsigned int i = 0; i < RRC_NTAPS-1; i++) {
          float tap = 0;
          for(unsigned int j = 0; j < 2; j++) {
            tap += diff_filter[j]*newtaps[i+j];
          }
          difftaps[i]=tap;
        }
        difftaps[RRC_NTAPS-1]=0;
      }

      void
	  atsc_rx_filter::pfb_arb_resampler_ccf::set_taps(const float taps[])
      {
        float dtaps[RRC_NTAPS];
        create_diff_taps(taps, dtaps);
        create_taps(taps, RRC_NTAPS, d_taps, d_filters);
        create_taps(dtaps, RRC_NTAPS, d_dtaps, d_diff_filters);
      }

      void
	  atsc_rx_filter::pfb_arb_resampler_ccf::set_rate(float rate)
      {
        d_dec_rate = (unsigned int)floor(d_int_rate/rate);
        d_flt_rate = (d_int_rate/rate) - d_dec_rate;
      }

      void
	  atsc_rx_filter::pfb_arb_resampler_ccf::set_phase(float ph)
      {
        float ph_diff = 2.0*M_PI / (float)FILTER_SIZE;
        d_last_filter = static_cast<int>(ph / ph_diff);
      }

      float
	  atsc_rx_filter::pfb_arb_resampler_ccf::phase() const
      {
        float ph_diff = 2.0*M_PI / static_cast<float>(FILTER_SIZE);
        return d_last_filter * ph_diff;
      }

      unsigned int
	  atsc_rx_filter::pfb_arb_resampler_ccf::taps_per_filter() const
      {
        return d_taps_per_filter;
      }

      float
	  atsc_rx_filter::pfb_arb_resampler_ccf::phase_offset(float freq, float fs)
      {
        float adj = (2.0*M_PI)*(freq/fs)/static_cast<float>(d_int_rate);
        return -adj * d_est_phase_change;
      }

      int
	  atsc_rx_filter::pfb_arb_resampler_ccf::filter(complex<float> *out, complex<float> *in,
                                    int n_to_read, int &n_read)
      {
        int i_out = 0, i_in = 0;
        static unsigned int j = d_last_filter;
        complex<float> o0, o1;

        complex<float> input[TRANSIENT+IN_SIZE/2] = {0};
    	static complex<float> prev_tail[TRANSIENT] = {0};
		for(int i = 0; i < TRANSIENT; i++)
			input[i] = prev_tail[i];
		for(int i = TRANSIENT; i < IN_SIZE/2+TRANSIENT; i++)
			input[i] = in[i-TRANSIENT];
    	for(int i=0; i<TRANSIENT; i++)
    		prev_tail[i] = in[IN_SIZE/2-TRANSIENT+i];

		while(i_in < n_to_read) {
		  // start j by wrapping around mod the number of channels
		  while(j < d_int_rate) {
			// Take the current filter and derivative filter output
			o0 = d_filters[j].filter(&input[i_in]);
			o1 = d_diff_filters[j].filter(&input[i_in]);

			out[i_out] = o0 + o1*d_acc;     // linearly interpolate between samples
			i_out++;
			// Adjust accumulator and index into filterbank
			d_acc += d_flt_rate;
			j += d_dec_rate + (int)floor(d_acc);
			d_acc = mod(d_acc, 1.0);
		  }
		  i_in += (int)(j / d_int_rate);
		  j = j % d_int_rate;
		}
		d_last_filter = j; // save last filter state for re-entry

        n_read = i_in;   // return how much we've actually read
        return i_out;    // return how much we've produced
      }

      void
	  atsc_rx_filter::pfb_arb_resampler_ccf::set_acc(float acc)
      {
    	  d_acc = acc;
      }

      float
	  atsc_rx_filter::pfb_arb_resampler_ccf::acc()
      {
    	  return d_acc;
      }

      float
	  atsc_rx_filter::pfb_arb_resampler_ccf::mod(float a, float b)
      {
    	  return a - b*floor(a/b);
      }


      atsc_rx_filter::fir_filter_ccf::fir_filter_ccf(void)
      	  : d_naligned(0), d_ntaps(0), d_align(0)
    {

    }
      atsc_rx_filter::fir_filter_ccf::fir_filter_ccf(int decimation,
				     const float taps[], int size)
      {

	set_taps(taps,size);

      }

      atsc_rx_filter::fir_filter_ccf::~fir_filter_ccf()
      {
      }

      void
	  atsc_rx_filter::fir_filter_ccf::set_taps(const float taps[], int size)
      {

	d_ntaps = size;
	for(int i=0;i<size;i++)
		d_taps[i] = taps[i];

	float temp;
	for (int i = 0; i < d_ntaps/2; i++) {
	    temp = d_taps[d_ntaps-i-1];
	    d_taps[d_ntaps-i-1] = d_taps[i];
	    d_taps[i] = temp;
	}

      }

      unsigned int
	  atsc_rx_filter::fir_filter_ccf::ntaps() const
      {
	return d_ntaps;
      }

      complex<float>
      atsc_rx_filter::fir_filter_ccf::filter(const complex<float> input[])
      {
		complex<float> res;
		float realpt = 0, imagpt = 0;

		for(int i = 0; i < d_ntaps; i++){
		  realpt += real(input[i]) * d_taps[i];
		  imagpt += imag(input[i]) * d_taps[i];
		}
		res.real() = realpt;
		res.imag() = imagpt;

		return res;
      }

      void
	  atsc_rx_filter::fir_filter_ccf::filterNdec(complex<float> output[],
				 const complex<float> input[],
				 unsigned long n,
				 unsigned int decimate)
      {
	unsigned long j = 0;
	for(unsigned long i = 0; i < n; i++){
	  output[i] = filter(&input[j]);
	  j += decimate;
	}
      }

  void atsc_fpll(complex<float> in, float &out)
  {
	static atsc_fpll_impl pll;
	pll.work(in, out);
  }

  atsc_fpll_impl::atsc_fpll_impl()
  {
//      d_afc.set_taps(1.0-exp(-1.0/RATE/5e-6));
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

d_nco.step();                  // increment phase
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
if (x > limit)
  x = limit;
else if (x < -limit)
  x = -limit;

static const float alpha = 0.01;
static const float beta = alpha*alpha/4.0;
d_nco.adjust_phase(alpha*x);
d_nco.adjust_freq(beta*x);
  }
