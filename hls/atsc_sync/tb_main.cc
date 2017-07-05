#include <stdio.h>
#include <stdlib.h>
#include <complex>
//#include "atsc_sync.h"
#include "atsc_sync_impl.h"

#define ITER 1

using namespace std;

int main()
{
	float in[COUNT] = {0};
	float out[COUNT] = {0};
	float acc_in[COUNT*ITER] = {0};
	float acc_out[COUNT*ITER] = {0};
	float golden;

	FILE *fp_r;

	fp_r = fopen("tb_input.txt", "r");
	if (fp_r == NULL)
	{
		printf("cannot open tb_input.txt");
		return 1;
	}
	for(int i = 0; i < COUNT*ITER; i++)
	{
		fscanf(fp_r, "%f", &acc_in[i]);
	}
	fclose(fp_r);

	for(int i = 0; i < ITER; i++)
	{
		for(int j = 0; j < COUNT; j++)
		{
			in[j] = acc_in[i*COUNT + j];
		}

		atsc_sync(in, out);

		for(int j = 0; j < COUNT; j++)
		{
			acc_out[i*COUNT + j] = out[j];
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
	for(int i = 0; i < COUNT*ITER; i++)
	{
		fscanf(fp_r, "%f", &golden);
		delta = fabs(acc_out[i]-golden);
		d_acc += delta;
		mag_acc += fabs(golden);
		if (delta > 0)
		{
//			printf("%d\tout=%.6f\tgolden=%.6f\tdelta=%.6f\n", \
					i, acc_out[i], golden, delta);
			fail++;
		}
//		else
//			printf("%d\tout=%.6f\tgolden=%.6f\n", \
									i, acc_out[i], golden);
	}
	fclose(fp_r);
	printf("\nAVG DELTA: %.20f\nACCURACY: %.20f%%\n\n", d_acc/(COUNT*ITER), 100 * (1-fabs((d_acc/mag_acc))));

	return 0;
}
