/* -*- c++ -*- */
/*
 * Copyright 2002, 2014 Free Software Foundation, Inc.
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

#include "atsc_single_viterbi.h"
#include <math.h>


    /* was_sent is a table of what symbol we get given what bit pair
       was sent and what state we where in [state][pair] */
    const int d_was_sent[4][4] = {
      {0,2,4,6},
      {0,2,4,6},
      {1,3,5,7},
      {1,3,5,7},
    };

    /* transition_table is a table of what state we were in
       given current state and bit pair sent [state][pair] */
    const int d_transition_table[4][4] = {
      {0,2,0,2},
      {2,0,2,0},
      {1,3,1,3},
      {3,1,3,1},
    };

    char
    decode1(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode2(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode3(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode4(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode5(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode6(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode7(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode8(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode9(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode10(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode11(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }

    char
    decode12(float input)
    {
	  static float d_path_metrics [2][4];
	  static unsigned long long d_traceback [2][4];
	  static unsigned char d_phase;
	  static int d_post_coder_state;
	  static float d_best_state_metric;
      unsigned int best_state = 0;
      //float best_state_metric = 100000;
      d_best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { fabsf( input + 7 ), fabsf( input + 5 ),
                             fabsf( input + 3 ), fabsf( input + 1 ),
                             fabsf( input - 1 ), fabsf( input - 3 ),
                             fabsf( input - 5 ), fabsf( input - 7 ) };

      /* We start by iterating over all possible states */
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
      // 0 state
        int min_metric_symb = 0;
        float min_metric = distances[d_was_sent[0][0]] +
          d_path_metrics[d_phase][d_transition_table[0][0]];

        if( (distances[d_was_sent[0][1]] +
             d_path_metrics[d_phase][d_transition_table[0][1]]) < min_metric) {
          min_metric = distances[d_was_sent[0][1]] +
            d_path_metrics[d_phase][d_transition_table[0][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[0][2]] +
             d_path_metrics[d_phase][d_transition_table[0][2]]) < min_metric) {
          min_metric = distances[d_was_sent[0][2]] +
            d_path_metrics[d_phase][d_transition_table[0][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[0][3]] +
             d_path_metrics[d_phase][d_transition_table[0][3]]) < min_metric) {
          min_metric = distances[d_was_sent[0][3]] +
            d_path_metrics[d_phase][d_transition_table[0][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][0] = min_metric;
        d_traceback[d_phase^1][0] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[0][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 0;
        }

        // 1 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[1][0]] +
          d_path_metrics[d_phase][d_transition_table[1][0]];

        if( (distances[d_was_sent[1][1]] +
             d_path_metrics[d_phase][d_transition_table[1][1]]) < min_metric) {
          min_metric = distances[d_was_sent[1][1]] +
            d_path_metrics[d_phase][d_transition_table[1][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[1][2]] +
             d_path_metrics[d_phase][d_transition_table[1][2]]) < min_metric) {
          min_metric = distances[d_was_sent[1][2]] +
            d_path_metrics[d_phase][d_transition_table[1][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[1][3]] +
             d_path_metrics[d_phase][d_transition_table[1][3]]) < min_metric) {
          min_metric = distances[d_was_sent[1][3]] +
            d_path_metrics[d_phase][d_transition_table[1][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][1] = min_metric;
        d_traceback[d_phase^1][1] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[1][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 1;
        }

        // 2 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[2][0]] +
          d_path_metrics[d_phase][d_transition_table[2][0]];

        if( (distances[d_was_sent[2][1]] +
             d_path_metrics[d_phase][d_transition_table[2][1]]) < min_metric) {
          min_metric = distances[d_was_sent[2][1]] +
            d_path_metrics[d_phase][d_transition_table[2][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[2][2]] +
             d_path_metrics[d_phase][d_transition_table[2][2]]) < min_metric) {
          min_metric = distances[d_was_sent[2][2]] +
            d_path_metrics[d_phase][d_transition_table[2][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[2][3]] +
             d_path_metrics[d_phase][d_transition_table[2][3]]) < min_metric) {
          min_metric = distances[d_was_sent[2][3]] +
            d_path_metrics[d_phase][d_transition_table[2][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][2] = min_metric;
        d_traceback[d_phase^1][2] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[2][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 2;
        }

        // 3 state
        min_metric_symb = 0;
        min_metric = distances[d_was_sent[3][0]] +
          d_path_metrics[d_phase][d_transition_table[3][0]];

        if( (distances[d_was_sent[3][1]] +
             d_path_metrics[d_phase][d_transition_table[3][1]]) < min_metric) {
          min_metric = distances[d_was_sent[3][1]] +
            d_path_metrics[d_phase][d_transition_table[3][1]];
          min_metric_symb = 1;
        }
        if( (distances[d_was_sent[3][2]] +
             d_path_metrics[d_phase][d_transition_table[3][2]]) < min_metric) {
          min_metric = distances[d_was_sent[3][2]] +
            d_path_metrics[d_phase][d_transition_table[3][2]];
          min_metric_symb = 2;
        }
        if( (distances[d_was_sent[3][3]] +
             d_path_metrics[d_phase][d_transition_table[3][3]]) < min_metric) {
          min_metric = distances[d_was_sent[3][3]] +
            d_path_metrics[d_phase][d_transition_table[3][3]];
          min_metric_symb = 3;
        }

        d_path_metrics[d_phase^1][3] = min_metric;
        d_traceback[d_phase^1][3] = (((unsigned long long)min_metric_symb) << 62) |
          (d_traceback[d_phase][d_transition_table[3][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= d_best_state_metric) {
          d_best_state_metric = min_metric;
          best_state = 3;
        }

      if(d_best_state_metric > 10000) {
          d_path_metrics[d_phase^1][0] -= d_best_state_metric;
          d_path_metrics[d_phase^1][1] -= d_best_state_metric;
          d_path_metrics[d_phase^1][2] -= d_best_state_metric;
          d_path_metrics[d_phase^1][3] -= d_best_state_metric;
      }
      d_phase ^= 1;

      int y2 = (0x2 & d_traceback[d_phase][best_state]) >> 1;
      int x2 = y2 ^ d_post_coder_state;
      d_post_coder_state = y2;

      return ( x2 << 1 ) | (0x1 & d_traceback[d_phase][best_state]);
    }
