/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#include <ap_int.h>

#define SIZE 1024

struct axis_float {
	float data;
    ap_uint<1> last;
};

void agc(axis_float in[SIZE], axis_float out[SIZE]);

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
		#pragma HLS RESOURCE variable=output core=FMul_maxdsp latency=0
		float diff = _reference - fabsf (output);
		#pragma HLS RESOURCE variable=diff core=FAddSub_fulldsp latency=0
		float prod = diff * _rate;
		#pragma HLS RESOURCE variable=prod core=FMul_maxdsp latency=0
		_gain += prod;
		#pragma HLS RESOURCE variable=_gain core=FAddSub_fulldsp latency=0
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
