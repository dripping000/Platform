double max(double *source,int low,int up)
	//主要功能为在有num数目个数据的source中去最小值
{
int i;
double fmax=source[low];
for(i=low;i<=up;i++)
{
if(source[i]>fmax)
fmax=source[i];
}
return fmax;
}