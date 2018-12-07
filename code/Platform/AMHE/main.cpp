#include <afxdlgs.h>
#include<stdio.h>
#include <stdlib.h>
#include "cv.h"
#include "highgui.h"
#include "math.h"
#include <string>
//#include <Windows.h>

extern "C" void AMHE_main_run(int *img,int m,int n,double c,int binNum,double *newHistOut,int *Map);

int main()
{   
	double contrast;
	printf("请输入对比度参数，增强时范围为[0,1.5]，模糊范围为[-1,0]，最优参数为1:");
	scanf("%lf",&contrast);
	if(!AfxWinInit(::GetModuleHandle(NULL),NULL,::GetCommandLine(),0))
	{
		return -1;
	}

	CString csBMP = "BMP Files(*.BMP)|*.BMP|";  
	CString csJPG = "JPEG Files(*.JPG)|*.JPG|";  
	CString csTIF = "TIF Files(*.TIF)|*.TIF|";  
	CString csPNG = "PNG Files(*.PNG)|*.PNG|";  
	CString csDIB = "DIB Files(*.DIB)|*.DIB|";  
	CString csPBM = "PBM Files(*.PBM)|*.PBM|";  
	CString csPGM = "PGM Files(*.PGM)|*.PGM|";  
	CString csPPM = "PPM Files(*.PPM)|*.PPM|";  
	CString csSR  = "SR  Files(*.SR) |*.SR|";  
	CString csRAS = "RAS Files(*.RAS)|*.RAS||";  

	CString csFilter = csJPG + csBMP + csTIF + csPNG + csDIB  
		+ csPBM + csPGM + csPPM + csSR + csRAS;  

	//CString name[]={"", "bmp" ,"jpg", "tif", "png", "dib",  
	//"pbm", "pgm", "ppm", "sr", "ras", ""};  

	CString strFileName;
	CString extname;
	CString filetitle;
	CString m_csFileName;
	CString contrast_name;
    const char *lpctStr1;
	// 文件对话框  
	CFileDialog FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, csFilter,NULL);  
	if (FileDlg.DoModal() == IDOK )                            
	{           
		m_csFileName= FileDlg.GetPathName();  
		//filename=FileDlg.GetFileName();
		extname=FileDlg.GetFileExt( );
		filetitle=FileDlg.GetFileTitle();
		lpctStr1 = (LPCTSTR)m_csFileName;   
	}   
	contrast_name.Format(_T("%.1lf"),contrast);
	//在d盘新建文件夹
	system("md D:\\test");                         
	//图像新名称
	CString new_name="D:\\test\\"+filetitle+"_"+contrast_name+"."+extname;    
	int i,j;
	int *img;
	IplImage *img_out;
	CvSize img_size; 
	//const char* imagename = "D:\\xin_51308073116470317722120.JPG";
	const char* imagename=lpctStr1;
	//加载图像
	IplImage *img1=cvLoadImage(imagename,CV_LOAD_IMAGE_COLOR);            
	if(!img1->imageData)
	{
		fprintf(stderr, "Can not load image %s\n", imagename);
		return -1;
	}
	//提取图像的R，G，B通道数据
	IplImage *redImage = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,1);  
	IplImage *greenImage = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,1);  
	IplImage *blueImage = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,1);  
	IplImage *Image = cvCreateImage(cvGetSize(img1),IPL_DEPTH_32F,3);  
	IplImage *Image1 = cvCreateImage(cvGetSize(img1),IPL_DEPTH_32F,3); 
	cvSplit(img1,redImage,greenImage,blueImage,NULL); 
	CvMat *r=cvCreateMat(img1->height,img1->width,CV_32FC1);
	CvMat *g=cvCreateMat(img1->height,img1->width,CV_32FC1);
	CvMat *b=cvCreateMat(img1->height,img1->width,CV_32FC1);
	CvMat *h=cvCreateMat(img1->height,img1->width,CV_32FC1);
	CvMat *bright=cvCreateMat(img1->height,img1->width,CV_32FC1);
	//转换数据到矩阵
	cvConvert(redImage,r);
	cvConvert(greenImage,g);
	cvConvert(blueImage,b);
	//计算图像亮度成分
	cvZero(h);
	cvAdd(r,g,h);
	cvAdd(h,b,h);
	cvConvertScale(h,bright,1.0/3.0,0);
	//CvMat *bright1=cvCloneMat( bright);
	//cvConvert(bright,Image);
	//cvConvertScale(bright1,bright1,1.0/255.0,0);
	cvNamedWindow("image", CV_WINDOW_AUTOSIZE); //创建窗口
	cvShowImage("image", img1); //显示图像
    //cvWaitKey(5000);
	img=(int *)malloc(sizeof(int)*(img1->width)*(img1->height));//分配输入图像空间
	for(i=0;i<img1->height;i++)
		for(j=0;j<img1->width;j++)
          *(img+i*img1->width+j) =(uchar)floor(*( bright->data.fl+i*img1->width+j));//把亮度图像输入图像空间
	img_size.width=img1->width;
	img_size.height=img1->height;
    img_out=cvCreateImage(img_size,IPL_DEPTH_8U,1);
	int binNum=256;
	int *Map=(int*)malloc(sizeof(int)*binNum);  //分配映射表空间
	double* newHistOut=(double*)malloc(sizeof(double)*binNum);
	//DWORD Begin = GetTickCount();
	AMHE_main_run(img,img1->width,img1->height,contrast,binNum,newHistOut,Map); //运行图像增强算法 ，同时得到图像映射表
	//DWORD Gap = GetTickCount() - Begin;
	for(i=0;i<img1->height;i++)
		for(j=0;j<img1->width;j++)
			*(img_out->imageData+i*img_out->widthStep+j)=(uchar)Map[img[i*img1->width+j]];//根据映射表得到输出图像
	CvMat *img3=cvCreateMat(img1->height,img1->width,CV_32FC1);
	CvMat *img4=cvCreateMat(img1->height,img1->width,CV_32FC1);
	cvConvert(img_out,img4);
	//cvConvertScale(img4,img4,1.0/255.0,0);
	//cvNamedWindow("image2", CV_WINDOW_AUTOSIZE); //创建窗口
	//cvShowImage("image2", img_out); //显示图像
	//cvWaitKey(10000);
	//根据亮度图像增强前后变化恢复彩色图像
	double a1,b1,c1;
	for(i=0;i<img1->height;i++)
		for(j=0;j<img1->width;j++)
			{
				 a1=floor(cvmGet(r,i,j)*cvmGet(img4,i,j)/cvmGet(bright,i,j));  
				 b1=floor(cvmGet(g,i,j)*cvmGet(img4,i,j)/cvmGet(bright,i,j));
				 c1=floor(cvmGet(b,i,j)*cvmGet(img4,i,j)/cvmGet(bright,i,j));
				 cvSet2D( Image, i, j, cvScalar( a1/255,b1/255,c1/255 ) );
				 cvSet2D( Image1, i, j, cvScalar( a1,b1, c1) );
		    }
	cvNamedWindow("image1", CV_WINDOW_AUTOSIZE); //创建窗口
	cvShowImage("image1", Image); //显示图像
	cvSaveImage((LPCTSTR)new_name,Image1);
	printf("新图片保存在D盘test文件中\n");
	if(cvWaitKey()==27)
		return 0;
	//system("pause");
	free(Map);
	free(newHistOut);
	free(img);
	cvReleaseImage(&img_out);
	cvReleaseImage(&img1);
	cvReleaseImage(&redImage);
	cvReleaseImage(&greenImage);
	cvReleaseImage(&blueImage);
	cvReleaseImage(&Image);
	cvReleaseImage(&Image1);
	cvReleaseMat(&r);
	cvReleaseMat(&g);
	cvReleaseMat(&b);
	cvReleaseMat(&img3);
	cvReleaseMat(&img4);
	cvReleaseMat(&bright);
	cvDestroyWindow("image");
	cvDestroyWindow("image1");
	return 0;
}