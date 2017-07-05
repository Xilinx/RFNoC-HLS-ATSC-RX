/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include <complex>
#include <ap_int.h>

#define IN_SIZE 	64
#define OUT_SIZE 	61	//ceil(1.8941538461538463*IN_SIZE/2)
#define RRC_NTAPS 	299
#define FILTER_SIZE	16
#define TRANSIENT	18 	//Overlap/discard length of output filter arm's transient

//RATE: atsc_sym_rate*sps = (4.5e6/286*684) * 1.1
static const float RATE = 11838461.53846153846153846154;

struct axis_float {
	float data;
    ap_uint<1> last;
};

const double interp = 1.894153846153846298960843341774307191371917724609375;
const float rrc_taps[RRC_NTAPS] = {0.00057239673333242536, 0.00053110171575099230, 0.00046942124026827514, 0.00038864865200594068, 0.00029078923398628831, 0.00017851754091680050, 0.00005511023846338503, -0.00007564402767457068, -0.00020955537911504507, -0.00034216098720207810, \
-0.00046886742347851396, -0.00058510212693363428, -0.00068646972067654133, -0.00076890812488272786, -0.00082883832510560751, -0.00086330337217077613, -0.00087009201524779201, -0.00084784039063379169, -0.00079610972898080945, -0.00071543565718457103, -0.00060734624275937676, \
-0.00047434779116883874, -0.00031987740658223629, -0.00014822289813309908, 0.00003558871321729384, 0.00022593094035983086, 0.00041674260864965618, 0.00060171284712851048, 0.00077448092633858323, 0.00092884461628273129, 0.00105897104367613790, 0.00115960265975445510, \
0.00122625171206891540, 0.00125537603162229060, 0.00124453043099492790, 0.00119248684495687480, 0.00109932036139070990, 0.00096645357552915812, 0.00079666060628369451, 0.00059402600163593888, 0.00036386010469868779, 0.00011257109872531146, -0.00015250233991537243, \
-0.00042329516145400703, -0.00069125049049034715, -0.00094757816987112164, -0.00118353043217211960, -0.00139068590942770240, -0.00156123249325901270, -0.00168824126012623310, -0.00176592264324426650, -0.00178985297679901120, -0.00175716681405901910, -0.00166670710314065220, \
-0.00151912379078567030, -0.00131692050490528350, -0.00106444256380200390, -0.00076780689414590597, -0.00043477260624058545, -0.00007455481681972742, 0.00030241365311667323, 0.00068476813612505794, 0.00106053764466196300, 0.00141750078182667490, 0.00174356286879628900, \
0.00202714232727885250, 0.00225755455903708930, 0.00242538284510374070, 0.00252282177098095420, 0.00254398258402943610, 0.00248515070416033270, 0.00234498502686619760, 0.00212464947253465650, 0.00182787270750850440, 0.00146092928480356930, 0.00103254290297627450, \
0.00055370887275785208, 0.00003744073910638690, -0.00050155509961768985, -0.00104726885911077260, -0.00158283137716352940, -0.00209100102074444290, -0.00255468091927468780, -0.00295745115727186200, -0.00328409927897155280, -0.00352113717235624790, -0.00365728279575705530, \
-0.00368389650247991090, -0.00359535007737576960, -0.00338932476006448270, -0.00306701892986893650, -0.00263325776904821400, -0.00209650048054754730, -0.00146873923949897290, -0.00076528929639607668, -0.00000447181537310826, 0.00079280440695583820, 0.00160355388652533290, \
0.00240330793894827370, 0.00316677475348114970, 0.00386855145916342740, 0.00448386836796998980, 0.00498934462666511540, 0.00536374095827341080, 0.00558867538347840310, 0.00564929330721497540, 0.00553486635908484460, 0.00523929949849843980, 0.00476153194904327390, \
0.00410581473261117940, 0.00328185758553445340, 0.00230483245104551320, 0.00119522865861654280, -0.00002143628444173373, -0.00131505355238914490, -0.00265149516053497790, -0.00399331422522664070, -0.00530055584385991100, -0.00653166510164737700, -0.00764446333050727840, \
-0.00859718117862939830, -0.00934950914233922960, -0.00986365135759115220, -0.01010535378009080900, -0.01004486531019210800, -0.00965782906860113140, -0.00892606936395168300, -0.00783823616802692410, -0.00639031967148184780, -0.00458599068224430080, -0.00243677268736064430, \
0.00003797296085394919, 0.00281124678440392020, 0.00584879331290721890, 0.00910966191440820690, 0.01254694163799285900, 0.01610863395035266900, 0.01973867788910865800, 0.02337807230651378600, 0.02696608193218708000, 0.03044152073562145200, 0.03374403342604637100, \
0.03681540116667747500, 0.03960079327225685100, 0.04204995930194854700, 0.04411834478378295900, 0.04576805606484413100, 0.04696872085332870500, 0.04769813641905784600, 0.04794279113411903400, 0.04769813641905784600, 0.04696872085332870500, 0.04576805606484413100, \
0.04411834478378295900, 0.04204995930194854700, 0.03960079327225685100, 0.03681540116667747500, 0.03374403342604637100, 0.03044152073562145200, 0.02696608193218708000, 0.02337807230651378600, 0.01973867788910865800, 0.01610863395035266900, 0.01254694163799285900, \
0.00910966191440820690, 0.00584879331290721890, 0.00281124678440392020, 0.00003797296085394919, -0.00243677268736064430, -0.00458599068224430080, -0.00639031967148184780, -0.00783823616802692410, -0.00892606936395168300, -0.00965782906860113140, -0.01004486531019210800, \
-0.01010535378009080900, -0.00986365135759115220, -0.00934950914233922960, -0.00859718117862939830, -0.00764446333050727840, -0.00653166510164737700, -0.00530055584385991100, -0.00399331422522664070, -0.00265149516053497790, -0.00131505355238914490, -0.00002143628444173373, \
0.00119522865861654280, 0.00230483245104551320, 0.00328185758553445340, 0.00410581473261117940, 0.00476153194904327390, 0.00523929949849843980, 0.00553486635908484460, 0.00564929330721497540, 0.00558867538347840310, 0.00536374095827341080, 0.00498934462666511540, \
0.00448386836796998980, 0.00386855145916342740, 0.00316677475348114970, 0.00240330793894827370, 0.00160355388652533290, 0.00079280440695583820, -0.00000447181537310826, -0.00076528929639607668, -0.00146873923949897290, -0.00209650048054754730, -0.00263325776904821400, \
-0.00306701892986893650, -0.00338932476006448270, -0.00359535007737576960, -0.00368389650247991090, -0.00365728279575705530, -0.00352113717235624790, -0.00328409927897155280, -0.00295745115727186200, -0.00255468091927468780, -0.00209100102074444290, -0.00158283137716352940, \
-0.00104726885911077260, -0.00050155509961768985, 0.00003744073910638690, 0.00055370887275785208, 0.00103254290297627450, 0.00146092928480356930, 0.00182787270750850440, 0.00212464947253465650, 0.00234498502686619760, 0.00248515070416033270, 0.00254398258402943610, \
0.00252282177098095420, 0.00242538284510374070, 0.00225755455903708930, 0.00202714232727885250, 0.00174356286879628900, 0.00141750078182667490, 0.00106053764466196300, 0.00068476813612505794, 0.00030241365311667323, -0.00007455481681972742, -0.00043477260624058545, \
-0.00076780689414590597, -0.00106444256380200390, -0.00131692050490528350, -0.00151912379078567030, -0.00166670710314065220, -0.00175716681405901910, -0.00178985297679901120, -0.00176592264324426650, -0.00168824126012623310, -0.00156123249325901270, -0.00139068590942770240, \
-0.00118353043217211960, -0.00094757816987112164, -0.00069125049049034715, -0.00042329516145400703, -0.00015250233991537243, 0.00011257109872531146, 0.00036386010469868779, 0.00059402600163593888, 0.00079666060628369451, 0.00096645357552915812, 0.00109932036139070990, \
0.00119248684495687480, 0.00124453043099492790, 0.00125537603162229060, 0.00122625171206891540, 0.00115960265975445510, 0.00105897104367613790, 0.00092884461628273129, 0.00077448092633858323, 0.00060171284712851048, 0.00041674260864965618, 0.00022593094035983086, \
0.00003558871321729384, -0.00014822289813309908, -0.00031987740658223629, -0.00047434779116883874, -0.00060734624275937676, -0.00071543565718457103, -0.00079610972898080945, -0.00084784039063379169, -0.00087009201524779201, -0.00086330337217077613, -0.00082883832510560751, \
-0.00076890812488272786, -0.00068646972067654133, -0.00058510212693363428, -0.00046886742347851396, -0.00034216098720207810, -0.00020955537911504507, -0.00007564402767457068, 0.00005511023846338503, 0.00017851754091680050, 0.00029078923398628831, 0.00038864865200594068, \
0.00046942124026827514, 0.00053110171575099230, 0.00057239673333242536};

void atsc_rx_filter_fpll(axis_float in[IN_SIZE], axis_float out[OUT_SIZE]);

struct atsc_rx_filter
{

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
						  const float taps[],
						  unsigned int filter_size);

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

};

void atsc_fpll(std::complex<float> in, float &out);

	/*!
	 * \brief base class template for Numerically Controlled Oscillator (NCO)
	 * \ingroup misc
	 */
	class nco
	{
	public:
	  nco() : phase(0), phase_inc(0) {}

	  virtual ~nco() {}

	  // radians
	  void set_phase(double angle)
	  {
		phase = angle;
	  }

	  void adjust_phase(double delta_phase)
	  {
		phase += delta_phase;
	  }

	  // angle_rate is in radians / step
	  void set_freq(double angle_rate)
	  {
		phase_inc = angle_rate;
	  }

	  // angle_rate is a delta in radians / step
	  void adjust_freq(double delta_angle_rate)
	  {
		phase_inc += delta_angle_rate;
	  }

	  // increment current phase angle
	  void step()
	  {
		phase += phase_inc;
//		if(fabs(phase) > M_PI) {
		if(std::abs(phase) > M_PI) {
		  while(phase > M_PI)
			phase -= 2*M_PI;

		  while(phase < -M_PI)
			phase += 2*M_PI;
		}
	  }

	  void step(int n)
	  {
		phase += phase_inc * n;
//		if(fabs(phase) > M_PI){
		if(std::abs(phase) > M_PI){
		  while(phase > M_PI)
			phase -= 2*M_PI;

		  while(phase < -M_PI)
			phase += 2*M_PI;
		}
	  }

	  // units are radians / step
	  double get_phase() const { return phase; }
	  double get_freq() const { return phase_inc; }

	  // compute cos or sin for current phase angle
	  float cos() const { return std::cos((float)phase); }
	  float sin() const { return std::sin((float)phase); }

	protected:
	  double phase;
	  double phase_inc;
	};

    /*!
     * \brief class template for single pole IIR filter
     */
    class single_pole_iir
    {
    public:
      /*!
       * \brief construct new single pole IIR with given alpha
       *
       * computes y(i) = (1-alpha) * y(i-1) + alpha * x(i)
       */
      single_pole_iir(float alpha = 1.0)
      {
	d_prev_output = 0;
	set_taps(alpha);
      }
	  std::complex<float>
	  filter(const std::complex<float> input)
	  {
		std::complex<float>	output;

		output = d_alpha * input + d_one_minus_alpha * d_prev_output;
		d_prev_output = output;

		return (std::complex<float>) output;
	  }

	  void
	  filterN(std::complex<float> output[],
							   const std::complex<float> input[],
							   unsigned long n)
	  {
		for(unsigned i = 0; i < n; i++)
	output[i] = filter(input[i]);
	  }

      /*!
       * \brief install \p alpha as the current taps.
       */
      void set_taps(float alpha)
      {
	d_alpha = alpha;
	d_one_minus_alpha = 1.0 - alpha;
      }

      //! reset state to zero
      void reset()
      {
	d_prev_output = 0;
      }

      std::complex<float> prev_output() const { return d_prev_output; }

    protected:
      float	d_alpha;
      float	d_one_minus_alpha;
      std::complex<float>	d_prev_output;
    };

    class atsc_fpll_impl //: public atsc_fpll
    {
    private:
      nco d_nco;
      single_pole_iir d_afc;

    public:
      atsc_fpll_impl();
      ~atsc_fpll_impl();

      void work(std::complex<float> &in, float &out);
    };
