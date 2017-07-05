#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <stdlib.h>
#include "atsc_rx_filter.h"

#define ITER 2 //1306 max
using std::complex;

int main()
{

	complex<float> in, golden_out, dut_out;

    FILE *file_i, *file_o, *file_dut;
    file_i = fopen("tb_in", "rb");
    file_o = fopen("tb_out", "rb");
    file_dut = fopen("hls_out", "wb");

	axis_float input[IN_SIZE], output[OUT_SIZE];
	int n_out;
    int acc = 0;
    float delta=0; float d_acc=0; float mag_acc=0; float outlier_acc=0; int ol_ct=0;
    float thresh = .001, max = 0;
	for(int i = 0; i < ITER; i++) {
		for(int j = 0; j < IN_SIZE; j+=2) {
			fread(&in, sizeof(complex<float>), 1, file_i);
			input[j].data = real(in);
			input[j+1].data = imag(in);
		}

		atsc_rx_filter(input, output);

		n_out = 0;
		for(int j = 0; j < OUT_SIZE; j++) {
			n_out++;
			if(output[j].last == true)
				break;
		}
		for(int j=0; j < n_out; j+=2) {
			dut_out.real()=output[j].data;
			dut_out.imag()=output[j+1].data;
			fwrite(&dut_out, sizeof(complex<float>), 1, file_dut);
			fread(&golden_out, sizeof(complex<float>), 1, file_o);
			delta = abs(dut_out-golden_out);
			if (delta > max)
				max = delta;
			d_acc += delta;
			mag_acc += abs(golden_out);
			if (delta > thresh || !(delta<0||delta==0||delta>0))//isnan(delta))
			{
				outlier_acc += delta;
				ol_ct++;
//				printf("%d\tout=(%.6f,%.6f)\tgolden_out=(%.6f,%.6f)  \tdelta=%.6f\n", (acc+j)/2, \
						real(dut_out), imag(dut_out), \
						real(golden_out), imag(golden_out), delta);
			}
//			printf("%d\tout=(%.6f,%.6f)\tgolden_out=(%.6f,%.6f)\tdelta=%.6f\n", (acc+j)/2, \
						real(dut_out), imag(dut_out), \
						real(golden_out), imag(golden_out),delta);
		}
		acc += n_out;
	}
    n_out = acc;
    fclose(file_i);
    fclose(file_o);
    fclose(file_dut);

//    file_dut = fopen("hls_out", "rb");
//	for(int i=0;i<n_out;i++){
//		fread(&dut_out, sizeof(complex<float>), 1, file_dut);
//		printf("%d\tdut_out={%f, %f}\n",i,real(dut_out),imag(dut_out));
//	}
//    fclose(file_dut);

	printf("\nn_in = %d\tn_out = %d\t(complex ordered pairs)\n",ITER*IN_SIZE/2,n_out/2);
	printf("\nAVG DELTA: %.20f\nMAX DELTA: %.20f\nACCURACY: %.20f%%\n", d_acc/(n_out/2), max, 100 * (1-(d_acc/mag_acc)));
	printf("DELTA>%f: ACCUM=%.6f | CT=%d | AVG=%f\n\n", thresh, outlier_acc, ol_ct, outlier_acc/ol_ct);

	return 0;
}
