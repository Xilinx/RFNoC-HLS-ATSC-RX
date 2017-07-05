#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>
#include "atsc_depad_impl.h"
#include <cstdio>

using namespace std;

extern void atsc_depad_impl(axis_cplx_in in[INPUT_SIZE], axis_cplx_out out[OUTPUT_SIZE]);

int main(void)
{
    FILE *file_r;
    FILE *file_w;
    int correct=0,incorrect=0;
    unsigned int arr_in[256],arr_out[188];

    file_r = fopen("der2dep_uchar", "rb");
    file_w = fopen("dep_out_uchar", "rb");
    axis_cplx_in in[256];
    axis_cplx_out temp[188];
    axis_cplx_out out[188];
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
        fread(&arr_in[0], sizeof(unsigned int), INPUT_SIZE, file_r);
        fread(&arr_out[0], sizeof(unsigned int), OUTPUT_SIZE, file_w);
        for(int i=0;i<INPUT_SIZE;i++)
        	in[i].arr_data = arr_in[i];
        for(int i=0;i<OUTPUT_SIZE;i++)
        	out[i].arr_data = arr_out[i];
        atsc_depad_impl(in,temp);

        for(int i = 0; i<(OUTPUT_SIZE); i++)
        {
            if(out[i].arr_data!=temp[i].arr_data)
            {
                printf("\nError at %d %d  Expected:%d   Got:%d",j,i,out[i].arr_data,temp[i].arr_data);
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
