#include <stdio.h>
#include <stdlib.h>
#include "agc.h"

#define COUNT 128
#define RATE 0.00001
#define REFERENCE 4.0
#define GAIN 1.0
#define MAX_GAIN 65536

int main()
{
	axis_float in[COUNT];
	axis_float out[COUNT];
	float golden_out;

	FILE *fp_r;

	fp_r = fopen("tb_input.txt", "r");
	if (fp_r == NULL)
	{
		printf("cannot open tb_input.txt");
		return 1;
	}

	for(int i = 0; i < COUNT; i++)
	{
		fscanf(fp_r, "%f", &in[i].data);
	}
	fclose(fp_r);

	axis_float dummy;
	agc(dummy, dummy, RATE, 128, true);
	agc(dummy, dummy, REFERENCE, 129, true);
	agc(dummy, dummy, GAIN, 130, true);
	agc(dummy, dummy, MAX_GAIN, 131, true);

	for (int i = 0; i < COUNT; i++)
	{
		agc(in[i], out[i], 0, 0, false);
	}

	fp_r = fopen("tb_output.txt", "r");
	if (fp_r == NULL)
	{
		printf("cannot open tb_output.txt");
		return 1;
	}

	float delta=0; float d_acc=0; float mag_acc=0;
	int fail = 0;
	for(int i = 0; i < COUNT; i++)
	{
		fscanf(fp_r, "%f", &golden_out);
		delta = fabsf(out[i].data-golden_out);
		d_acc += delta;
		mag_acc += fabsf(golden_out);
		if (delta > 0)
		{
			printf("%d\tout=%.6f\tgolden_out=%.6f\tdelta=%.6f\n", \
					i, out[i].data, golden_out, delta);
			fail++;
		}
//		else
//			printf("%d\tout=%.6f\tgolden_out=%.6f\n", \
//									i, out[i].data, golden_out);
	}
	fclose(fp_r);
	printf("\nAVG DELTA: %.20f\nACCURACY: %.20f%%\n\n", d_acc/COUNT, 100 * (1-(d_acc/mag_acc)));

	return 0;
}
