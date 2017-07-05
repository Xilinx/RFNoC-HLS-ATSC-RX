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

#define D_MAX 4096

struct axis_float {
	float data;
    ap_uint<1> last;
};

void dc_blocker(axis_float in[D_MAX], axis_float out[D_MAX], unsigned int set_data, unsigned char set_addr, bool set_stb);

    class moving_averager_f
    {
    public:
	  float d_delay_line[D_MAX-1];

      moving_averager_f();
      ~moving_averager_f();

      float filter(float x);
      float delayed_sig() { return d_out; }

    private:
      int d_length;
      float d_out, d_out_d1, d_out_d2;
//      float d_delay_line[D-1];
    };

    class dc_blocker_ff_impl //: public dc_blocker_ff
    {
    private:
      int d_length;
//      moving_averager_f d_ma_0;
//      moving_averager_f d_ma_1;
//      moving_averager_f d_ma_2;
//      moving_averager_f d_ma_3;
//      float d_delay_line[D-1];

    public:
      moving_averager_f d_ma_0;
      moving_averager_f d_ma_1;
      moving_averager_f d_ma_2;
      moving_averager_f d_ma_3;
      float d_delay_line[D_MAX-1];

      dc_blocker_ff_impl();

      ~dc_blocker_ff_impl();

      int group_delay();

      void work(float *in,
    		    float *out);
    };
