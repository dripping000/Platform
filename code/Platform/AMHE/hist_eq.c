#include <stdlib.h>
#include <math.h>
void hist_eq(double *hist_AM,int low,int up,int *eq_index)
	//主要功能为直方图均衡化，hist_AM是直方图，low和up为直方图对应的下界和上界，eq_index为生成的像素值索引
{
int num=up-low+1;
double *img_culmutive_hist;                                                 //积分直方图
int i;
img_culmutive_hist=(double*)malloc(sizeof(double)*num);
img_culmutive_hist[0]=hist_AM[0];
for(i=1;i<num;i++)
 img_culmutive_hist[i] =img_culmutive_hist[i-1]+hist_AM[i];                 //生成积分直方图
for(i=0;i<num;i++)
 {
 img_culmutive_hist[i] =img_culmutive_hist[i]/img_culmutive_hist[num-1];    //积分直方图归一化
 eq_index[i]=floor(img_culmutive_hist[i]*(up-low)+low);                     //生成像素值索引
 }
free(img_culmutive_hist);
}