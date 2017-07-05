/*!
* \brief a computationally efficient controllable DC blocker
*
* \details
* This block implements a computationally efficient DC blocker
* that produces a tighter notch filter around DC for a smaller
* group delay than an equivalent FIR filter or using a single
* pole IIR filter (though the IIR filter is computationally
* cheaper).
*
* The block defaults to using a delay line of length 32 and the
* long form of the filter. Optionally, the delay line length can
* be changed to alter the width of the DC notch (longer lines
* will decrease the width).
*
* The long form of the filter produces a nearly flat response
* outside of the notch but at the cost of a group delay of 2D-2.
*
* The short form of the filter does not have as flat a response
* in the passband but has a group delay of only D-1 and is
* cheaper to compute.
*
* The theory behind this block can be found in the paper:
*
*    <B><EM>R. Yates, "DC Blocker Algorithms," IEEE Signal Processing Magazine,
*        Mar. 2008, pp 132-134.</EM></B>
*
* Modified by Andrew Lanez and Sachin Bharadwaj Sundramurthy (Team Rabbit Ears)
* for RFNoC and Vivado HLS Challenge
*/

#include <complex>
#include <ap_int.h>
static const int D = 128;

struct axis_float {
	float data;
    ap_uint<1> last;
};

void dc_blocker_agc(axis_float in[D], axis_float out[D]);

class moving_averager_f
{
public:
	float d_delay_line[D-1];

	moving_averager_f();
	~moving_averager_f();

	float filter(float x);
	float delayed_sig() { return d_out; }

private:
	int d_length;
	float d_out, d_out_d1, d_out_d2;
};

class dc_blocker_ff_impl //: public dc_blocker_ff
{
private:
	int d_length;

public:
	moving_averager_f d_ma_0;
	moving_averager_f d_ma_1;

	dc_blocker_ff_impl();

	~dc_blocker_ff_impl();

	int group_delay();

	void work(float *in,
              float *out);
};

void agc(axis_float &in, axis_float &out);

      /*!
       * \brief high performance Automatic Gain Control class for float signals.
       *
       * Power is approximated by absolute value
       */
class agc_ff
{
public:
	/*!
	 * Construct a floating point value AGC loop implementation object.
	 *
	 * \param rate the update rate of the loop.
	 * \param reference reference value to adjust signal power to.
	 * \param gain initial gain value.
	 * \param max_gain maximum gain value (0 for unlimited).
	 */
	agc_ff(float rate = 0.00001, float reference = 4.0,
		   float gain = 1.0, float max_gain = 65536)
	  : _rate(rate), _reference(reference), _gain(gain),
	  _max_gain(max_gain) {};

	~agc_ff() {};

	float rate () const      { return _rate; }
	float reference () const { return _reference; }
	float gain () const      { return _gain;  }
	float max_gain () const  { return _max_gain; }

	void set_rate (float rate) { _rate = rate; }
	void set_reference (float reference) { _reference = reference; }
	void set_gain (float gain) { _gain = gain; }
	void set_max_gain (float max_gain) { _max_gain = max_gain; }

	float scale (float input)
	{
		float output = input * _gain;
		_gain += (_reference - fabsf (output)) * _rate;
		if(_max_gain > 0.0 && _gain > _max_gain)
			_gain = _max_gain;
		return output;
	}

protected:
	float _rate;		// adjustment rate
	float _reference;	// reference value
	float _gain;		// current gain
	float _max_gain;	// maximum gain
};
