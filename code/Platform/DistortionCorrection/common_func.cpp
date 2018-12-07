//#include "stdafx.h"

float MAX_CUSTOM(float a, float b)
{
    if(a>b)return a;
    else return b;
}
float MIN_CUSTOM(float a, float b)
{
    if(a>b)return b;
    else return a;
}

unsigned char clip(int indata)
{
    unsigned char pOut = indata;

    if(indata>255)pOut  =255;
    else if(indata<0)pOut  =0;

    return pOut;
}

