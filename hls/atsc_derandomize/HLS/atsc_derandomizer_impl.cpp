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
 */

#include "atsc_derandomizer_impl.h"
#include "atsc_consts.h"



    atsc_derandomizer_impl::atsc_derandomizer_impl()
    {
      d_rand.reset();
    }

    void
    atsc_derandomizer_impl::work (const atsc_mpeg_packet_no_sync &in, atsc_mpeg_packet &out)
    {

        assert(in.pli.regular_seg_p());

        if (in.pli.first_regular_seg_p())
          d_rand.reset();

        d_rand.derandomize(out, in);

        // Check the pipeline info for error status and and set the
        // corresponding bit in transport packet header.

        if (in.pli.transport_error_p())
          out.data[1] |= MPEG_TRANSPORT_ERROR_BIT;
        else
          out.data[1] &= ~MPEG_TRANSPORT_ERROR_BIT;
    }

	void derandomize()
	{
	}
