#include <math.h>
#define max(a,b) (a>b)?a:b
#define min(a,b)  (a<=b)?a:b
void makeLUT(int img_Min,int img_Max,int NrBins,int *LUT)
	//主要功能为生成NrBins数目个颜色索引条，img_Min和img_Max为最小像素值和最大像素值，NrBins为bin的数目，LUT为颜色索引条
{
	int i;
	int cc=max(1,img_Min);
    int img_Max1=img_Max+cc-img_Min;
    int img_Min1=cc;
    int BinSize=(int)(floor(1+(double)(img_Max-img_Min)/NrBins));    //每个bin的跨度
    for( i=img_Min1;i<=img_Max1;i++)
    LUT[i-img_Min1]=(int)(floor((double)(i-img_Min1)/BinSize));
}