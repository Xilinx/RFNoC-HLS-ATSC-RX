#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <stdlib.h>
#include "atsc_rx_filter_fpll.h"

#define ITER 2 //1621 max

// Mysterious gain on FPGA output
#define FPGA_GAIN 32767.00480546441

using std::complex;

int main()
{
	complex<float> in;
	float golden_out, dut_out;

    FILE *file_i, *file_o, *file_dut;
    file_i = fopen("tb_in", "rb");
    file_o = fopen("tb_out", "rb");
    file_dut = fopen("hls_out", "wb");

	axis_float input[IN_SIZE], output[OUT_SIZE];
	int n_out;
    int acc = 0;
    float delta=0; float d_acc=0; float mag_acc=0; float outlier_acc=0; int ol_ct=0;
    float thresh = .0001, max = 0;
	for(int i = 0; i < ITER; i++) {
		for(int j = 0; j < IN_SIZE; j+=2) {
			fread(&in, sizeof(complex<float>), 1, file_i);
			input[j].data = real(in);
			input[j+1].data = imag(in);
		}

		atsc_rx_filter_fpll(input, output);

		n_out = 0;
		for(int j = 0; j < OUT_SIZE; j++) {
			n_out++;
			if(output[j].last == true)
				break;
		}

		for(int j=0; j < n_out; j++) {
//			fwrite(&output[j].data*FPGA_GAIN, sizeof(float), 1, file_dut);
			dut_out=output[j].data*FPGA_GAIN;
			fread(&golden_out, sizeof(float), 1, file_o);
			delta = abs(dut_out-golden_out);
			if (delta > max)
				max = delta;
			d_acc += delta;
			mag_acc += abs(golden_out);
			if (delta > thresh || isnan(delta))
			{
				outlier_acc += delta;
				ol_ct++;
				printf("%d\tout= %.6f\tgolden_out= %.6f\tdelta=%.6f\n", acc+j, \
						dut_out, golden_out, delta);
			}
//			printf("%d\tout= %.6f\tgolden_out= %.6f\tdelta=%.6f\n", acc+j, \
					dut_out, golden_out, delta);
		}
		acc += n_out;
	}
    n_out = acc;
    fclose(file_i);
    fclose(file_o);

//    fclose(file_dut);
//    file_dut = fopen("hls_out", "rb");
//	for(int i=0;i<OUT_SIZE;i++){
//		fread(&dut_out, sizeof(float), 1, file_dut);
//		printf("%d\tdut_out={%f, %f}\n",i,dut_out);
//	}
//    fclose(file_dut);

	printf("\nn_in = %d\tn_out = %d\t\n",ITER*IN_SIZE,n_out);
	printf("\nAVG DELTA: %.20f\nMAX DELTA: %.20f\nACCURACY: %.20f%%\n", d_acc/(n_out), max, 100 * (1-(d_acc/mag_acc)));
	printf("DELTA>%.4f: ACCUM=%.6f | CT=%d | AVG=%f\n\n", thresh, outlier_acc, ol_ct, outlier_acc/ol_ct);

	return 0;
}
