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

#include <stdio.h>
#include <complex>
#include <ap_int.h>

//RATE: atsc_sym_rate*sps = (4.5e6/286*684) * 1.1
static const float RATE = 11838461.53846153846153846154;

struct axis_float {
	float data;
    ap_uint<1> last;
};

void atsc_fpll(axis_float in[2], axis_float &out);

/*!
 * \brief base class template for Numerically Controlled Oscillator (NCO)
 * \ingroup misc
 */
class nco
{
public:
	nco() : phase(0), phase_inc(0) {}

	~nco() {}

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
		if(std::abs(phase) > M_PI) {
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

	/*!
	* \brief install \p alpha as the current taps.
	*/
	void set_taps(float alpha)
	{
		d_alpha = alpha;
		d_one_minus_alpha = 1.0 - alpha;
	}

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
