#include <stdio.h>
#include <stdlib.h>
#include <complex>
#include "atsc_fpll.h"

#define ITER 3

// Mysterious gain on FPGA output
#define FPGA_GAIN 32767.00480546441
using namespace std;

int main()
{
	axis_float in[IN_SIZE], out[OUT_SIZE];
	complex<float> acc_in[ITER*OUT_SIZE] = {0};
	float acc_out[ITER*OUT_SIZE] = {0};
	float golden_out;

	FILE *fp_r;
	char s[64];
	char I[32], Q[32];

	fp_r = fopen("tb_input.txt", "r");
	if (fp_r == NULL)
	{
		printf("cannot open tb_input.txt");
		return 1;
	}

	for(int i = 0; i < ITER; i++)
	{
		for(int j = 0; j < IN_SIZE/2; j++) {
			fscanf(fp_r, "%s", s);
			sscanf(s,"%[^,],%[^,]", I, Q);
			acc_in[i*IN_SIZE/2+j].real() = atof(I);
			acc_in[i*IN_SIZE/2+j].imag() = atof(Q);
		}
	}
	fclose(fp_r);

	for(int i = 0; i < ITER; i++)
	{
		for(int j = 0; j < IN_SIZE; j+=2) {
			in[j].data = real(acc_in[i*IN_SIZE/2+j/2]);
			in[j+1].data = imag(acc_in[i*IN_SIZE/2+j/2]);
		}

		atsc_fpll(in, out);

		for(int j = 0; j < OUT_SIZE; j++) {
			acc_out[i*OUT_SIZE+j] = out[j].data*FPGA_GAIN;
		}
	}

	fp_r = fopen("tb_output.txt", "r");
	if (fp_r == NULL)
	{
		printf("cannot open tb_output.txt");
		return 1;
	}

	float delta=0; float d_acc=0; float mag_acc=0;
	int fail = 0;
	for(int i = 0; i < ITER*OUT_SIZE; i++)
	{
		fscanf(fp_r, "%f", &golden_out);
		delta = fabsf(acc_out[i]-golden_out);
		d_acc += delta;
		mag_acc += fabsf(golden_out);
		if (delta > .0001)
		{
			printf("%d\tout=%.6f\tgolden_out=%.6f\tdelta=%.6f\n", \
					i, acc_out[i], golden_out, delta);
			fail++;
		}
//		else
//			printf("%d\tout=%.6f\tgolden_out=%.6f\n", \
									i, acc_out[i], golden_out);
	}
		fclose(fp_r);
		printf("\nAVG DELTA: %.20f\nACCURACY: %.20f%%\n\n", d_acc/(ITER), 100 * (1-fabsf(d_acc/mag_acc)));

		return 0;
}
