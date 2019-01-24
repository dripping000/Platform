#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <malloc.h>
#include "ISP_Alglog.h"
#include <stdlib.h>
#include <math.h>
#include "CPUBC_VS.h"
#include <pthread.h>
#include <arm_neon.h>
#include <sys/prctl.h>

/////////////////////////////////////////////////////////////////////////////////////////
//#define VSTHRE12	//define to run in 12 threads. if not defined, run in 8 threads
//#define CPUCOMBINE

//#define VS_RESOLUTION_2560_1440_TO_1920_1080	//the resolution set here
#define VS_RESOLUTION_2048_1536_TO_1920_1080
/////////////////////////////////////////////////////////////////////////////////////////


#ifdef VS_RESOLUTION_2560_1440_TO_1920_1080
	#define VS_PITCHY 2560
	#define VS_PITCHYADD1 2561
#endif
#ifdef VS_RESOLUTION_2048_1536_TO_1920_1080
	#define VS_PITCHY 2048
	#define VS_PITCHYADD1 2049
#endif


#define CLIP(x) ((x)<0?0:(x)>255?255:(x))

unsigned char* pSrcY;
unsigned char* pSrcU;

unsigned char* VSY = NULL;
unsigned char* VSUV = NULL;
unsigned int* coordVSYY = NULL;
unsigned int* coordVSYY2 = NULL;
unsigned int* coordVSUUV = NULL;
unsigned char* VScoordYYFXYCHAR = NULL;
unsigned char* VScoordYYFXYCHAR2 = NULL;
unsigned char* VScoordWEIGHT = NULL;

float* coordVSY = NULL;
//float* coordVSY2 = NULL;
unsigned int* coordVSUV = NULL;

pthread_t g_hThread1;
pthread_t g_hThread2;
pthread_t g_hThread3;
pthread_t g_hThread4;
pthread_t g_hThread5;
pthread_t g_hThread6;
pthread_t g_hThread7;
pthread_t g_hThread8;
pthread_t g_hThread9;
pthread_t g_hThread10;
pthread_t g_hThread11;

int g1_exit_flag;
int g2_exit_flag;
int g3_exit_flag;
int g4_exit_flag;
int g5_exit_flag;
int g6_exit_flag;
int g7_exit_flag;
int g8_exit_flag;
int g9_exit_flag;
int g10_exit_flag;
int g11_exit_flag;

int TLDDataThread1Bool = 0;
int TLDDataThread2Bool = 0;
int TLDDataThread3Bool = 0;
int TLDDataThread4Bool = 0;
int TLDDataThread5Bool = 0;
int TLDDataThread6Bool = 0;
int TLDDataThread7Bool = 0;
int TLDDataThread8Bool = 0;
int TLDDataThread9Bool = 0;
int TLDDataThread10Bool = 0;
int TLDDataThread11Bool = 0;

int TLDDataThread1OUTBool = 0;
int TLDDataThread2OUTBool = 0;
int TLDDataThread3OUTBool = 0;
int TLDDataThread4OUTBool = 0;
int TLDDataThread5OUTBool = 0;
int TLDDataThread6OUTBool = 0;
int TLDDataThread7OUTBool = 0;
int TLDDataThread8OUTBool = 0;
int TLDDataThread9OUTBool = 0;
int TLDDataThread10OUTBool = 0;
int TLDDataThread11OUTBool = 0;

pthread_mutex_t mutex_thread1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread3 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread4 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread5 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread5 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread6 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread6 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread7 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread7 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread8 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread8 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread9 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread9 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread10 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread10 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread11 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread11 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_main = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_main = PTHREAD_COND_INITIALIZER;


static int BarrelCorrectInit(int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight, float i_f32Ratio[2],
				 float* coordY, float* coordUV)
{
	float vary, vary1, varall;

	int nI, nJ,deltx,delty;

	int nNewHalfW, nNewHalfH, nNewWidthStep;
	int nOrgJ, nOrgI, nHalfWidth, nHalfHeight, nWidthStep, nDiff;
	int nWeight1j,nWeight2j,nWeight1i,nWeight2i;
	int nOrgJLow,nOrgJHigh, nOrgILow,nOrgIHigh;
	int nLeftBound, nRightBound, nUpBound, nDownBound;

	nHalfWidth  = i_nWidth/2;
	nHalfHeight = i_nHeight/2;
	nNewHalfW = i_nNewWidth/2;
	nNewHalfH = i_nNewHeight/2;
	nWidthStep = i_nWidth;
	nNewWidthStep = i_nNewWidth;

	for(nI=0;nI<i_nNewHeight;nI++)
	{
		delty = nI-nNewHalfH;
		vary = delty*delty;

		for (nJ=0;nJ<i_nNewWidth;nJ++)
		{
			deltx = nJ-nNewHalfW;
			vary1 = deltx*deltx+vary;
			varall = (1.0+ vary1*vary1 * i_f32Ratio[1] +vary1*i_f32Ratio[0]);

			nOrgJ = (deltx*varall+nHalfWidth)*1024;
			nOrgI = (delty*varall+nHalfHeight)*1024;
			nOrgJLow = nOrgJ/1024;
			nOrgJHigh = nOrgJLow+1;
			nOrgILow = nOrgI/1024;
			nOrgIHigh = nOrgILow+1;

			if(nOrgJLow>0 && nOrgJHigh < i_nWidth-1 && nOrgILow>0 && nOrgIHigh < i_nHeight-1)
			{
				nDiff=nOrgJ-(nOrgJLow<<10);
				coordY[(nI*i_nNewWidth+nJ)*2+0] = ((float)nOrgJLow + (float)nDiff/(float)1024)/(float)i_nWidth;
				nDiff=nOrgI-nOrgILow*1024;
				coordY[(nI*i_nNewWidth+nJ)*2+1] = ((float)nOrgILow + (float)nDiff/(float)1024)/(float)i_nHeight;

				if (nJ % 2 == 0 && nI % 2 == 0)
				{
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+0] = coordY[(nI*i_nNewWidth+nJ)*2+0];
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+1] = coordY[(nI*i_nNewWidth+nJ)*2+1];
				}
			}
		}
	}

	return 0;
}


static int VirtualStitchInit(int width, int height, int widthOut, int heightOut,
	float scale, int* vertIdxShift,
	int* k, int* weight1, int* weight2, int* kUV, int* weight1UV, int* weight2UV)
{
	int nV;
	int nI;
	int nK = height-2; //��ʼ�ӵ����ڶ��п�ʼ
	int i;
	int j;
	int nDiff;
	for(nI=heightOut-1;nI>=0;nI--)//�����Ŵ�
	{
	        nV = (((float)nI-((float)heightOut-height*scale))/scale)*1024;
	        while(nV<vertIdxShift[nK] && nK>0)
	        {
	            nK--;
	        }

		k[nI] = nK;
		if (nK >= 0)
		{
			weight1[nI] = vertIdxShift[nK+1]-nV;
			weight2[nI] = nV-vertIdxShift[nK];
		}
//		ISPprintLog("k[%d] = %d,", nI, k[nI]);
	}

    nK = height-2;
    for(nI=heightOut-2;nI>=0;nI-=2)//uv�����Ŵ�
    {
        nV = (((float)nI-((float)heightOut-height*scale))/scale)*1024;
        while(nV<vertIdxShift[nK] && nK>0)
        {
            nK--;
        }
        if(nK%2==1)
        {
            nK--;
        }

	kUV[nI/2] = nK/2;
//	ISPprintLog("k[%d] = %d,", nI/2, kUV[nI/2]);
        weight1UV[nI/2]= vertIdxShift[nK+2]-nV;
        weight2UV[nI/2] = nV-vertIdxShift[nK];
    	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
#define VERT_SIZE 3000

float MAX_CUSTOM(float a, float b)
{
    if(a>b)return a;
    else return b;
}
float MIN_CUSTOM(float a, float b)
{
    if(a>b)return b;
    else return a;
}

float g_f32VertRatio[VERT_SIZE];
int g_nVertIdxShift[VERT_SIZE];
float g_f32ScaleRatio;

int *pOrigXLow;
int *pOrigYLow;
int *pWeightX;
int *pWeightY;

int *pOrigXLine, *pOrigYLine, *pWtXLine, *pWtYLine;
typedef  signed long s32;
typedef float f32;

typedef struct KedaVirtualStitchAlgoParam
{
	s32 m_s32BarrelParam1;
	s32 m_s32BarrelParam2;
	s32 m_s32TeleZoomInRatio;
	s32 m_s32TeleCurvLevel;
	s32 m_s32TeleMultVal;
	s32 m_s32WideRatio;
	s32 m_s32MidWidth;
	s32 m_s32MidHeight;
	s32 m_s32AdjHeight;
	f32 m_f32WidthScale;

}KedaVirtualStitchAlgoParam_T;

typedef struct KedaVirtualStitchProcessParam
{
	s32 m_s32ImgWidth;
	s32 m_s32ImgHeight;
	s32 m_s32FinalWidth;
	s32 m_s32FinalHeight;
	KedaVirtualStitchAlgoParam_T m_tAlgoParam;

}KedaVirtualStitchProcessParam_T;

int VirtualStitchParaInit(int width, int height,
						float zoom, int endHeight,
						int startHeight, int *pOutWidth,
						int *pOutHeight, int level,
						int upMult, float maxRatio,
						int finalHeight, float f32WidthAdjRatio)
{
   int nI=0;
   int nOutH;
   int nFinalWidth;
   int nAdjustEndHeight, nAdjustStartHeight;

   int nMidPoint;

   float f32HoriRatioComp;
   float f32MaxZoomIn, f32Ratio, f32OrgTopWidth;
   float f32VertZoomStart;

   float f32DownPartRatio, f32UpPartRatio;

	f32MaxZoomIn = zoom;
    nAdjustEndHeight = endHeight;
    nAdjustStartHeight = startHeight;

    f32OrgTopWidth = width/f32MaxZoomIn;
    *pOutWidth = (int)(width*f32MaxZoomIn);
	for(nI=0;nI<height;nI++)//
    {
		if(nI<nAdjustStartHeight)
			f32Ratio = exp((nAdjustStartHeight-(float)nI)/(level+1000+2*abs(nAdjustStartHeight-nI)));
		else
			f32Ratio = (nAdjustEndHeight-(float)nI)/((float)(nAdjustEndHeight-nAdjustStartHeight));

        f32Ratio = MAX_CUSTOM(f32Ratio, 0);

        nFinalWidth = (f32Ratio*f32OrgTopWidth+(1.0-MIN_CUSTOM(f32Ratio,1.0))*width);
        g_f32VertRatio[nI] = ((float)width)/((float)nFinalWidth);
	}
	for(nI=height-1;nI>=0;nI--)
    {
        if(g_f32VertRatio[nI] <=1.0000001 || nI==height-1)
        {
            f32VertZoomStart = nI;
            g_nVertIdxShift[nI] = nI<<10;

        }
        else
        {
            f32VertZoomStart -= g_f32VertRatio[nI];//���ո�ֵ
            g_nVertIdxShift[nI] = (int)(f32VertZoomStart*1024.0);

        }
		if(nI==nAdjustStartHeight)
			nMidPoint = f32VertZoomStart;
    }
	nOutH = height-f32VertZoomStart;//��������Ӧ�ĳ����ĸ���
	nMidPoint -= f32VertZoomStart;

	f32DownPartRatio = (1.0 - maxRatio)/(nOutH-nMidPoint-1);
	f32UpPartRatio = f32DownPartRatio*upMult;
	f32VertZoomStart = nOutH;

	if(finalHeight>0 && finalHeight <nOutH)
		g_f32ScaleRatio = (float)finalHeight / (float)nOutH;
	else
		g_f32ScaleRatio = 1.0;

	for(nI=height-1;nI>=0;nI--)
    {
        if(g_f32VertRatio[nI] <=1.0000001 || nI==height-1)
        {
            f32VertZoomStart -= 1;
        }
        else
        {
            f32VertZoomStart -= g_f32VertRatio[nI];//���ո�ֵ
        }
		if (f32VertZoomStart>nMidPoint)
			f32HoriRatioComp =  maxRatio + f32DownPartRatio*(f32VertZoomStart-nMidPoint);
		else
			f32HoriRatioComp =f32HoriRatioComp +  f32UpPartRatio*MIN_CUSTOM(exp((f32VertZoomStart-nMidPoint+100.0)/100.0),1.0);//���۲⣺��������100.0������ȡΪ���Ʋ���

		g_f32VertRatio[nI] *= (f32HoriRatioComp*g_f32ScaleRatio*f32WidthAdjRatio);

	}

	nOutH = nOutH*g_f32ScaleRatio;
	*pOutWidth = (*pOutWidth)*g_f32ScaleRatio;
	*pOutWidth = (*pOutWidth/8)*8;
	*pOutHeight = nOutH;

//	for(nI=0;nI<height;nI++)
//		fprintf(pParaFile,"%d\n", g_nVertIdxShift[nI]);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
int VirtualStitchParaProcess(int width, int height,
							int i_nOutWidth, int i_nOutHeight,
							float i_f32Ratio[2], int i_nOrgWidth,
							int i_nOrgHeight, int i_nFinalWidth)
{
	float deltx,delty;
	float vary, vary1, varall;
	FILE * fp=NULL;
    int nI,nJ,nCnt;
    int nOrgJ,nOrgJLow,nOrgJHigh;
    int nHalfWidth, nHalfHeight,nFinalWidth,nFinalCenter,nOutFinalCenter, nWidthStep, nOutWidthStep;
	int nOrgHalfWidth, nOrgHalfHeight;
	int nTempCor1,nTempCor2;

    int nDiff;
	int nLeftStart, nRightEnd;

    long long s64TmpValue, s64TmpConst0,s64TmpConst1, s64TmpConst2;

 /*  pOrigXLow = (int*)malloc(i_nFinalWidth*height*sizeof(int));
   pOrigYLow = (int*)malloc(i_nFinalWidth*height*sizeof(int));
   pWeightX = (int*)malloc(i_nFinalWidth*height*sizeof(int));
   pWeightY = (int*)malloc(i_nFinalWidth*height*sizeof(int));*/

	nOrgHalfWidth = i_nOrgWidth/2;
	nOrgHalfHeight = i_nOrgHeight/2;
    nHalfWidth  = width/2;
    nHalfHeight = height/2;
    nWidthStep = width;

	nOutWidthStep = i_nOutWidth;

	nFinalCenter = width/2;
	nOutFinalCenter = i_nOutWidth/2;

	nLeftStart = -1;
//	fp=fopen("/sdcard/zhaohaibin/1.txt","w");
    for(nI=0;nI<height;nI++)//�����Ŵ�
    {
		delty =(nI-nHalfHeight);
		vary = delty*delty;

        pOrigXLine = pOrigXLow+i_nFinalWidth*nI;
        pOrigYLine = pOrigYLow+i_nFinalWidth*nI;
        pWtXLine = pWeightX+i_nFinalWidth*nI;
        pWtYLine = pWeightY+i_nFinalWidth*nI;

        nFinalWidth = (int)(((float)width)/(g_f32VertRatio[nI]));

        s64TmpConst2 = nFinalCenter<<10;
        s64TmpValue = (long long)(nFinalWidth);
        s64TmpConst0 = (s64TmpValue<<20)/((long long)width);
        s64TmpConst1 = ((s64TmpValue*(long long)nOutFinalCenter)<<10)/((long long)width);

        nCnt = nI*nOutWidthStep;
		if(nLeftStart == -1)
		{
			nOrgJ = -s64TmpConst1+s64TmpConst2;
			nOrgJLow = nOrgJ>>10;
			nLeftStart = (i_nOutWidth-i_nFinalWidth)/2;
			nRightEnd = i_nOutWidth-nLeftStart;
		}

        for (nJ=0;nJ<i_nOutWidth;nJ++,nCnt+=3)
        {
            nOrgJ = (((long long)nJ*s64TmpConst0)>>10)-s64TmpConst1+s64TmpConst2;

			deltx = (nOrgJ-(nHalfWidth<<10))/1024.0;
			vary1 = deltx*deltx+vary;
			varall = (1.0+ vary1*vary1 * i_f32Ratio[1] +vary1*i_f32Ratio[0]);

            nOrgJLow = nOrgJ>>10;
            nOrgJHigh = nOrgJLow+1;
            nDiff=nOrgJ-(nOrgJLow<<10);

			if(nJ>=nLeftStart && nJ<nRightEnd)//����������Ϊbarrel stitch�ϲ�ʹ�õĲ�����
			{
				nTempCor1 = (nOrgHalfWidth+deltx*varall)*1024;
				nTempCor2 = (nOrgHalfHeight+delty*varall)*1024;
				pOrigXLine[nJ-nLeftStart] = nTempCor1>>10;
				pOrigYLine[nJ-nLeftStart] = nTempCor2>>10;
//
				pWtXLine[nJ-nLeftStart]=nTempCor1-(pOrigXLine[nJ-nLeftStart]<<10);
				pWtYLine[nJ-nLeftStart]=nTempCor2-(pOrigYLine[nJ-nLeftStart]<<10);

//				fprintf(fp,"%d,%d,%d,%d\n",pOrigXLine[nJ-nLeftStart],pOrigYLine[nJ-nLeftStart],pWtXLine[nJ-nLeftStart],pWtYLine[nJ-nLeftStart]);
			}

        }
    }

/*   free(pOrigXLow);
   free(pOrigYLow);
   free(pWeightX);
   free(pWeightY);*/
    return 0;
}

void param_process(KedaVirtualStitchProcessParam_T i_tParam)
{
	float f32Ratio[2];
	float f32MaxZoomIn,maxRatio;
	int nMidWidth, nMidHeight;
	int nOutWidth,nOutHeight;

	int  nHalfWidth;
	int  nAdjustStartHeight, nAdjustEndHeight;

	f32Ratio[0] = -(i_tParam.m_tAlgoParam.m_s32BarrelParam1)/10000000000.0;
	f32Ratio[1]= (i_tParam.m_tAlgoParam.m_s32BarrelParam2)/10000000000000000.0;

	f32MaxZoomIn = 1.0+i_tParam.m_tAlgoParam.m_s32TeleZoomInRatio/1000.0;

	nMidWidth = i_tParam.m_tAlgoParam.m_s32MidWidth;//3584;
	nMidHeight= i_tParam.m_tAlgoParam.m_s32MidHeight;//1657;
	nHalfWidth  = nMidWidth/2;

	nAdjustEndHeight=nMidHeight-1;
	nAdjustStartHeight = i_tParam.m_tAlgoParam.m_s32AdjHeight;

	maxRatio = (1000.0-i_tParam.m_tAlgoParam.m_s32WideRatio)/1000.0;

    VirtualStitchParaInit(nMidWidth, nMidHeight, f32MaxZoomIn, nAdjustEndHeight, nAdjustStartHeight, &nOutWidth,&nOutHeight,
		i_tParam.m_tAlgoParam.m_s32TeleCurvLevel,i_tParam.m_tAlgoParam.m_s32TeleMultVal,maxRatio,i_tParam.m_s32FinalHeight,i_tParam.m_tAlgoParam.m_f32WidthScale);
  	VirtualStitchParaProcess( nMidWidth, nMidHeight, nOutWidth,nOutHeight, f32Ratio,i_tParam.m_s32ImgWidth, i_tParam.m_s32ImgHeight,i_tParam.m_s32FinalWidth);
}

int BC_VSParamsInit(TBC_VSOpen* ptBarrelCorrectionOpen)
{
	KedaVirtualStitchProcessParam_T tVirtualStitchParam;
	int nFinalHeight = ptBarrelCorrectionOpen->u32OutputHeight;
	int nFinalWidth = ptBarrelCorrectionOpen->u32OutputWidth;
	float f32WidthAdjRatio = ptBarrelCorrectionOpen->tBC_VSParam.f32WidthScale;//1.061947;
	int BASE1 =  ptBarrelCorrectionOpen->tBC_VSParam.s32BarrelParam1;//1390;
	int BASE2 = ptBarrelCorrectionOpen->tBC_VSParam.s32BarrelParam2;//140;
	int levels = ptBarrelCorrectionOpen->tBC_VSParam.s32TeleZoomInRatio;//600;
	int levels_curv = ptBarrelCorrectionOpen->tBC_VSParam.s32TeleCurvLevel;//400;
	int ratio_level = ptBarrelCorrectionOpen->tBC_VSParam.s32WideRatio;//200;
	int upMult = ptBarrelCorrectionOpen->tBC_VSParam.s32TeleMultVal;//5;

	tVirtualStitchParam.m_s32ImgWidth = ptBarrelCorrectionOpen->u32InputWidth;//2688;
	tVirtualStitchParam.m_s32ImgHeight = ptBarrelCorrectionOpen->u32InputHeight;//1512;
	tVirtualStitchParam.m_s32FinalWidth = nFinalWidth;
	tVirtualStitchParam.m_s32FinalHeight = nFinalHeight;

	tVirtualStitchParam.m_tAlgoParam.m_s32BarrelParam1 = BASE1;
	tVirtualStitchParam.m_tAlgoParam.m_s32BarrelParam2 = BASE2;
	tVirtualStitchParam.m_tAlgoParam.m_s32TeleZoomInRatio = levels;//���ű���
	tVirtualStitchParam.m_tAlgoParam.m_s32TeleCurvLevel = levels_curv;//�ϲ�����
	tVirtualStitchParam.m_tAlgoParam.m_s32TeleMultVal = upMult; //�ϲ�б�ʳ���
	tVirtualStitchParam.m_tAlgoParam.m_s32WideRatio = ratio_level;//�²�����

	tVirtualStitchParam.m_tAlgoParam.m_s32MidWidth = ptBarrelCorrectionOpen->tBC_VSParam.s32MidWidth;//3584;
	tVirtualStitchParam.m_tAlgoParam.m_s32MidHeight = ptBarrelCorrectionOpen->tBC_VSParam.s32MidHeight;//1656;
	tVirtualStitchParam.m_tAlgoParam.m_s32AdjHeight = ptBarrelCorrectionOpen->tBC_VSParam.s32AdjHeight;//248;
	tVirtualStitchParam.m_tAlgoParam.m_f32WidthScale = f32WidthAdjRatio;

	param_process(tVirtualStitchParam);
	return 0;
}

void bcvs_convert0 ()
{
	int j;


#if 0
	int b, m,n,o,p, e,f,g,h, tmp, tmp2;
	for (j = 0; j < 1080*1920; j ++)
	{
		b = coordVSYY[j];
		m = pSrcY[b];
		n = pSrcY[b+1];
		o = pSrcY[b+2560];
		p = pSrcY[b+2561];

		e = VScoordYYFXYCHAR[j*4];
		f = VScoordYYFXYCHAR[j*4+1];
		g = VScoordYYFXYCHAR[j*4+2];
		h = VScoordYYFXYCHAR[j*4+3];

		tmp = (m*e + n*f + o*g + p*h)>>8;

		b = coordVSYY2[j];
		m = pSrcY[b];
		n = pSrcY[b+1];
		o = pSrcY[b+2560];
		p = pSrcY[b+2561];

		e = VScoordYYFXYCHAR2[j*4];
		f = VScoordYYFXYCHAR2[j*4+1];
		g = VScoordYYFXYCHAR2[j*4+2];
		h = VScoordYYFXYCHAR2[j*4+3];

		tmp2 = (m*e + n*f + o*g + p*h)>>8;





		VSY[j] = (tmp*VScoordWEIGHT[2*j] + tmp2*VScoordWEIGHT[2*j+1])/(VScoordWEIGHT[2*j]+VScoordWEIGHT[2*j+1]);
	}
#endif

#if 0
#ifdef VSTHRE12
	for (j = 1920*1080/12*0; j < 1920*1080/12*1; j += 8)
#else
	for (j = 0; j < 1080*1920; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+2560],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+2560],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+2560],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+2560],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+2560],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+2560],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+2560],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+2560],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+2561],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+2561],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+2561],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+2561],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+2561],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+2561],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+2561],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+2561],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+2560],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+2560],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+2560],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+2560],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+2560],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+2560],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+2560],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+2560],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+2561],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+2561],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+2561],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+2561],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+2561],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+2561],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+2561],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+2561],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result1 = vshrn_n_u16 (temp, 8);

		uint16x8_t temp2;
		temp2 = vmull_u8(x0y02, fxy2.val[0]);
		temp2 = vmlal_u8(temp2, x1y02, fxy2.val[1]);
		temp2 = vmlal_u8(temp2, x0y12, fxy2.val[2]);
		temp2 = vmlal_u8(temp2, x1y12, fxy2.val[3]);
	    uint8x8_t result2 = vshrn_n_u16 (temp2, 8);

		uint8x8x2_t fw =  vld2_u8(VScoordWEIGHT+j*2);
		uint16x8_t temp3;
		temp3 = vmull_u8(result1, fw.val[0]);
		temp3 = vmlal_u8(temp3, result2, fw.val[1]);

	    uint8x8_t result = vshrn_n_u16 (temp3, 8);
    	vst1_u8 (VSY+j, result);
	}
#endif



#if 1

#ifdef VSTHRE12
	for (j = 1920*1080/12*0; j < 1920*1080/12*1; j += 8)
#else
	for (j = 1080*1920/8*0; j < 1080*1920/8*1; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}




#endif
	int a;
#ifdef VSTHRE12
	for (j = 0; j < 1920*1080/48; j++)
#else
	for (j = 0; j < 1080*1920/32; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert1 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*1; j < 1920*1080/12*2; j += 8)
#else
	for (j = 1080*1920/8*1; j < 1080*1920/8*2; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*1; j < 1920*1080/48*2; j++)
#else
	for (j = 1080*1920/32*1; j < 1080*1920/32*2; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert2 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*2; j < 1920*1080/12*3; j += 8)
#else
	for (j = 1080*1920/8*2; j < 1080*1920/8*3; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*2; j < 1920*1080/48*3; j++)
#else
	for (j = 1080*1920/32*2; j < 1080*1920/32*3; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert3 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*3; j < 1920*1080/12*4; j += 8)
#else
	for (j = 1080*1920/8*3; j < 1080*1920/8*4; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*3; j < 1920*1080/48*4; j++)
#else
	for (j = 1080*1920/32*3; j < 1080*1920/32*4; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert4 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*4; j < 1920*1080/12*5; j += 8)
#else
	for (j = 1080*1920/8*4; j < 1080*1920/8*5; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*4; j < 1920*1080/48*5; j++)
#else
	for (j = 1080*1920/32*4; j < 1080*1920/32*5; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert5 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*5; j < 1920*1080/12*6; j += 8)
#else
	for (j = 1080*1920/8*5; j < 1080*1920/8*6; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*5; j < 1920*1080/48*6; j++)
#else
	for (j = 1080*1920/32*5; j < 1080*1920/32*6; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert6 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*6; j < 1920*1080/12*7; j += 8)
#else
	for (j = 1080*1920/8*6; j < 1080*1920/8*7; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*6; j < 1920*1080/48*7; j++)
#else
	for (j = 1080*1920/32*6; j < 1080*1920/32*7; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert7 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*7; j < 1920*1080/12*8; j += 8)
#else
	for (j = 1080*1920/8*7; j < 1080*1920/8*8; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*7; j < 1920*1080/48*8; j++)
#else
	for (j = 1080*1920/32*7; j < 1080*1920/32*8; j++)
#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert8 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*8; j < 1920*1080/12*9; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*8; j < 1920*1080/48*9; j++)
#else

#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}
void bcvs_convert9 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*9; j < 1920*1080/12*10; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*9; j < 1920*1080/48*10; j++)
#else

#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert10 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*10; j < 1920*1080/12*11; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHYADD1],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*10; j < 1920*1080/48*11; j++)
#else

#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}

void bcvs_convert11 ()
{
	int j;
#ifdef VSTHRE12
	for (j = 1920*1080/12*11; j < 1920*1080/12*12; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHAR+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcY[coordVSYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcY[coordVSYY[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcY[coordVSYY2[j+7]+VS_PITCHY],x1y12,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);

		temp = vmlal_u8(temp, x0y02, fxy2.val[0]);
		temp = vmlal_u8(temp, x1y02, fxy2.val[1]);
		temp = vmlal_u8(temp, x0y12, fxy2.val[2]);
		temp = vmlal_u8(temp, x1y12, fxy2.val[3]);

	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (VSY+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 1920*1080/48*11; j < 1920*1080/48*12; j++)
#else

#endif
	{
		a=coordVSUUV[j];
		VSUV[(j<<1)] = pSrcU[a];
		VSUV[(j<<1) +1] = pSrcU[a+1];
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////


void ProcessBCThread1()
{

#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(4, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_1");
	prctl(PR_SET_NAME, (unsigned long)threName);

	while(0 == g1_exit_flag)
	{
		ISPprintLog("ProcessBCThread1 enter\n");
		pthread_mutex_lock(&mutex_thread1);
		while(TLDDataThread1Bool == 0)
		{
			pthread_cond_wait(&cond_thread1,&mutex_thread1);
		}
		pthread_mutex_unlock(&mutex_thread1);
		TLDDataThread1Bool=0;
		ISPprintLog("ProcessBCThread1 start\n");

		bcvs_convert1();

		TLDDataThread1OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}



void ProcessBCThread2()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(5, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_2");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g2_exit_flag)
	{
		ISPprintLog("ProcessBCThread2 enter\n");

		pthread_mutex_lock(&mutex_thread2);
		while(TLDDataThread2Bool == 0)
		{
			pthread_cond_wait(&cond_thread2,&mutex_thread2);
		}
		pthread_mutex_unlock(&mutex_thread2);
		TLDDataThread2Bool=0;
		ISPprintLog("ProcessBCThread2 start\n");

		bcvs_convert2();

		TLDDataThread2OUTBool = 1;
		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
	}
}



void ProcessBCThread3()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(6, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_3");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g3_exit_flag)
	{
		ISPprintLog("ProcessBCThread3 enter\n");

		pthread_mutex_lock(&mutex_thread3);
		while(TLDDataThread3Bool == 0)
		{
			pthread_cond_wait(&cond_thread3,&mutex_thread3);
		}
		pthread_mutex_unlock(&mutex_thread3);
		TLDDataThread3Bool=0;
		ISPprintLog("ProcessBCThread3 start\n");

		bcvs_convert3();

		TLDDataThread3OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
	}
}



void ProcessBCThread4()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(7, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_4");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g4_exit_flag)
	{
		ISPprintLog("ProcessBCThread4 enter\n");

		pthread_mutex_lock(&mutex_thread4);
		while(TLDDataThread4Bool == 0)
		{
			pthread_cond_wait(&cond_thread4,&mutex_thread4);
		}
		pthread_mutex_unlock(&mutex_thread4);
		TLDDataThread4Bool=0;
		ISPprintLog("ProcessBCThread4 start\n");

		bcvs_convert4();

		TLDDataThread4OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);

	}
}

void ProcessBCThread5()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(4, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_5");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g5_exit_flag)
	{
		ISPprintLog("ProcessBCThread5 enter\n");

		pthread_mutex_lock(&mutex_thread5);
		while(TLDDataThread5Bool == 0)
		{
			pthread_cond_wait(&cond_thread5,&mutex_thread5);
		}
		pthread_mutex_unlock(&mutex_thread5);
		TLDDataThread5Bool=0;

		ISPprintLog("ProcessBCThread5 start\n");

		bcvs_convert5();

		TLDDataThread5OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}

void ProcessBCThread6()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(5, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_6");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g6_exit_flag)
	{
		ISPprintLog("ProcessBCThread6 enter\n");

		pthread_mutex_lock(&mutex_thread6);
		while(TLDDataThread6Bool == 0)
		{
			pthread_cond_wait(&cond_thread6,&mutex_thread6);
		}
		pthread_mutex_unlock(&mutex_thread6);
		TLDDataThread6Bool=0;

		ISPprintLog("ProcessBCThread6 start\n");

		bcvs_convert6();

		TLDDataThread6OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}

void ProcessBCThread7()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(6, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_7");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g7_exit_flag)
	{
		ISPprintLog("ProcessBCThread7 enter\n");

		pthread_mutex_lock(&mutex_thread7);
		while(TLDDataThread7Bool == 0)
		{
			pthread_cond_wait(&cond_thread7,&mutex_thread7);
		}
		pthread_mutex_unlock(&mutex_thread7);
		TLDDataThread7Bool=0;

		ISPprintLog("ProcessBCThread7 start\n");

		bcvs_convert7();


		TLDDataThread7OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}

void ProcessBCThread8()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(7, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_8");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g8_exit_flag)
	{
		ISPprintLog("ProcessBCThread8 enter\n");

		pthread_mutex_lock(&mutex_thread8);
		while(TLDDataThread8Bool == 0)
		{
			pthread_cond_wait(&cond_thread8,&mutex_thread8);
		}
		pthread_mutex_unlock(&mutex_thread8);
		TLDDataThread8Bool=0;

		ISPprintLog("ProcessBCThread8 start\n");

		bcvs_convert8();

		TLDDataThread8OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}


void ProcessBCThread9()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(7, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_9");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g9_exit_flag)
	{
		ISPprintLog("ProcessBCThread9 enter\n");

		pthread_mutex_lock(&mutex_thread9);
		while(TLDDataThread9Bool == 0)
		{
			pthread_cond_wait(&cond_thread9,&mutex_thread9);
		}
		pthread_mutex_unlock(&mutex_thread9);
		TLDDataThread9Bool=0;

		ISPprintLog("ProcessBCThread9 start\n");

		bcvs_convert9();

		TLDDataThread9OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}


void ProcessBCThread10()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(7, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_10");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g10_exit_flag)
	{
		ISPprintLog("ProcessBCThread10 enter\n");

		pthread_mutex_lock(&mutex_thread10);
		while(TLDDataThread10Bool == 0)
		{
			pthread_cond_wait(&cond_thread10,&mutex_thread10);
		}
		pthread_mutex_unlock(&mutex_thread10);
		TLDDataThread10Bool=0;

		ISPprintLog("ProcessBCThread10 start\n");

		bcvs_convert10();

		TLDDataThread10OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}


void ProcessBCThread11()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(7, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif

	char threName[100] = {0};
	sprintf(threName,"BCVS_THREAD_11");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g11_exit_flag)
	{
		ISPprintLog("ProcessBCThread11 enter\n");

		pthread_mutex_lock(&mutex_thread11);
		while(TLDDataThread11Bool == 0)
		{
			pthread_cond_wait(&cond_thread11,&mutex_thread11);
		}
		pthread_mutex_unlock(&mutex_thread11);
		TLDDataThread11Bool=0;

		ISPprintLog("ProcessBCThread11 start\n");

		bcvs_convert11();

		TLDDataThread11OUTBool = 1;

		pthread_mutex_lock(&mutex_main);
		pthread_cond_signal(&cond_main);
		pthread_mutex_unlock(&mutex_main);
		}
}


int BC_VSOpen(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject,
				TBC_VSObject* ptBC_VSObject, TBC_VSOpen* ptBC_VSOpen)
{

     ISPprintLog("TBC_VSOpen===========>1.InputWidth:%d\n,2.InputHeight:%d\n,",ptBC_VSOpen->u32InputWidth,ptBC_VSOpen->u32InputHeight);
     ISPprintLog("====>3.InputPitchY:%d\n,4.InputPitchUV:%d\n,",ptBC_VSOpen->u32InputPitchY,ptBC_VSOpen->u32InputPitchUV);
     ISPprintLog("====>5.OutputWidth:%d\n,6.OutputHeight:%d\n,",ptBC_VSOpen->u32OutputWidth,ptBC_VSOpen->u32OutputHeight);
     ISPprintLog("TBC_VSParam===========>7.BarrelParam1:%d\n,8.BarrelParam2:%d\n,", ptBC_VSOpen->tBC_VSParam.s32BarrelParam1,ptBC_VSOpen->tBC_VSParam.s32BarrelParam2);
     ISPprintLog("====>9.TeleZoomInRatio:%d\n,10.TeleCurvLevel:%d\n,", ptBC_VSOpen->tBC_VSParam.s32TeleZoomInRatio,ptBC_VSOpen->tBC_VSParam.s32TeleCurvLevel);
     ISPprintLog("====>11.TeleMultVal:%d\n,12.WideRatio:%d\n,", ptBC_VSOpen->tBC_VSParam.s32TeleMultVal,ptBC_VSOpen->tBC_VSParam.s32WideRatio);
     ISPprintLog("====>13.MidWidth:%d\n,14.MidHeight:%d\n,", ptBC_VSOpen->tBC_VSParam.s32MidWidth,ptBC_VSOpen->tBC_VSParam.s32MidHeight);
     ISPprintLog("====>15.AdjHeight:%d\n, 16.WidthScale:%f\n", ptBC_VSOpen->tBC_VSParam.s32AdjHeight,ptBC_VSOpen->tBC_VSParam.f32WidthScale);

	int width = ptBC_VSOpen->u32InputWidth;
	int height = ptBC_VSOpen->u32InputHeight;
	int widthOut = ptBC_VSOpen->u32OutputWidth;
	int heightOut = ptBC_VSOpen->u32OutputHeight;
	float scale = 0;
	int i;
	int j;
	char path[200];
	FILE* file = NULL;

	int newWidth;
	int newHeight;
	float f32Ratio[2];

	int *p1 = NULL;
	int* p2 = NULL;
	int* p3 = NULL;
	int* p4 = NULL;

	float* coordY = NULL;
	float* coordUV = NULL;

	unsigned char* pTempY = NULL;
	unsigned char* pTempUV = NULL;

    pOrigXLow = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));
    pOrigYLow = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));
    pWeightX = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));
    pWeightY = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));

   	p1 = pOrigXLow;
	p2 = pOrigYLow;
	p3 = pWeightX;
	p4 = pWeightY;

    BC_VSParamsInit(ptBC_VSOpen);


	scale = g_f32ScaleRatio;
	ptBC_VSObject->pVertIdxShift = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*sizeof(int));


	ISPprintLog("vertIdxShift[1000] = %d\n", g_nVertIdxShift[1000]);

	newWidth = widthOut;
	newHeight =  ptBC_VSOpen->tBC_VSParam.s32MidHeight;

	coordY = (float*)malloc(newWidth*newHeight*sizeof(float)*2);
	coordUV = (float*)malloc(newWidth*(newHeight+8)*sizeof(float)*2/4);

	ptBC_VSObject->virtualWidthOut = ptBC_VSOpen->u32OutputWidth;
	ptBC_VSObject->virtualHeightOut = ptBC_VSOpen->u32OutputHeight;

	coordVSY = (float*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(float)*2);
//	coordVSY2 = (float*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(float)*2);
	coordVSUV = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int)*2/4);

	coordVSYY = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int));
	coordVSYY2 = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int));
	coordVSUUV = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int)*2/4/2);

	ptBC_VSObject->srcWidth = width;
	ptBC_VSObject->srcHeight = height;
	ptBC_VSObject->srcPitchY = ptBC_VSOpen->u32InputPitchY;
	ptBC_VSObject->srcPitchUV = ptBC_VSOpen->u32InputPitchUV;
	ptBC_VSObject->dstWidth = newWidth;
	ptBC_VSObject->dstHeight = newHeight;
	ptBC_VSObject->pCoordY = coordY;
	ptBC_VSObject->pCoordUV = coordUV;

	ptBC_VSObject->scale = scale;

	ptBC_VSObject->pK = (int*)malloc(newHeight*sizeof(int));
	ptBC_VSObject->pWeight1 = (int*)malloc(newHeight*sizeof(int));
	ptBC_VSObject->pWeight2 = (int*)malloc(newHeight*sizeof(int));

	ptBC_VSObject->pKUV = (int*)malloc(newHeight/2*sizeof(int));
	ptBC_VSObject->pWeight1UV = (int*)malloc(newHeight/2*sizeof(int));
	ptBC_VSObject->pWeight2UV = (int*)malloc(newHeight/2*sizeof(int));


//	ptBC_VSObject->pVirtualImgOutY = (unsigned char*)malloc(ptBarrelCorrectionObject->virtualWidthOut * ptBarrelCorrectionObject->virtualHeightOut* sizeof(unsigned char));
//	ptBC_VSObject->pVirtualImgOutUV = (unsigned char*)malloc(ptBarrelCorrectionObject->virtualWidthOut * ptBarrelCorrectionObject->virtualHeightOut* sizeof(unsigned char) / 2);

/*	VirtualStitchInit(newWidth, newHeight, ptBarrelCorrectionOpen->u32OutputWidth, ptBarrelCorrectionOpen->u32OutputHeight,
		scale, tBarrelCorrectionObject.pVertRatio, tBarrelCorrectionObject.pVertIdxShift,
		tBarrelCorrectionObject.pVirtualCoordY, tBarrelCorrectionObject.pVirtualCoordUV,
		tBarrelCorrectionObject.pK,tBarrelCorrectionObject.pWeight1, tBarrelCorrectionObject.pWeight2,
		tBarrelCorrectionObject.pKUV,tBarrelCorrectionObject.pWeight1UV, tBarrelCorrectionObject.pWeight2UV);*/

	ISPprintLog("VirtualStitchInit start!\n");
	VirtualStitchInit(newWidth, newHeight, ptBC_VSOpen->u32OutputWidth, ptBC_VSOpen->u32OutputHeight,
	scale,g_nVertIdxShift,ptBC_VSObject->pK,ptBC_VSObject->pWeight1, ptBC_VSObject->pWeight2,
		ptBC_VSObject->pKUV,ptBC_VSObject->pWeight1UV, ptBC_VSObject->pWeight2UV);
	ISPprintLog("VirtualStitchInit end!\n");

	ISPprintLog("coordination start!\n");


	VScoordYYFXYCHAR = (unsigned char*)malloc(ptBC_VSObject->virtualWidthOut * ptBC_VSObject->virtualHeightOut * 4 * sizeof(unsigned char));
	VScoordYYFXYCHAR2 = (unsigned char*)malloc(ptBC_VSObject->virtualWidthOut * ptBC_VSObject->virtualHeightOut * 4 * sizeof(unsigned char));
	VScoordWEIGHT = (unsigned char*)malloc(ptBC_VSObject->virtualWidthOut * ptBC_VSObject->virtualHeightOut * 2 * sizeof(unsigned char));



//BCVS�е�BC����ת�� float��
	for (j = 0; j < newHeight; j++)
	{
		for (i = 0; i < newWidth; i++)
		{
			int x = *p1++;
			int y = *p2++;
			int weightx = *p3++;
			int weighty = *p4++;
			coordY[(j*newWidth+i)*2] = ((float)x + (float)weightx/(float)1024);///(float)width;
			coordY[(j*newWidth+i)*2+1] = ((float)y + (float)weighty/(float)1024);///(float)height;
			if (i % 2 == 0 && j % 2 == 0)
			{
				coordUV[(j/2*newWidth/2+i/2)*2] = coordY[(j*newWidth+i)*2];
				coordUV[(j/2*newWidth/2+i/2)*2+1] = coordY[(j*newWidth+i)*2+1];
			}
		}
	}

	for (j = 0; j < 200; j++)
	{
		ISPprintLog("weight 1-%d  2-%d  sum-%d\n",ptBC_VSObject->pWeight1[j],ptBC_VSObject->pWeight2[j],ptBC_VSObject->pWeight1[j]+ptBC_VSObject->pWeight2[j]);
	}

//BCVS�е�VS����
	int a,x,y,x2,xx,yy;
	int weight1,weight2,weightsum;
	float fxx,fyy;
	for (j = 0; j < ptBC_VSObject->virtualHeightOut; j++)
	{
		a = j*ptBC_VSObject->virtualWidthOut;
		for (i = 0; i < ptBC_VSObject->virtualWidthOut; i++)
		{
			x = ptBC_VSObject->pK[j];
			x2 = (ptBC_VSObject->pK[j])+1;
			weight1 = ptBC_VSObject->pWeight1[j];
			weight2 = ptBC_VSObject->pWeight2[j];
			weightsum = weight1+weight2;
			weight1 = (float)(weight1)/(float)(weightsum) * 255;
			weight2 = (float)(weight2)/(float)(weightsum) * 255;
			VScoordWEIGHT[2*(a+i)] = weight1;
			VScoordWEIGHT[2*(a+i)+1] = weight2;

			if(x>=0)
			{
				coordVSY[(j*ptBC_VSObject->virtualWidthOut+i)*2] = coordY[(x*newWidth+i)*2];
				coordVSY[(j*ptBC_VSObject->virtualWidthOut+i)*2+1] = coordY[(x*newWidth+i)*2+1];
//				coordVSY2[(j*ptBC_VSObject->virtualWidthOut+i)*2] = coordY[(x2*newWidth+i)*2];
//				coordVSY2[(j*ptBC_VSObject->virtualWidthOut+i)*2+1] = coordY[(x2*newWidth+i)*2+1];

				xx = coordY[(x*newWidth+i)*2];
				yy = coordY[(x*newWidth+i)*2+1];
				fxx = coordY[(x*newWidth+i)*2] - xx;
				fyy = coordY[(x*newWidth+i)*2+1] - yy;
				VScoordYYFXYCHAR[4*(a+i)] = (1- fxx)*(1-fyy)*weight1;
				VScoordYYFXYCHAR[4*(a+i)+1] = fxx*(1-fyy)*weight1;
				VScoordYYFXYCHAR[4*(a+i)+2] = (1- fxx)*fyy*weight1;
				VScoordYYFXYCHAR[4*(a+i)+3] = fxx*fyy*weight1;
//				VScoordYYFXYCHAR[4*(a+i)] = (1- fxx)*(1-fyy)*255;
//				VScoordYYFXYCHAR[4*(a+i)+1] = fxx*(1-fyy)*255;
//				VScoordYYFXYCHAR[4*(a+i)+2] = (1- fxx)*fyy*255;
//				VScoordYYFXYCHAR[4*(a+i)+3] = fxx*fyy*255;
				coordVSYY[(a+i)] = yy*ptBC_VSOpen->u32InputWidth + xx;

				xx = coordY[(x2*newWidth+i)*2];
				yy = coordY[(x2*newWidth+i)*2+1];
				fxx = coordY[(x2*newWidth+i)*2] - xx;
				fyy = coordY[(x2*newWidth+i)*2+1] - yy;
				VScoordYYFXYCHAR2[4*(a+i)] = (1- fxx)*(1-fyy)*weight2;
				VScoordYYFXYCHAR2[4*(a+i)+1] = fxx*(1-fyy)*weight2;
				VScoordYYFXYCHAR2[4*(a+i)+2] = (1- fxx)*fyy*weight2;
				VScoordYYFXYCHAR2[4*(a+i)+3] = fxx*fyy*weight2;
//				VScoordYYFXYCHAR2[4*(a+i)] = (1- fxx)*(1-fyy)*255;
//				VScoordYYFXYCHAR2[4*(a+i)+1] = fxx*(1-fyy)*255;
//				VScoordYYFXYCHAR2[4*(a+i)+2] = (1- fxx)*fyy*255;
//				VScoordYYFXYCHAR2[4*(a+i)+3] = fxx*fyy*255;
				coordVSYY2[(a+i)] = yy*ptBC_VSOpen->u32InputWidth + xx;

				if (i % 2 == 0 && j % 2 == 0)
				{
					coordVSUV[(j/2*ptBC_VSObject->virtualWidthOut/2+i/2)*2] = coordVSY[(j*ptBC_VSObject->virtualWidthOut+i)*2];//coordUV[(y/2*newWidth/2+i/2)*2];// = coordY[(j*newWidth+i)*2];
					coordVSUV[(j/2*ptBC_VSObject->virtualWidthOut/2+i/2)*2+1] = coordVSY[(j*ptBC_VSObject->virtualWidthOut+i)*2+1];//coordUV[(y/2*newWidth/2+i/2)*2+1];// = coordY[(j*newWidth+i)*2+1];
				}
			}
		}
	}

	ISPprintLog("------\n\n\n");
	for (j = 0; j < 200; j++)
	{
		ISPprintLog("weight 1-%d  2-%d  sum-%d\n",VScoordWEIGHT[2*j*ptBC_VSObject->virtualWidthOut],VScoordWEIGHT[2*j*ptBC_VSObject->virtualWidthOut+1],VScoordWEIGHT[2*j*ptBC_VSObject->virtualWidthOut]+VScoordWEIGHT[2*j*ptBC_VSObject->virtualWidthOut+1]);
	}

//��ά����תһά����
	for (j = 0; j < ptBC_VSObject->virtualHeightOut/2; j++)
	{
		a = j*ptBC_VSObject->virtualWidthOut/2;
		for (i = 0; i < ptBC_VSObject->virtualWidthOut/2; i++)
		{
				x=coordVSUV[(a+i)*2];
				y=coordVSUV[(a+i)*2+1];
				coordVSUUV[a + i] = y/2 * ptBC_VSOpen->u32InputWidth + x/2*2;
		}
	}



#if 1
	g1_exit_flag = 0;
	g2_exit_flag = 0;
	g3_exit_flag = 0;
	g4_exit_flag = 0;
	g5_exit_flag = 0;
	g6_exit_flag = 0;
	g7_exit_flag = 0;
	g8_exit_flag = 0;
	g9_exit_flag = 0;
	g10_exit_flag = 0;
	g11_exit_flag = 0;

	int ret = 0;
	ret = pthread_create(&g_hThread1, NULL, (void *)ProcessBCThread1, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread1 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread2, NULL, (void *)ProcessBCThread2, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread2 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread3, NULL, (void *)ProcessBCThread3, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread3 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread4, NULL, (void *)ProcessBCThread4, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread4 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread5, NULL, (void *)ProcessBCThread5, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread5 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread6, NULL, (void *)ProcessBCThread6, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread6 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread7, NULL, (void *)ProcessBCThread7, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread7 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread8, NULL, (void *)ProcessBCThread8, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread8 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread9, NULL, (void *)ProcessBCThread9, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread9 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread10, NULL, (void *)ProcessBCThread10, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread10 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread11, NULL, (void *)ProcessBCThread11, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread11 create failed.");
		return -1;
	}
    //CPU_SET(1, &mask);
#endif
	return SUCCESS_GPUALG;
}




int BC_VSProcess(TPlatformObject* ptPlatformObject,
					TBC_VSObject* ptBC_VSObject,
					TISPImageInfo* ptBC_VSInput, TISPImageInfo* ptBC_VSOutput)
{
	pSrcY = ptBC_VSInput->tImageBuffer[0].pu8ImageDataY;
	pSrcU = ptBC_VSInput->tImageBuffer[0].pu8ImageDataU;
	VSY = ptBC_VSOutput->tImageBuffer[0].pu8ImageDataY;
	VSUV = ptBC_VSOutput->tImageBuffer[0].pu8ImageDataU;

	TLDDataThread1OUTBool = 0;
	TLDDataThread2OUTBool = 0;
	TLDDataThread3OUTBool = 0;
	TLDDataThread4OUTBool = 0;
	TLDDataThread5OUTBool = 0;
	TLDDataThread6OUTBool = 0;
	TLDDataThread7OUTBool = 0;
	TLDDataThread1Bool = 1;
	TLDDataThread2Bool = 1;
	TLDDataThread3Bool = 1;
	TLDDataThread4Bool = 1;
	TLDDataThread5Bool = 1;
	TLDDataThread6Bool = 1;
	TLDDataThread7Bool = 1;
	pthread_mutex_lock(&mutex_thread1);
	pthread_cond_signal(&cond_thread1);
	pthread_mutex_unlock(&mutex_thread1);
	pthread_mutex_lock(&mutex_thread2);
	pthread_cond_signal(&cond_thread2);
	pthread_mutex_unlock(&mutex_thread2);
	pthread_mutex_lock(&mutex_thread3);
	pthread_cond_signal(&cond_thread3);
	pthread_mutex_unlock(&mutex_thread3);
	pthread_mutex_lock(&mutex_thread4);
	pthread_cond_signal(&cond_thread4);
	pthread_mutex_unlock(&mutex_thread4);
	pthread_mutex_lock(&mutex_thread5);
	pthread_cond_signal(&cond_thread5);
	pthread_mutex_unlock(&mutex_thread5);
	pthread_mutex_lock(&mutex_thread6);
	pthread_cond_signal(&cond_thread6);
	pthread_mutex_unlock(&mutex_thread6);
	pthread_mutex_lock(&mutex_thread7);
	pthread_cond_signal(&cond_thread7);
	pthread_mutex_unlock(&mutex_thread7);

#ifdef VSTHRE12
	TLDDataThread8OUTBool = 0;
	TLDDataThread9OUTBool = 0;
	TLDDataThread10OUTBool = 0;
	TLDDataThread11OUTBool = 0;
	TLDDataThread8Bool = 1;
	TLDDataThread9Bool = 1;
	TLDDataThread10Bool = 1;
	TLDDataThread11Bool = 1;
	pthread_mutex_lock(&mutex_thread8);
	pthread_cond_signal(&cond_thread8);
	pthread_mutex_unlock(&mutex_thread8);
	pthread_mutex_lock(&mutex_thread9);
	pthread_cond_signal(&cond_thread9);
	pthread_mutex_unlock(&mutex_thread9);
	pthread_mutex_lock(&mutex_thread10);
	pthread_cond_signal(&cond_thread10);
	pthread_mutex_unlock(&mutex_thread10);
	pthread_mutex_lock(&mutex_thread11);
	pthread_cond_signal(&cond_thread11);
	pthread_mutex_unlock(&mutex_thread11);
#endif
	ISPprintLog("[[[[[MAIN ALL start]]]]]\n");
	bcvs_convert0();
	pthread_mutex_lock(&mutex_main);
#ifdef VSTHRE12
	while((TLDDataThread1OUTBool == 0)||(TLDDataThread2OUTBool == 0)||(TLDDataThread3OUTBool == 0)||(TLDDataThread4OUTBool == 0)||(TLDDataThread5OUTBool == 0)||(TLDDataThread6OUTBool == 0)||(TLDDataThread7OUTBool == 0)
	||(TLDDataThread8OUTBool == 0)||(TLDDataThread9OUTBool == 0)||(TLDDataThread10OUTBool == 0)||(TLDDataThread11OUTBool == 0))
#else
	while((TLDDataThread1OUTBool == 0)||(TLDDataThread2OUTBool == 0)||(TLDDataThread3OUTBool == 0)||(TLDDataThread4OUTBool == 0)||(TLDDataThread5OUTBool == 0)||(TLDDataThread6OUTBool == 0)||(TLDDataThread7OUTBool == 0))
#endif
	{
		pthread_cond_wait(&cond_main,&mutex_main);
	}
	pthread_mutex_unlock(&mutex_main);
	ISPprintLog("[[[[[MAIN ALL END]]]]]\n");

	ptBC_VSOutput->tImageBuffer[0].u32Width = ptBC_VSObject->virtualWidthOut;
	ptBC_VSOutput->tImageBuffer[0].u32Height= ptBC_VSObject->virtualHeightOut;
	ptBC_VSOutput->tImageBuffer[0].u32PitchY= ptBC_VSObject->virtualWidthOut;
	ptBC_VSOutput->tImageBuffer[0].u32PitchUV= ptBC_VSObject->virtualWidthOut;
	return SUCCESS_GPUALG;
}

int BC_VSClose(TPlatformObject* ptPlatformObject, TBC_VSObject* ptBC_VSObject)
{
	free(ptBC_VSObject->pK);
	free(ptBC_VSObject->pWeight1);
	free(ptBC_VSObject->pWeight2);
	free(ptBC_VSObject->pKUV);
	free(ptBC_VSObject->pWeight1UV);
	free(ptBC_VSObject->pWeight2UV);
	free(ptBC_VSObject->pVertIdxShift);
	free(ptBC_VSObject->pCoordY);
	free(ptBC_VSObject->pCoordUV);

	free(coordVSY);
//	free(coordVSY2);
	free(coordVSUV);
	free(coordVSYY);
	free(coordVSYY2);
	free(coordVSUUV);
	free(VScoordYYFXYCHAR);
	free(VScoordYYFXYCHAR2);
	free(VScoordWEIGHT);

	free(pOrigXLow);
	free(pOrigYLow);
	free(pWeightX);
	free(pWeightY);

	ptBC_VSObject->pK = NULL;
	ptBC_VSObject->pWeight1 = NULL;
	ptBC_VSObject->pWeight2 = NULL;
	ptBC_VSObject->pKUV = NULL;
	ptBC_VSObject->pWeight1UV = NULL;
	ptBC_VSObject->pWeight2UV = NULL;
	ptBC_VSObject->pVertIdxShift = NULL;
	ptBC_VSObject->pCoordY = NULL;
	ptBC_VSObject->pCoordUV = NULL;

	coordVSY = NULL;
//	coordVSY2 = NULL;
	coordVSUV = NULL;
	coordVSYY = NULL;
	coordVSYY2 = NULL;
	coordVSUUV = NULL;
	VScoordYYFXYCHAR = NULL;
	VScoordYYFXYCHAR2 = NULL;
	VScoordWEIGHT = NULL;

	pOrigXLow = NULL;
	pOrigYLow = NULL;
	pWeightX == NULL;
	pWeightY== NULL;



	return 0;
}
