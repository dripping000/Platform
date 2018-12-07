#include <math.h>
extern double max(int *source,int low,int up);                                            //串中求最大值和最小值
extern double min(int *source,int low,int up);
extern double hist_mean(double *hist,int low,int up);                              

//_inline double *sring_cut(double *string,int begin,int end)                               //串截取
//{
	//return &string[begin];
//}

void modified_hist(double *hist,int low,int up,double c,double *img_hist_AM)
	//功能为直方图修正，hist为输入直方图，low和up为直方图对应的下界和上届，c为对比度参数，img_hist_AM为修正的直方图
{
int num=up-low+1;
double p_min=min(hist,low,up);                                                          
double p_max=max(hist,low,up);
double p_mid=(p_min+p_max)/2;
double xm=hist_mean(hist,low,up);                      //直方图对应的均值
int Xm=(int)floor(xm);
double xml=hist_mean(hist,low,Xm);                     //直方图低区的均值
double xmu=hist_mean(hist,Xm+1,up);                    //直方图高区的均值
double alph[256]={0};                                                
int i;
for (i=low;i<=up;i++)
{
if(i<=Xm)
alph[i]=(Xm-xml)/(xmu-xml);
else  alph[i]=(xmu-Xm)/(xmu-xml);
alph[i]*=c;
}
for (i=low;i<=up;i++)
{
if(hist[i]>p_mid)
img_hist_AM[i-low]=p_mid+alph[i]*(hist[i]-p_mid)*(hist[i]-p_mid)/(p_max-p_mid);       //直方图修正           
else img_hist_AM[i-low]=p_mid-alph[i]*(p_mid-hist[i])*(p_mid-hist[i])/(p_mid-p_min);  
}
}

