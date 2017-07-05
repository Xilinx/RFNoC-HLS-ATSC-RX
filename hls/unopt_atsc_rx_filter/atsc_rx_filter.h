#include <complex>
#include <ap_int.h>

#define IN_SIZE 	64
#define OUT_SIZE 	122	//2*ceil(1.8941538461538463*IN_SIZE/2)
#define RRC_NTAPS 	299
#define FILTER_SIZE	16
#define TRANSIENT	18 	//Overlap/discard length of output filter arm's transient
#define INPUT_RATE 6.25e6 // Sample rate
#define SPS 1.1 // Oversampling ratio

struct axis_float {
	float data;
    ap_uint<1> last;
};

void atsc_rx_filter(axis_float in[IN_SIZE], axis_float out[OUT_SIZE]);

static void
root_raised_cosine(float taps[],
        double gain,
        double sampling_freq,
        double symbol_rate, // Symbol rate, NOT bitrate (unless BPSK)
        double alpha,       // Excess Bandwidth Factor
        int ntaps);

class fir_filter_ccf
{
	public:
		  fir_filter_ccf(void);
		fir_filter_ccf(int decimation,
						 const float taps[], int size);
	~fir_filter_ccf();

	void set_taps(const float taps[], int size);
	unsigned int ntaps() const;

	std::complex<float> filter(const std::complex<float> input[]);
	void filterN(std::complex<float> output[],
			 const std::complex<float> input[],
			 unsigned long n);
	void filterNdec(std::complex<float> output[],
			const std::complex<float> input[],
			unsigned long n,
			unsigned int decimate);

	protected:
	float d_taps[RRC_NTAPS/FILTER_SIZE+1];
	unsigned int d_ntaps;
	int          d_align;
	int          d_naligned;
};


/*!
* \brief Polyphase filterbank arbitrary resampler with
*        gr_complex input, gr_complex output and float taps
* \ingroup resamplers_blk
*
* \details
* This  takes in a signal stream and performs arbitrary
* resampling. The resampling rate can be any real number
* <EM>r</EM>. The resampling is done by constructing <EM>N</EM>
* filters where <EM>N</EM> is the interpolation rate.  We then
* calculate <EM>D</EM> where <EM>D = floor(N/r)</EM>.
*
* Using <EM>N</EM> and <EM>D</EM>, we can perform rational
* resampling where <EM>N/D</EM> is a rational number close to
* the input rate <EM>r</EM> where we have <EM>N</EM> filters
* and we cycle through them as a polyphase filterbank with a
* stride of <EM>D</EM> so that <EM>i+1 = (i + D) % N</EM>.
*
* To get the arbitrary rate, we want to interpolate between two
* points. For each value out, we take an output from the
* current filter, <EM>i</EM>, and the next filter <EM>i+1</EM>
* and then linearly interpolate between the two based on the
* real resampling rate we want.
*
* The linear interpolation only provides us with an
* approximation to the real sampling rate specified. The error
* is a quantization error between the two filters we used as
* our interpolation points.  To this end, the number of
* filters, <EM>N</EM>, used determines the quantization error;
* the larger <EM>N</EM>, the smaller the noise. You can design
* for a specified noise floor by setting the filter size
* (parameters <EM>filter_size</EM>). The size defaults to 32
* filters, which is about as good as most implementations need.
*
* The trick with designing this filter is in how to specify the
* taps of the prototype filter. Like the PFB interpolator, the
* taps are specified using the interpolated filter rate. In
* this case, that rate is the input sample rate multiplied by
* the number of filters in the filterbank, which is also the
* interpolation rate. All other values should be relative to
* this rate.
*
* For example, for a 32-filter arbitrary resampler and using
* the GNU Radio's firdes utility to build the filter, we build
* a low-pass filter with a sampling rate of <EM>fs</EM>, a 3-dB
* bandwidth of <EM>BW</EM> and a transition bandwidth of
* <EM>TB</EM>. We can also specify the out-of-band attenuation
* to use, <EM>ATT</EM>, and the filter window function (a
* Blackman-harris window in this case). The first input is the
* gain of the filter, which we specify here as the
* interpolation rate (<EM>32</EM>).
*
*   <B><EM>self._taps = filter.firdes.low_pass_2(32, 32*fs, BW, TB,
*      attenuation_dB=ATT, window=filter.firdes.WIN_BLACKMAN_hARRIS)</EM></B>
*
* The theory behind this block can be found in Chapter 7.5 of
* the following book.
*
*   <B><EM>f. harris, "Multirate Signal Processing for Communication
*      Systems", Upper Saddle River, NJ: Prentice Hall, Inc. 2004.</EM></B>
*/
class pfb_arb_resampler_ccf
{
	private:
	fir_filter_ccf d_filters[FILTER_SIZE];
	fir_filter_ccf d_diff_filters[FILTER_SIZE];
	float d_taps[FILTER_SIZE][RRC_NTAPS/FILTER_SIZE+1];
	float d_dtaps[FILTER_SIZE][RRC_NTAPS/FILTER_SIZE+1];
	unsigned int d_int_rate;          // the number of filters (interpolation rate)
	unsigned int d_dec_rate;          // the stride through the filters (decimation rate)
	float        d_flt_rate;          // residual rate for the linear interpolation
	float        d_acc;               // accumulator; holds fractional part of sample
	unsigned int d_last_filter;       // stores filter for re-entry
	unsigned int d_taps_per_filter;   // num taps for each arm of the filterbank
	int d_delay;                      // filter's group delay
	float d_est_phase_change;         // est. of phase change of a sine wave through filt.

	/*!
	 * Takes in the taps and convolves them with [-1,0,1], which
	 * creates a differential set of taps that are used in the
	 * difference filterbank.
	 * \param newtaps (vector of floats) The prototype filter.
	 * \param difftaps (vector of floats) (out) The differential filter taps.
	 */
	void create_diff_taps(const float newtaps[],
						  float difftaps[]);

	/*!
	 * Resets the filterbank's filter taps with the new prototype filter
	 * \param newtaps    (vector of floats) The prototype filter to populate the filterbank.
	 *                   The taps should be generated at the interpolated sampling rate.
	 * \param ourtaps    (vector of floats) Reference to our internal member of holding the taps.
	 * \param ourfilter  (vector of filters) Reference to our internal filter to set the taps for.
	 */
	void create_taps(const float newtaps[], int size,
									   float ourtaps[FILTER_SIZE][RRC_NTAPS/FILTER_SIZE+1],
									   fir_filter_ccf ourfilter[]);

	public:
	/*!
	 * Creates a kernel to perform arbitrary resampling on a set of samples.
	 * \param rate  (float) Specifies the resampling rate to use
	 * \param taps  (vector/list of floats) The prototype filter to populate the filterbank. The taps       *              should be generated at the filter_size sampling rate.
	 * \param filter_size (unsigned int) The number of filters in the filter bank. This is directly
	 *                    related to quantization noise introduced during the resampling.
	 *                    Defaults to 32 filters.
	 */
	pfb_arb_resampler_ccf(float rate,
						  const float taps[]);

	~pfb_arb_resampler_ccf();

	/*!
	 * Resets the filterbank's filter taps with the new prototype filter
	 * \param taps (vector/list of floats) The prototype filter to populate the filterbank.
	 */
	void set_taps(const float taps[]);

	/*!
	 * Sets the resampling rate of the block.
	 */
	void set_rate(float rate);

	/*!
	 * Sets the current phase offset in filterbank.
	 */
	void set_phase(float ph);

	/*!
	 * Gets the current phase of the resampler filterbank.
	 */
	float phase() const;

	/*!
	 * Gets the number of taps per filter.
	 */
	unsigned int taps_per_filter() const;

	unsigned int interpolation_rate() const { return d_int_rate; }
	unsigned int decimation_rate() const { return d_dec_rate; }
	float fractional_rate() const { return d_flt_rate; }

	/*!
	 * Get the group delay of the filter.
	 */
	int group_delay() const { return d_delay; }

	/*!
	 * Calculates the phase offset expected by a sine wave of
	 * frequency \p freq and sampling rate \p fs (assuming input
	 * sine wave has 0 degree phase).
	 */
	float phase_offset(float freq, float fs);

	/*!
	 * Performs the filter operation that resamples the signal.
	 *
	 * This block takes in a stream of samples and outputs a
	 * resampled and filtered stream. This block should be called
	 * such that the output has \p rate * \p n_to_read amount of
	 * space available in the \p output buffer.
	 *
	 * \param output The output samples at the new sample rate.
	 * \param input An input vector of samples to be resampled
	 * \param n_to_read Number of samples to read from \p input.
	 * \param n_read (out) Number of samples actually read from \p input.
	 * \return Number of samples put into \p output.
	 */
	int filter(std::complex<float> *out, std::complex<float> *in,
			   int n_to_read, int &n_read);

	void set_acc(float acc);

	float acc();

	float mod(float a, float b);
};


