#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <stdlib.h>
#include "dc_blocker.h"

#define ITER 1//175064

using std::complex;

int main()
{
	float in, golden_out, dut_out;

    FILE *file_i, *file_o, *file_dut;
    file_i = fopen("tb_in", "rb");
    file_o = fopen("tb_out_4096_long", "rb");
    file_dut = fopen("hls_out", "wb");

	axis_float input[D], output[D];
    float delta=0; float d_acc=0; float mag_acc=0; float outlier_acc=0; int ol_ct=0;
    float thresh = .0001, max = 0;
	for(int i = 0; i < ITER; i++) {
		for(int j = 0; j < D; j++) {
			fread(&in, sizeof(float), 1, file_i);
			input[j].data = in;
		}

		dc_blocker(input, output);

		for(int j=0; j < D; j++) {
			fwrite(&output[j].data, sizeof(float), 1, file_dut);
			dut_out=output[j].data;
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
//				printf("%d\tout= %.6f\tgolden_out= %.6f\tdelta=%.6f\n", i*D+j, \
						dut_out, golden_out, delta);
			}
//			printf("%d\tout= %.6f\tgolden_out= %.6f\tdelta=%.6f\n", i*D+j, \
					dut_out, golden_out, delta);
		}
	}
    fclose(file_i);
    fclose(file_o);

    fclose(file_dut);
    file_dut = fopen("hls_out", "rb");
	for(int i=0;i<D*ITER;i++){
		fread(&dut_out, sizeof(float), 1, file_dut);
//		printf("%.20f,\n",i,dut_out);
	}
    fclose(file_dut);

	printf("\nProcessed %d samples\n",D*ITER);
	printf("\nAVG DELTA: %.20f\nMAX DELTA: %.20f\nACCURACY: %.20f%%\n", d_acc/(D*ITER), max, 100 * (1-(d_acc/mag_acc)));
	printf("DELTA>%.4f: ACCUM=%.6f | CT=%d | AVG=%f\n\n", thresh, outlier_acc, ol_ct, outlier_acc/ol_ct);

	return 0;
}
