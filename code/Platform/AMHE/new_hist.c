void new_hist(double *old_hist,double *newHistOut,int *map,int binNum)
	//主要功能为旧直方图映射成新的直方图，old_hist为原直方图，newHistOut为新直方图，map为像素值映射表，binNum为bin数目
{
int i,j;
for(i=0;i<binNum;i++)                                //新直方图清零
newHistOut[i]=0;
for(j=0;j<binNum;j++)
newHistOut[map[j]]+=old_hist[j];   //生成新直方图
}