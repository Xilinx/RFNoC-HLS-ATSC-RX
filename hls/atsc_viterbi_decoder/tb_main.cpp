#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>
#include "atsc_types.h"
#include "atsc_viterbi_impl.h"
#include <cstdio>

using namespace std;

extern void atsc_viterbi_impl(axis_cplx_in in[12*1024], axis_cplx_out out[12*64]);

float in_tb[12*1024];
unsigned int out_tb[12*64];
axis_cplx_in temp_in[12*1024];
axis_cplx_out temp_out[12*256];
atsc_mpeg_packet_rs_encoded viterbi_out_tb[12];
int main(void)
{
    FILE *file_r;
    FILE *file_w;
    int correct=0,incorrect=0;
    file_r = fopen("viterbi_in_f", "rb");
    file_w = fopen("viterbi_out_di_in_uchar", "rb");
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
    for(int j=0; j<4; j++)
    {
        for(int i=0; i<12; i++)
        {
            fread(&in_tb[i*1024], sizeof(float), 1024, file_r);
        }

        for(int i=0; i<12; i++)
        {
            fread(&out_tb[i*64], sizeof(unsigned int), 64, file_w);
        }
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

        for(int i=0;i<(12*1024);i++)
        {
       		temp_in[i].data = in_tb[i];
        }

        atsc_viterbi_impl(&temp_in[0],&temp_out[0]);

        for(int k=0; k<(12*64); k++)
        {
            if(out_tb[k]!=temp_out[k].data)
            {
                printf("\nError at %d  %d  Expected:%d   Got:%d",j,k,out_tb[k],temp_out[k].data);
                incorrect++;
            }
            else
                correct++;
        }
    }
    printf("\nEnding    Correct %d    Incorrect %d    Percent %f",correct,incorrect,(float)(correct)/(float)(correct+incorrect));
    fclose(file_r);
    fclose(file_w);
}
