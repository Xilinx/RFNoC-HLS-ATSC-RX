#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>
#include "atsc_deinterleaver_impl.h"

using namespace std;

extern void atsc_deinterleaver_impl(axis_cplx in[64], axis_cplx out[64]);
axis_cplx arr_in[64];
axis_cplx arr_out[64];
axis_cplx arr_temp[64];
unsigned int arr[64];
int main(void)
{
    FILE *file_r;
    FILE *file_w;
    file_r = fopen("viterbi_out_di_in_uchar", "rb");
    file_w = fopen("di_out_rs_in_uchar", "rb");
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
//    for(int j=0; j<1170; j++)
    for(int j=0; j<31; j++)
    {
        fread(&arr[0], sizeof(unsigned int), 64, file_r);
        for(int i = 0; i < 64; i++)
        {
        	arr_in[i].data = arr[i];
        }
        fread(&arr[0], sizeof(unsigned int), 64, file_w);
        for(int i = 0; i < 64; i++)
        {
        	arr_out[i].data = arr[i];
        }
//        printf("segment:%d   line:%d   in flags:%d   in seg no:%d    ",j,line-255,in.data.pli._flags,in.data.pli._segno);
        atsc_deinterleaver_impl(arr_in,arr_temp);
//        printf("temp flags:%d   temp seg no:%d\n",temp.data.pli._flags,temp.data.pli._segno);

        for(int i = 0; i<64; i++)
        {
            if(arr_out[i].data!=arr_temp[i].data)
            {
                printf("\nError at j:%d i:%d   Expected:%x   Got:%x   %x",j,i,arr_out[i].data,arr_temp[i].data,arr_in[i].data);
                //while(1);
            }
        }
    }
    printf("\nDeinterleaver Done.");
    fclose(file_r);
    fclose(file_w);
}
