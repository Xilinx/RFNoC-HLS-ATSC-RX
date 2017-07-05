#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>
#include "atsc_types.h"
#include "atsc_derandomizer_impl.h"
#include <cstdio>

using namespace std;


int main(void)
{
    FILE *file_r;
    FILE *file_w;
    int correct=0,incorrect=0;

    file_r = fopen("der2dep_uchar", "rb");
    file_w = fopen("der2dep_uchar", "rb");
    atsc_mpeg_packet_no_sync in;
    atsc_mpeg_packet temp;
    atsc_mpeg_packet out;
    atsc_derandomizer_impl derandom;
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
    for(int j=0; j<154; j++)
    {
        fread(&in.pli._flags, sizeof(unsigned short), 1, file_r);
        fread(&in.pli._segno, sizeof(short), 1, file_r);
        fread(&in.data[0], sizeof(unsigned char), ATSC_MPEG_DATA_LENGTH, file_r);
        fread(&in._pad_[0], sizeof(unsigned char), 65, file_r);

        fread(&out.data[0], sizeof(unsigned char), ATSC_MPEG_DATA_LENGTH+1, file_w);
        fread(&out._pad_[0], sizeof(unsigned char), 68, file_w);
        derandom.work(in,temp);

        for(int i = 0; i<(ATSC_MPEG_DATA_LENGTH+1); i++)
        {
            if(out.data[i]!=temp.data[i])
            {
                printf("\nError at %d %d  Expected:%d   Got:%d",j,i,out.data[i],temp.data[i]);
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
