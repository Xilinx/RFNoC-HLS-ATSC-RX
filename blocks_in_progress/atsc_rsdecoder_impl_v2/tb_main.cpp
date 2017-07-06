#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>
#include "atsc_rs_decoder_impl.h"
#include <cstdio>

using namespace std;

extern void atsc_rsdecoder_impl(axis_cplx_in in[64], axis_cplx_out out[64]);

unsigned int in[64];
unsigned int out[64];
axis_cplx_in arr_in[64];
axis_cplx_out arr_out[64];

int main(void)
{
    FILE *file_r;
    FILE *file_w;
    int correct=0,incorrect=0;

    file_r = fopen("di_out_rs_in_uchar", "rb");
    file_w = fopen("rs_out_uchar", "rb");
    if (file_r == NULL)
    {
        printf("cannot open file 1");
        return 1;
    }
    if (file_w == NULL)
    {
        printf("cannot open file 2");
        return 1;
    }
    printf("Starting\n");
    for(int j=0; j<100; j++)
    {
        fread(&in, sizeof(unsigned int), 64, file_r);
        for(int i = 0; i < 64; i++)
        {
        	arr_in[i].data = in[i];
        }

        fread(&out, sizeof(unsigned int), 64, file_w);
        atsc_rsdecoder_impl(arr_in,arr_out);

        for(int i = 0; i<64; i++)
        {
            if(out[i]!=arr_out[i].data)
            {
                printf("\nError at %d %d  Expected:%d   Got:%d",j,i,out[i],arr_out[i].data);
                incorrect++;
                //while(1);
            }
            else
                correct++;
        }
    }
    printf("\nEnding    Correct %d    Incorrect %d    Percent %f",correct,incorrect,(float)(correct)/(float)(correct+incorrect));
    fclose(file_r);
    fclose(file_w);
    return 0;
}
