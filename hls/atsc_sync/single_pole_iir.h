/* -*- c++ -*- */
/*
 * Copyright 2002,2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_SINGLE_POLE_IIR_H
#define INCLUDED_SINGLE_POLE_IIR_H

//#include <gnuradio/filter/api.h>
//#include <stdexcept>
//#include <gnuradio/gr_complex.h>

#include <complex>

namespace gr {
  namespace filter {

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

  	  /*!
  	   * \brief compute a single output value.
  	   * \returns the filtered input value.
  	   */
  	  float filter(const float input)
  	  {
  	    float	output;

  	    output = d_alpha * input + d_one_minus_alpha * d_prev_output;
  	    d_prev_output = output;

  	    return (float) output;
  	  }
  	  /*!
  	   * \brief compute an array of N output values.
  	   * \p input must have n valid entries.
  	   */
  	  void filterN(float output[], const float input[], unsigned long n)
  	  {
  	    for(unsigned i = 0; i < n; i++)
  	    	output[i] = filter(input[i]);
  	  }

  	  /*!
  	   * \brief install \p alpha as the current taps.
  	   */
  	  void set_taps(float alpha)
  	  {
  	//	if(alpha < 0 || alpha > 1)
  	//	  throw std::out_of_range("Alpha must be in [0, 1]\n");

  	d_alpha = alpha;
  	d_one_minus_alpha = 1.0 - alpha;
  	  }

  	  //! reset state to zero
  	  void reset()
  	  {
  	d_prev_output = 0;
  	  }

  	  float prev_output() const { return d_prev_output; }

  	protected:
  	  float	d_alpha;
  	  float	d_one_minus_alpha;
  	  float	d_prev_output;
  	};

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_SINGLE_POLE_IIR_H */
