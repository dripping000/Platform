double hist_mean(double *hist,int low,int up)
	//主要功能为计算直方图对应的像素值均值，hist为直方图，low和up分别为直方图的下界和上界
{
double a=0,b=0;
int i;
for(i=low;i<=up;i++)
{
a+=hist[i]*i;
b+=hist[i];
}
return a/b;
}