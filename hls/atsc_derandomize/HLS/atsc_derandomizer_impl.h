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

#ifndef INCLUDED_DTV_ATSC_DERANDOMIZER_IMPL_H
#define INCLUDED_DTV_ATSC_DERANDOMIZER_IMPL_H

#include "atsc_randomize.h"


    class atsc_derandomizer_impl
    {
    private:
      atsc_randomize d_rand;

    public:
      atsc_derandomizer_impl();

      void work(const atsc_mpeg_packet_no_sync &in, atsc_mpeg_packet &out);
    };


#endif /* INCLUDED_DTV_ATSC_DERANDOMIZER_IMPL_H */
