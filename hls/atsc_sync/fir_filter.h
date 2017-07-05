/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_FIR_FILTER_H
#define INCLUDED_FILTER_FIR_FILTER_H

//#include <gnuradio/filter/api.h>
//#include <vector>
//#include <gnuradio/gr_complex.h>

#include <vector>
#include <complex>

namespace gr {
  namespace filter {
    namespace kernel {

      class fir_filter_fff
      {
      public:
	fir_filter_fff(int decimation,
		       const std::vector<float> &taps);
	~fir_filter_fff();

	void set_taps(const std::vector<float> &taps);
	void update_tap(float t, unsigned int index);
	std::vector<float> taps() const;
	unsigned int ntaps() const;

	float filter(const float input[]);
	void  filterN(float output[],
		      const float input[],
		      unsigned long n);
	void  filterNdec(float output[],
			 const float input[],
			 unsigned long n,
			 unsigned int decimate);

      protected:
	std::vector<float> d_taps;
	unsigned int  d_ntaps;
	float       **d_aligned_taps;
	float        *d_output;
	int           d_align;
	int           d_naligned;
      };

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FIR_FILTER_H */
