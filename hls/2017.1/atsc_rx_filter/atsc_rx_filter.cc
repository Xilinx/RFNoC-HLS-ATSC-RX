#include "atsc_rx_filter.h"
#include <stdio.h>
using std::complex;

void atsc_rx_filter(axis_float in[IN_SIZE], axis_float out[OUT_SIZE])
{
#pragma HLS INTERFACE axis depth=64 port=in
#pragma HLS INTERFACE axis depth=122 port=out
#pragma HLS INTERFACE ap_ctrl_none port=return

	int n_out;
	static float acc_hist = 0, ph = 1.9634954929351806640625;

	complex<float> pfb_in[IN_SIZE/2] = {0};
	complex<float> pfb_out[OUT_SIZE/2] = {0};
	for(int i=0; i<IN_SIZE; i+=2) {
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
		pfb_in[i/2].real() = in[i].data;
		pfb_in[i/2].imag() = in[i+1].data;
	}

    static pfb_arb_resampler_ccf pfb;

    pfb.set_phase(ph);
	pfb.set_acc(acc_hist);
	n_out = pfb.filter(pfb_out, pfb_in, IN_SIZE/2);
    ph = pfb.phase();
	acc_hist = pfb.acc();

	for(int i=0; i<n_out*2; i+=2) {
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
    	out[i].data = real(pfb_out[i/2]);
    	out[i].last = false;
    	out[i+1].data = imag(pfb_out[i/2]);
    	out[i+1].last = i == (n_out*2-2);
	}
}

pfb_arb_resampler_ccf::pfb_arb_resampler_ccf()
{
	d_acc = 0; // start accumulator at 0
	d_last_filter = (RRC_NTAPS/2) % FILTER_SIZE;
}

pfb_arb_resampler_ccf::~pfb_arb_resampler_ccf()
{
}

complex<float>
pfb_arb_resampler_ccf::filters(complex<float> input[], int j)
{
	complex<float> res;
	float realpt = 0, imagpt = 0;
	for(int i = 0; i < TAPS; i++){
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
		realpt += real(input[i]) * d_filters[j][i];
		imagpt += imag(input[i]) * d_filters[j][i];
	}
	res.real() = realpt;
	res.imag() = imagpt;

	return res;
}

complex<float>
pfb_arb_resampler_ccf::diff_filters(complex<float> input[], int j)
{
	complex<float> res;
	float realpt = 0, imagpt = 0;
	for(int i = 0; i < TAPS; i++){
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
		realpt += real(input[i]) * d_diff_filters[j][i];
		imagpt += imag(input[i]) * d_diff_filters[j][i];
	}
	res.real() = realpt;
	res.imag() = imagpt;

	return res;
}

void
pfb_arb_resampler_ccf::set_phase(float ph)
{
	float ph_diff = 2.0*M_PI / (float)FILTER_SIZE;
	d_last_filter = static_cast<int>(ph / ph_diff);
}

float
pfb_arb_resampler_ccf::phase() const
{
	float ph_diff = 2.0*M_PI / static_cast<float>(FILTER_SIZE);
	return d_last_filter * ph_diff;
}

int
pfb_arb_resampler_ccf::filter(complex<float> *out, complex<float> *in,
							int n_to_read)
{
	int i_out = 0, i_in = 0;
	static unsigned int j = d_last_filter;
	complex<float> o0, o1;

	// Overlap last 18 samples every input so output transient can be discarded
	complex<float> input[TRANSIENT+IN_SIZE/2] = {0};
	static complex<float> prev_tail[TRANSIENT] = {0};
	for(int i = 0; i < TRANSIENT; i++)
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
		input[i] = prev_tail[i];
	for(int i = TRANSIENT; i < IN_SIZE/2+TRANSIENT; i++)
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
		input[i] = in[i-TRANSIENT];
	for(int i=0; i<TRANSIENT; i++)
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
		prev_tail[i] = in[IN_SIZE/2-TRANSIENT+i];

	while(i_in < n_to_read) {
		#pragma HLS PIPELINE
		#pragma HLS UNROLL
		// start j by wrapping around mod the number of channels
		while(j < FILTER_SIZE) {
			#pragma HLS PIPELINE
			#pragma HLS UNROLL
			// Take the current filter and derivative filter output
			o0 = filters(&input[i_in], j);
			o1 = diff_filters(&input[i_in], j);

			out[i_out] = o0 + o1*d_acc;     // linearly interpolate between samples
			i_out++;
			// Adjust accumulator and index into filterbank
			d_acc += FLT_RATE;
			j += (int)floor(d_acc);
			d_acc = mod(d_acc, 1.0);
	  }
	  i_in += (int)(j / FILTER_SIZE);
	  j = j % FILTER_SIZE;
	}
	d_last_filter = j; // save last filter state for re-entry

	return i_out;    // return how much we've produced
}

void
pfb_arb_resampler_ccf::set_acc(float acc)
{
	d_acc = acc;
}

float
pfb_arb_resampler_ccf::acc()
{
	return d_acc;
}

float
pfb_arb_resampler_ccf::mod(float a, float b)
{
	return a - b*floor(a/b);
}
