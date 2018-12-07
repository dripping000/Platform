double min(double *source,int low,int up)
	//主要功能为在有num数目个数据的source中去最小值
{
int i;
double fmin=source[low];
for(i=low;i<=up;i++)
{
if(source[i]<fmin)
fmin=source[i];
}
return fmin;
}