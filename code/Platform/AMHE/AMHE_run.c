#include <stdlib.h>
extern int OstuCompute(double *hist,int binNum);
extern void modified_hist(double *hist,int low,int up,double c,double *img_hist_AM);
extern void hist_eq(double *hist_AM,int low,int up,int *eq_index);
extern void new_hist(double *old_hist,double *newHistOut,int *map,int binNum);
void AMHE_run(int *histIN,int binNum, double c, double *newHistOut,int *Map,int m,int n)
	//histIN为输入直方图，binNum为bin数目，c为对比度，newHistOut为输出直方图，Map为映射表，m和n分别为图像的列数和行数
{  
   double *low_hist_AM;                                     //暗区修正后的直方图
   double *up_hist_AM;                                      //亮区修正后的直方图
   int *low_eq_index;                                       //暗区均衡化后的颜色索引
   int *up_eq_index;                                        //亮区均衡化后的颜色索引
   int i;
   int low_num=0;
   int up_num=0;
   int level;
   double *hist;
   hist=(double*)malloc(sizeof(double)*binNum);
   for(i=0;i<binNum;i++)
	   hist[i]=(double)histIN[i]/(m*n);
	level=OstuCompute(hist,binNum);                            //otsu阈值提取
   for(i=0;i<=level;i++)                                        //直方图分别归一化
	   low_num=low_num+histIN[i];
   for(i=0;i<=level;i++)
	   hist[i]=(double)histIN[i]/low_num;
   for(i=level+1;i<binNum;i++)
	   up_num=up_num+histIN[i];
   for(i=level+1;i<binNum;i++)
	    hist[i]=(double)histIN[i]/up_num;
   low_hist_AM=(double*)malloc(sizeof(double)*(level+1));            
   modified_hist(hist,0,level,c,low_hist_AM);               //暗区直方图修正 
   up_hist_AM=(double*)malloc(sizeof(double)*(binNum-level-1));
   modified_hist(hist,level+1,binNum-1,c,up_hist_AM);       //亮区直方图修正
   low_eq_index=(int*)malloc(sizeof(int)*(level+1));        //暗区和亮区均衡化后颜色索引
   up_eq_index=(int*)malloc(sizeof(int)*(binNum-level-1)); 
   hist_eq(low_hist_AM,0,level,low_eq_index);               //对暗区和亮区分别均衡化
   hist_eq(up_hist_AM,level+1,binNum-1,up_eq_index);
   for( i=0;i<=level;i++)                                  //生成映射表
	   {
		 if(low_eq_index[i]>=0)
         Map[i]=low_eq_index[i];
		 else Map[i]=0;
        } 
   for(i=0;i<binNum-level-1;i++)
	      {
		  if(up_eq_index[i]<binNum)
          Map[i+level+1]=up_eq_index[i];
		  else Map[i+level+1]=255;
          } 
   new_hist(hist,newHistOut,Map,binNum);                  //生成输出直方图
   free(hist);
   free(low_eq_index);
   free(up_eq_index);
   free(low_hist_AM);
   free(up_hist_AM);
}