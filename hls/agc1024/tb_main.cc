#include <stdio.h>
#include <stdlib.h>
#include "agc.h"

#define ITER 3

int main()
{
	axis_float in[SIZE];
	axis_float out[SIZE];
	float acc_in[ITER*SIZE], acc_out[ITER*SIZE];
	float golden_out;

	FILE *fp_r;

	fp_r = fopen("tb_input.txt", "r");
	if (fp_r == NULL)
	{
		printf("cannot open tb_input.txt");
		return 1;
	}

	for(int i = 0; i < ITER; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			fscanf(fp_r, "%f", &acc_in[i*SIZE+j]);
		}
	}
	fclose(fp_r);

	for(int i = 0; i < ITER; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			in[j].data = acc_in[i*SIZE+j];
		}

		agc(in, out);

		for (int j = 0; j < SIZE; j++)
		{
			acc_out[i*SIZE+j] = out[j].data;
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
	for(int i = 0; i < ITER*SIZE; i++)
	{
		fscanf(fp_r, "%f", &golden_out);
		delta = fabsf(acc_out[i]-golden_out);
		d_acc += delta;
		mag_acc += fabsf(golden_out);
		if (delta > 0)
		{
			printf("%d\tout=%.6f\tgolden_out=%.6f\tdelta=%.6f\n", \
					i, acc_out[i], golden_out, delta);
			fail++;
		}
		else
			printf("%d\tout=%.6f\tgolden_out=%.6f\n", \
									i, acc_out[i], golden_out);
	}
	fclose(fp_r);
	printf("\nAVG DELTA: %.20f\nACCURACY: %.20f%%\n\n", d_acc/SIZE, 100 * (1-(d_acc/mag_acc)));

	return 0;
}
