/* -*- c++ -*- */
/*
 * Copyright 2013, 2014 Free Software Foundation, Inc.
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <volk/constants.h>

char*
volk_prefix()
{
  return "/home/switchlanez/rfnoc";
}

char*
volk_version()
{
  return "1.3";
}

char*
volk_c_compiler()
{
  return "cc (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609 \nCopyright (C) 2015 Free Software Foundation, Inc. \nThis is free software see the source for copying conditions.  There is NO \nwarranty not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
}

char*
volk_compiler_flags()
{
  return "/usr/bin/cc:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall \n/usr/bin/c++:::-O2 -g -DNDEBUG  -std=c++98 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall \ngeneric:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall  \nsse2_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 \nsse3_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 -msse3 \nssse3_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 -msse3 -mssse3 \nsse4_a_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 -msse3 -msse4a -mpopcnt \nsse4_1_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 \nsse4_2_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt \navx_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt -mavx \navx2_64_mmx:::GNU:::-O2 -g -DNDEBUG  -std=gnu99 -fvisibility=hidden -Wsign-compare -Wall -Wno-uninitialized -Wall -m64 -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt -mavx -mfma -mavx2";
}

char*
volk_available_machines()
{
  return "generic;sse2_64_mmx;sse3_64_mmx;ssse3_64_mmx;sse4_a_64_mmx;sse4_1_64_mmx;sse4_2_64_mmx;avx_64_mmx;avx2_64_mmx";
}
