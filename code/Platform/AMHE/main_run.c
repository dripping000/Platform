#include <stdlib.h>
extern void makeLUT(int img_Min,int img_Max,int NrBins,int *LUT);
extern void img_base(int *img,int m,int n,int *basebin,int bin_num,int *baseimg);
extern void img_hist(int *img,int m,int n,int binnum,double *hist);
extern void AMHE_run(double *histIN,int binNum, double c, double *newHistOut,int *Map);
void AMHE_main_run(int *img,int m,int n,double c,int binNum,double *newHistOut,int *Map)  
	//img为输入图像，m为图像列数，n为图像行数，binNum为图像bin数目，newHistOut为输出直方图，Map为图像映射表
{
	int i;
	int *basebin;                                       //图像数据转换到指定bin数的像素值索引
	int *baseimg;                                       //指定bin数目转换后的图像
	int *hist;                                       //输入图像直方图                              
    basebin =(int*)malloc(sizeof(int)*256);   
    makeLUT(0,255,binNum,basebin);                         //生成256个像素值索引
    baseimg=(int*)malloc(sizeof(int)*m*n);        
    img_base(img,m,n,basebin,binNum,baseimg);           //生成bin数目的图像
    hist=(int*)malloc(sizeof(int)*binNum);   
	for(i=0;i<binNum;i++)                               //图像直方图初始化为0
		hist[i]=0;
    img_hist(baseimg,m,n,binNum,hist);                  //根据图像生成直方图
    AMHE_run(hist,binNum,c,newHistOut,Map,m,n);           //生成输出直方图和映射表
    free(basebin);
    free(baseimg);
	free(hist);

}