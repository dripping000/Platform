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


#define VS_PITCHY 2048
#define VS_PITCHYADD1 2049



#define CLIP(x) ((x)<0?0:(x)>255?255:(x))

unsigned char* pSrcYA;
unsigned char* pSrcUA;

unsigned char* VSYA = NULL;
unsigned char* VSUVA = NULL;
unsigned int* coordVSYYA = NULL;
unsigned int* coordVSYY2A = NULL;
unsigned int* coordVSUUVA = NULL;
unsigned char* VScoordYYFXYCHARA = NULL;
unsigned char* VScoordYYFXYCHAR2A = NULL;
unsigned char* VScoordWEIGHTA = NULL;

//float* coordVSYA = NULL;
//float* coordVSY2A = NULL;
unsigned int* coordVSUVA = NULL;

pthread_t g_hThread1A;
pthread_t g_hThread2A;
pthread_t g_hThread3A;
pthread_t g_hThread4A;
pthread_t g_hThread5A;
pthread_t g_hThread6A;
pthread_t g_hThread7A;
pthread_t g_hThread8A;
pthread_t g_hThread9A;
pthread_t g_hThread10A;
pthread_t g_hThread11A;

int g1A_exit_flag;
int g2A_exit_flag;
int g3A_exit_flag;
int g4A_exit_flag;
int g5A_exit_flag;
int g6A_exit_flag;
int g7A_exit_flag;
int g8A_exit_flag;
int g9A_exit_flag;
int g10A_exit_flag;
int g11A_exit_flag;

int TLDDataThread1ABool = 0;
int TLDDataThread2ABool = 0;
int TLDDataThread3ABool = 0;
int TLDDataThread4ABool = 0;
int TLDDataThread5ABool = 0;
int TLDDataThread6ABool = 0;
int TLDDataThread7ABool = 0;
int TLDDataThread8ABool = 0;
int TLDDataThread9ABool = 0;
int TLDDataThread10ABool = 0;
int TLDDataThread11ABool = 0;

int TLDDataThread1AOUTBool = 0;
int TLDDataThread2AOUTBool = 0;
int TLDDataThread3AOUTBool = 0;
int TLDDataThread4AOUTBool = 0;
int TLDDataThread5AOUTBool = 0;
int TLDDataThread6AOUTBool = 0;
int TLDDataThread7AOUTBool = 0;
int TLDDataThread8AOUTBool  = 0;
int TLDDataThread9AOUTBool = 0;
int TLDDataThread10AOUTBool = 0;
int TLDDataThread11AOUTBool = 0;

pthread_mutex_t mutex_thread1A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread1A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread2A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread2A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread3A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread3A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread4A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread4A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread5A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread5A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread6A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread6A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread7A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread7A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread8A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread8A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread9A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread9A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread10A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread10A  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_thread11A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_thread11A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_mainA = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mainA = PTHREAD_COND_INITIALIZER;


static int BarrelCorrectInitA(int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight, float i_f32Ratio[2],
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


static int VirtualStitchInitA(int width, int height, int widthOut, int heightOut,
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

float MAX_CUSTOMA(float a, float b)
{
    if(a>b)return a;
    else return b;
}
float MIN_CUSTOMA(float a, float b)
{
    if(a>b)return b;
    else return a;
}

float g_f32VertRatioA[VERT_SIZE];
int g_nVertIdxShiftA[VERT_SIZE];
float g_f32ScaleRatioA;

int *pOrigXLowA;
int *pOrigYLowA;
int *pWeightXA;
int *pWeightYA;

int *pOrigXLineA, *pOrigYLineA, *pWtXLineA, *pWtYLineA;
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

}KedaVirtualStitchAlgoParamA_T;

typedef struct KedaVirtualStitchProcessParam
{
	s32 m_s32ImgWidth;
	s32 m_s32ImgHeight;
	s32 m_s32FinalWidth;
	s32 m_s32FinalHeight;
	KedaVirtualStitchAlgoParamA_T m_tAlgoParam;

}KedaVirtualStitchProcessParamB_T;

int VirtualStitchParaInitA(int width, int height,
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

        f32Ratio = MAX_CUSTOMA(f32Ratio, 0);

        nFinalWidth = (f32Ratio*f32OrgTopWidth+(1.0-MIN_CUSTOMA(f32Ratio,1.0))*width);
        g_f32VertRatioA[nI] = ((float)width)/((float)nFinalWidth);
	}
	for(nI=height-1;nI>=0;nI--)
    {
        if(g_f32VertRatioA[nI] <=1.0000001 || nI==height-1)
        {
            f32VertZoomStart = nI;
            g_nVertIdxShiftA[nI] = nI<<10;

        }
        else
        {
            f32VertZoomStart -= g_f32VertRatioA[nI];//���ո�ֵ
            g_nVertIdxShiftA[nI] = (int)(f32VertZoomStart*1024.0);

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
		g_f32ScaleRatioA = (float)finalHeight / (float)nOutH;
	else
		g_f32ScaleRatioA = 1.0;

	for(nI=height-1;nI>=0;nI--)
    {
        if(g_f32VertRatioA[nI] <=1.0000001 || nI==height-1)
        {
            f32VertZoomStart -= 1;
        }
        else
        {
            f32VertZoomStart -= g_f32VertRatioA[nI];//���ո�ֵ
        }
		if (f32VertZoomStart>nMidPoint)
			f32HoriRatioComp =  maxRatio + f32DownPartRatio*(f32VertZoomStart-nMidPoint);
		else
			f32HoriRatioComp =f32HoriRatioComp +  f32UpPartRatio*MIN_CUSTOMA(exp((f32VertZoomStart-nMidPoint+100.0)/100.0),1.0);//���۲⣺��������100.0������ȡΪ���Ʋ���

		g_f32VertRatioA[nI] *= (f32HoriRatioComp*g_f32ScaleRatioA*f32WidthAdjRatio);

	}

	nOutH = nOutH*g_f32ScaleRatioA;
	*pOutWidth = (*pOutWidth)*g_f32ScaleRatioA;
	*pOutWidth = (*pOutWidth/8)*8;
	*pOutHeight = nOutH;

//	for(nI=0;nI<height;nI++)
//		fprintf(pParaFile,"%d\n", g_nVertIdxShiftA[nI]);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
int VirtualStitchParaProcessA(int width, int height,
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

 /*  pOrigXLowA = (int*)malloc(i_nFinalWidth*height*sizeof(int));
   pOrigYLowA = (int*)malloc(i_nFinalWidth*height*sizeof(int));
   pWeightXA = (int*)malloc(i_nFinalWidth*height*sizeof(int));
   pWeightYA = (int*)malloc(i_nFinalWidth*height*sizeof(int));*/

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

        pOrigXLineA = pOrigXLowA+i_nFinalWidth*nI;
        pOrigYLineA = pOrigYLowA+i_nFinalWidth*nI;
        pWtXLineA = pWeightXA+i_nFinalWidth*nI;
        pWtYLineA = pWeightYA+i_nFinalWidth*nI;

        nFinalWidth = (int)(((float)width)/(g_f32VertRatioA[nI]));

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
				pOrigXLineA[nJ-nLeftStart] = nTempCor1>>10;
				pOrigYLineA[nJ-nLeftStart] = nTempCor2>>10;
//
				pWtXLineA[nJ-nLeftStart]=nTempCor1-(pOrigXLineA[nJ-nLeftStart]<<10);
				pWtYLineA[nJ-nLeftStart]=nTempCor2-(pOrigYLineA[nJ-nLeftStart]<<10);

//				fprintf(fp,"%d,%d,%d,%d\n",pOrigXLineA[nJ-nLeftStart],pOrigYLineA[nJ-nLeftStart],pWtXLineA[nJ-nLeftStart],pWtYLineA[nJ-nLeftStart]);
			}

        }
    }

/*   free(pOrigXLowA);
   free(pOrigYLowA);
   free(pWeightXA);
   free(pWeightYA);*/
    return 0;
}

void param_processA(KedaVirtualStitchProcessParamB_T i_tParam)
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

    VirtualStitchParaInitA(nMidWidth, nMidHeight, f32MaxZoomIn, nAdjustEndHeight, nAdjustStartHeight, &nOutWidth,&nOutHeight,
		i_tParam.m_tAlgoParam.m_s32TeleCurvLevel,i_tParam.m_tAlgoParam.m_s32TeleMultVal,maxRatio,i_tParam.m_s32FinalHeight,i_tParam.m_tAlgoParam.m_f32WidthScale);
  	VirtualStitchParaProcessA( nMidWidth, nMidHeight, nOutWidth,nOutHeight, f32Ratio,i_tParam.m_s32ImgWidth, i_tParam.m_s32ImgHeight,i_tParam.m_s32FinalWidth);
}

int BC_VSParamsInitA(TBC_VSOpen* ptBarrelCorrectionOpen)
{
	KedaVirtualStitchProcessParamB_T tVirtualStitchParam;
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

	param_processA(tVirtualStitchParam);
	return 0;
}

void bcvs_convert0A ()
{
	int j;

#ifdef VSTHRE12
	for (j = 2048*1536/12*0; j < 2048*1536/12*1; j += 8)
#else
	for (j = 2048*1536/8*0; j < 2048*1536/8*1; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}


	int a;
#ifdef VSTHRE12
	for (j = 0; j < 2048*1536/48; j++)
#else
	for (j = 0; j < 2048*1536/32; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert1A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*1; j < 2048*1536/12*2; j += 8)
#else
	for (j = 2048*1536/8*1; j < 2048*1536/8*2; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*1; j < 2048*1536/48*2; j++)
#else
	for (j = 2048*1536/32*1; j < 2048*1536/32*2; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert2A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*2; j < 2048*1536/12*3; j += 8)
#else
	for (j = 2048*1536/8*2; j < 2048*1536/8*3; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*2; j < 2048*1536/48*3; j++)
#else
	for (j = 2048*1536/32*2; j < 2048*1536/32*3; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert3A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*3; j < 2048*1536/12*4; j += 8)
#else
	for (j = 2048*1536/8*3; j < 2048*1536/8*4; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*3; j < 2048*1536/48*4; j++)
#else
	for (j = 2048*1536/32*3; j < 2048*1536/32*4; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert4A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*4; j < 2048*1536/12*5; j += 8)
#else
	for (j = 2048*1536/8*4; j < 2048*1536/8*5; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*4; j < 2048*1536/48*5; j++)
#else
	for (j = 2048*1536/32*4; j < 2048*1536/32*5; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert5A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*5; j < 2048*1536/12*6; j += 8)
#else
	for (j = 2048*1536/8*5; j < 2048*1536/8*6; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*5; j < 2048*1536/48*6; j++)
#else
	for (j = 2048*1536/32*5; j < 2048*1536/32*6; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert6A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*6; j < 2048*1536/12*7; j += 8)
#else
	for (j = 2048*1536/8*6; j < 2048*1536/8*7; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*6; j < 2048*1536/48*7; j++)
#else
	for (j = 2048*1536/32*6; j < 2048*1536/32*7; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert7A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*7; j < 2048*1536/12*8; j += 8)
#else
	for (j = 2048*1536/8*7; j < 2048*1536/8*8; j += 8)
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*7; j < 2048*1536/48*8; j++)
#else
	for (j = 2048*1536/32*7; j < 2048*1536/32*8; j++)
#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert8A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*8; j < 2048*1536/12*9; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*8; j < 2048*1536/48*9; j++)
#else

#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}
void bcvs_convert9A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*9; j < 2048*1536/12*10; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*9; j < 2048*1536/48*10; j++)
#else

#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert10A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*10; j < 2048*1536/12*11; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHYADD1],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*10; j < 2048*1536/48*11; j++)
#else

#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}

void bcvs_convert11A ()
{
	int j;
#ifdef VSTHRE12
	for (j = 2048*1536/12*11; j < 2048*1536/12*12; j += 8)
#else

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(VScoordYYFXYCHARA+j*4);
		uint8x8x4_t	fxy2 = vld4_u8(VScoordYYFXYCHAR2A+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;
		uint8x8_t x0y02;
		uint8x8_t x1y02;
		uint8x8_t x0y12;
		uint8x8_t x1y12;

		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j]+VS_PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+1]+VS_PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+2]+VS_PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+3]+VS_PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+4]+VS_PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+5]+VS_PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+6]+VS_PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYA[coordVSYYA[j+7]+VS_PITCHYADD1],x1y1,7);

		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]],x0y02,0);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]],x0y02,1);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]],x0y02,2);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]],x0y02,3);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]],x0y02,4);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]],x0y02,5);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]],x0y02,6);
		x0y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]],x0y02,7);

		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+1],x1y02,0);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+1],x1y02,1);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+1],x1y02,2);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+1],x1y02,3);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+1],x1y02,4);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+1],x1y02,5);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+1],x1y02,6);
		x1y02 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+1],x1y02,7);

		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHY],x0y12,0);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHY],x0y12,1);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHY],x0y12,2);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHY],x0y12,3);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHY],x0y12,4);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHY],x0y12,5);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHY],x0y12,6);
		x0y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x0y12,7);

		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j]+VS_PITCHYADD1],x1y12,0);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+1]+VS_PITCHYADD1],x1y12,1);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+2]+VS_PITCHYADD1],x1y12,2);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+3]+VS_PITCHYADD1],x1y12,3);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+4]+VS_PITCHYADD1],x1y12,4);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+5]+VS_PITCHYADD1],x1y12,5);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+6]+VS_PITCHYADD1],x1y12,6);
		x1y12 = vset_lane_u8(pSrcYA[coordVSYY2A[j+7]+VS_PITCHY],x1y12,7);

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
    	vst1_u8 (VSYA+j, result);
	}

	int a;
#ifdef VSTHRE12
	for (j = 2048*1536/48*11; j < 2048*1536/48*12; j++)
#else

#endif
	{
		a=coordVSUUVA[j];
		VSUVA[(j<<1)] = pSrcUA[a];
		VSUVA[(j<<1) +1] = pSrcUA[a+1];
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////


void ProcessBCThread1A()
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

	while(0 == g1A_exit_flag)
	{
		ISPprintLog("ProcessBCThread1A enter\n");
		pthread_mutex_lock(&mutex_thread1A);
		while(TLDDataThread1ABool == 0)
		{
			pthread_cond_wait(&cond_thread1A,&mutex_thread1A);
		}
		pthread_mutex_unlock(&mutex_thread1A);
		TLDDataThread1ABool=0;
		ISPprintLog("ProcessBCThread1A start\n");

		bcvs_convert1A();

		TLDDataThread1AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
	}
}



void ProcessBCThread2A()
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

    while(0 == g2A_exit_flag)
	{
		ISPprintLog("ProcessBCThread2A enter\n");

		pthread_mutex_lock(&mutex_thread2A);
		while(TLDDataThread2ABool == 0)
		{
			pthread_cond_wait(&cond_thread2A,&mutex_thread2A);
		}
		pthread_mutex_unlock(&mutex_thread2A);
		TLDDataThread2ABool=0;
		ISPprintLog("ProcessBCThread2A start\n");

		bcvs_convert2A();

		TLDDataThread2AOUTBool = 1;
		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
	}
}



void ProcessBCThread3A()
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

    while(0 == g3A_exit_flag)
	{
		ISPprintLog("ProcessBCThread3A enter\n");

		pthread_mutex_lock(&mutex_thread3A);
		while(TLDDataThread3ABool == 0)
		{
			pthread_cond_wait(&cond_thread3A,&mutex_thread3A);
		}
		pthread_mutex_unlock(&mutex_thread3A);
		TLDDataThread3ABool=0;
		ISPprintLog("ProcessBCThread3A start\n");

		bcvs_convert3A();

		TLDDataThread3AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
	}
}



void ProcessBCThread4A()
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

    while(0 == g4A_exit_flag)
	{
		ISPprintLog("ProcessBCThread4A enter\n");

		pthread_mutex_lock(&mutex_thread4A);
		while(TLDDataThread4ABool == 0)
		{
			pthread_cond_wait(&cond_thread4A,&mutex_thread4A);
		}
		pthread_mutex_unlock(&mutex_thread4A);
		TLDDataThread4ABool=0;
		ISPprintLog("ProcessBCThread4A start\n");

		bcvs_convert4A();

		TLDDataThread4AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);

	}
}

void ProcessBCThread5A()
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

    while(0 == g5A_exit_flag)
	{
		ISPprintLog("ProcessBCThread5A enter\n");

		pthread_mutex_lock(&mutex_thread5A);
		while(TLDDataThread5ABool == 0)
		{
			pthread_cond_wait(&cond_thread5A,&mutex_thread5A);
		}
		pthread_mutex_unlock(&mutex_thread5A);
		TLDDataThread5ABool=0;

		ISPprintLog("ProcessBCThread5A start\n");

		bcvs_convert5A();

		TLDDataThread5AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
		}
}

void ProcessBCThread6A()
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

    while(0 == g6A_exit_flag)
	{
		ISPprintLog("ProcessBCThread6A enter\n");

		pthread_mutex_lock(&mutex_thread6A);
		while(TLDDataThread6ABool == 0)
		{
			pthread_cond_wait(&cond_thread6A,&mutex_thread6A);
		}
		pthread_mutex_unlock(&mutex_thread6A);
		TLDDataThread6ABool=0;

		ISPprintLog("ProcessBCThread6A start\n");

		bcvs_convert6A();

		TLDDataThread6AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
		}
}

void ProcessBCThread7A()
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

    while(0 == g7A_exit_flag)
	{
		ISPprintLog("ProcessBCThread7A enter\n");

		pthread_mutex_lock(&mutex_thread7A);
		while(TLDDataThread7ABool == 0)
		{
			pthread_cond_wait(&cond_thread7A,&mutex_thread7A);
		}
		pthread_mutex_unlock(&mutex_thread7A);
		TLDDataThread7ABool=0;

		ISPprintLog("ProcessBCThread7A start\n");

		bcvs_convert7A();


		TLDDataThread7AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
		}
}

void ProcessBCThread8A()
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

    while(0 == g8A_exit_flag)
	{
		ISPprintLog("ProcessBCThread8A enter\n");

		pthread_mutex_lock(&mutex_thread8A);
		while(TLDDataThread8ABool == 0)
		{
			pthread_cond_wait(&cond_thread8A,&mutex_thread8A);
		}
		pthread_mutex_unlock(&mutex_thread8A);
		TLDDataThread8ABool=0;

		ISPprintLog("ProcessBCThread8A start\n");

		bcvs_convert8A();

		TLDDataThread8AOUTBool  = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
		}
}


void ProcessBCThread9A()
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

    while(0 == g9A_exit_flag)
	{
		ISPprintLog("ProcessBCThread9A enter\n");

		pthread_mutex_lock(&mutex_thread9A);
		while(TLDDataThread9ABool == 0)
		{
			pthread_cond_wait(&cond_thread9A,&mutex_thread9A);
		}
		pthread_mutex_unlock(&mutex_thread9A);
		TLDDataThread9ABool=0;

		ISPprintLog("ProcessBCThread9A start\n");

		bcvs_convert9A();

		TLDDataThread9AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
		}
}


void ProcessBCThread10A()
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

    while(0 == g10A_exit_flag)
	{
		ISPprintLog("ProcessBCThread10A enter\n");

		pthread_mutex_lock(&mutex_thread10A);
		while(TLDDataThread10ABool == 0)
		{
			pthread_cond_wait(&cond_thread10A ,&mutex_thread10A);
		}
		pthread_mutex_unlock(&mutex_thread10A);
		TLDDataThread10ABool=0;

		ISPprintLog("ProcessBCThread10A start\n");

		bcvs_convert10A();

		TLDDataThread10AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
		}
}


void ProcessBCThread11A()
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

    while(0 == g11A_exit_flag)
	{
		ISPprintLog("ProcessBCThread11A enter\n");

		pthread_mutex_lock(&mutex_thread11A);
		while(TLDDataThread11ABool == 0)
		{
			pthread_cond_wait(&cond_thread11A,&mutex_thread11A);
		}
		pthread_mutex_unlock(&mutex_thread11A);
		TLDDataThread11ABool=0;

		ISPprintLog("ProcessBCThread11A start\n");

		bcvs_convert11A();

		TLDDataThread11AOUTBool = 1;

		pthread_mutex_lock(&mutex_mainA);
		pthread_cond_signal(&cond_mainA);
		pthread_mutex_unlock(&mutex_mainA);
		}
}


int BC_VSOpenA(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject,
				TBC_VSObject* ptBC_VSObject, TBC_VSOpen* ptBC_VSOpen)
{

     ISPprintLog("TBC_VSOpenA===========>1.InputWidth:%d\n,2.InputHeight:%d\n,",ptBC_VSOpen->u32InputWidth,ptBC_VSOpen->u32InputHeight);
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

    pOrigXLowA = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));
    pOrigYLowA = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));
    pWeightXA = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));
    pWeightYA = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*widthOut*sizeof(int));

  	if ( (NULL == pOrigXLowA) ||(NULL == pOrigYLowA)||(NULL == pWeightXA)||(NULL == pWeightXA) )
	{
		printf("malloc memory 1 fail !\n");
		return ERR_CREATE_HOST_BUFFER;
	}

   	p1 = pOrigXLowA;
	p2 = pOrigYLowA;
	p3 = pWeightXA;
	p4 = pWeightYA;

    BC_VSParamsInitA(ptBC_VSOpen);


	scale = g_f32ScaleRatioA;
	ptBC_VSObject->pVertIdxShift = (int*)malloc(ptBC_VSOpen->tBC_VSParam.s32MidHeight*sizeof(int));

	if (NULL == ptBC_VSObject->pVertIdxShift )
	{
		printf("malloc memory 2 fail!\n");
		return ERR_CREATE_HOST_BUFFER;
	}


	ISPprintLog("vertIdxShift[1000] = %d\n", g_nVertIdxShiftA[1000]);

	newWidth = widthOut;
	newHeight =  ptBC_VSOpen->tBC_VSParam.s32MidHeight;

	coordY = (float*)malloc(newWidth*newHeight*sizeof(float)*2);
	coordUV = (float*)malloc(newWidth*(newHeight+8)*sizeof(float)*2/4);

	if ( (NULL == coordY) ||(NULL == coordUV))
	{
		printf("malloc memory 3 fail!\n");
		return ERR_CREATE_HOST_BUFFER;
	}

	ptBC_VSObject->virtualWidthOut = ptBC_VSOpen->u32OutputWidth;
	ptBC_VSObject->virtualHeightOut = ptBC_VSOpen->u32OutputHeight;

	//coordVSYA = (float*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(float)*2);
	//coordVSY2A = (float*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(float)*2);
	coordVSUVA = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int)*2/4);

	coordVSYYA = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int));
	coordVSYY2A = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int));
	coordVSUUVA = (unsigned int*)malloc(ptBC_VSObject->virtualWidthOut* ptBC_VSObject->virtualHeightOut*sizeof(unsigned int)*2/4/2);

	if ( (NULL == coordVSUVA) ||(NULL == coordVSYYA)||(NULL == coordVSYY2A )||(NULL == coordVSUUVA) )
	{
		printf("malloc memory 4 fail!\n");
		return ERR_CREATE_HOST_BUFFER;
	}

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

	if ( (NULL == ptBC_VSObject->pK) ||(NULL == ptBC_VSObject->pWeight1)||(NULL == ptBC_VSObject->pWeight2 ) )
	{
		printf("malloc memory 5 fail!\n");
		return ERR_CREATE_HOST_BUFFER;
	}

	ptBC_VSObject->pKUV = (int*)malloc(newHeight/2*sizeof(int));
	ptBC_VSObject->pWeight1UV = (int*)malloc(newHeight/2*sizeof(int));
	ptBC_VSObject->pWeight2UV = (int*)malloc(newHeight/2*sizeof(int));

	if ( (NULL == ptBC_VSObject->pKUV) ||(NULL == ptBC_VSObject->pWeight1UV)||(NULL == ptBC_VSObject->pWeight2UV ) )
	{
		printf("malloc memory 6 fail!\n");
		return ERR_CREATE_HOST_BUFFER;
	}



//	ptBC_VSObject->pVirtualImgOutY = (unsigned char*)malloc(ptBarrelCorrectionObject->virtualWidthOut * ptBarrelCorrectionObject->virtualHeightOut* sizeof(unsigned char));
//	ptBC_VSObject->pVirtualImgOutUV = (unsigned char*)malloc(ptBarrelCorrectionObject->virtualWidthOut * ptBarrelCorrectionObject->virtualHeightOut* sizeof(unsigned char) / 2);

/*	VirtualStitchInitA(newWidth, newHeight, ptBarrelCorrectionOpen->u32OutputWidth, ptBarrelCorrectionOpen->u32OutputHeight,
		scale, tBarrelCorrectionObject.pVertRatio, tBarrelCorrectionObject.pVertIdxShift,
		tBarrelCorrectionObject.pVirtualCoordY, tBarrelCorrectionObject.pVirtualCoordUV,
		tBarrelCorrectionObject.pK,tBarrelCorrectionObject.pWeight1, tBarrelCorrectionObject.pWeight2,
		tBarrelCorrectionObject.pKUV,tBarrelCorrectionObject.pWeight1UV, tBarrelCorrectionObject.pWeight2UV);*/

	ISPprintLog("VirtualStitchInitA start!\n");
	VirtualStitchInitA(newWidth, newHeight, ptBC_VSOpen->u32OutputWidth, ptBC_VSOpen->u32OutputHeight,
	scale,g_nVertIdxShiftA,ptBC_VSObject->pK,ptBC_VSObject->pWeight1, ptBC_VSObject->pWeight2,
		ptBC_VSObject->pKUV,ptBC_VSObject->pWeight1UV, ptBC_VSObject->pWeight2UV);
	ISPprintLog("VirtualStitchInitA end!\n");

	ISPprintLog("coordination start!\n");


	VScoordYYFXYCHARA = (unsigned char*)malloc(ptBC_VSObject->virtualWidthOut * ptBC_VSObject->virtualHeightOut * 4 * sizeof(unsigned char));
	VScoordYYFXYCHAR2A = (unsigned char*)malloc(ptBC_VSObject->virtualWidthOut * ptBC_VSObject->virtualHeightOut * 4 * sizeof(unsigned char));
	VScoordWEIGHTA = (unsigned char*)malloc(ptBC_VSObject->virtualWidthOut * ptBC_VSObject->virtualHeightOut * 2 * sizeof(unsigned char));

	if ( (NULL == VScoordYYFXYCHARA) ||(NULL == VScoordYYFXYCHAR2A)||(NULL == VScoordWEIGHTA ) )
	{
		printf("malloc memory 7 fail !\n");
		return ERR_CREATE_HOST_BUFFER;
	}


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
			VScoordWEIGHTA[2*(a+i)] = weight1;
			VScoordWEIGHTA[2*(a+i)+1] = weight2;

			if(x>=0)
			{
				//coordVSYA[(j*ptBC_VSObject->virtualWidthOut+i)*2] = coordY[(x*newWidth+i)*2];
				//coordVSYA[(j*ptBC_VSObject->virtualWidthOut+i)*2+1] = coordY[(x*newWidth+i)*2+1];
				//coordVSY2A[(j*ptBC_VSObject->virtualWidthOut+i)*2] = coordY[(x2*newWidth+i)*2];
				//coordVSY2A[(j*ptBC_VSObject->virtualWidthOut+i)*2+1] = coordY[(x2*newWidth+i)*2+1];

				xx = coordY[(x*newWidth+i)*2];
				yy = coordY[(x*newWidth+i)*2+1];
				fxx = coordY[(x*newWidth+i)*2] - xx;
				fyy = coordY[(x*newWidth+i)*2+1] - yy;
				VScoordYYFXYCHARA[4*(a+i)] = (1- fxx)*(1-fyy)*weight1;
				VScoordYYFXYCHARA[4*(a+i)+1] = fxx*(1-fyy)*weight1;
				VScoordYYFXYCHARA[4*(a+i)+2] = (1- fxx)*fyy*weight1;
				VScoordYYFXYCHARA[4*(a+i)+3] = fxx*fyy*weight1;
//				VScoordYYFXYCHARA[4*(a+i)] = (1- fxx)*(1-fyy)*255;
//				VScoordYYFXYCHARA[4*(a+i)+1] = fxx*(1-fyy)*255;
//				VScoordYYFXYCHARA[4*(a+i)+2] = (1- fxx)*fyy*255;
//				VScoordYYFXYCHARA[4*(a+i)+3] = fxx*fyy*255;
				coordVSYYA[(a+i)] = yy*ptBC_VSOpen->u32InputWidth + xx;

				xx = coordY[(x2*newWidth+i)*2];
				yy = coordY[(x2*newWidth+i)*2+1];
				fxx = coordY[(x2*newWidth+i)*2] - xx;
				fyy = coordY[(x2*newWidth+i)*2+1] - yy;
				VScoordYYFXYCHAR2A[4*(a+i)] = (1- fxx)*(1-fyy)*weight2;
				VScoordYYFXYCHAR2A[4*(a+i)+1] = fxx*(1-fyy)*weight2;
				VScoordYYFXYCHAR2A[4*(a+i)+2] = (1- fxx)*fyy*weight2;
				VScoordYYFXYCHAR2A[4*(a+i)+3] = fxx*fyy*weight2;
//				VScoordYYFXYCHAR2A[4*(a+i)] = (1- fxx)*(1-fyy)*255;
//				VScoordYYFXYCHAR2A[4*(a+i)+1] = fxx*(1-fyy)*255;
//				VScoordYYFXYCHAR2A[4*(a+i)+2] = (1- fxx)*fyy*255;
//				VScoordYYFXYCHAR2A[4*(a+i)+3] = fxx*fyy*255;
				coordVSYY2A[(a+i)] = yy*ptBC_VSOpen->u32InputWidth + xx;

				if (i % 2 == 0 && j % 2 == 0)
				{
					//coordVSUVA[(j/2*ptBC_VSObject->virtualWidthOut/2+i/2)*2] = coordVSYA[(j*ptBC_VSObject->virtualWidthOut+i)*2];//coordUV[(y/2*newWidth/2+i/2)*2];// = coordY[(j*newWidth+i)*2];
					//coordVSUVA[(j/2*ptBC_VSObject->virtualWidthOut/2+i/2)*2+1] = coordVSYA[(j*ptBC_VSObject->virtualWidthOut+i)*2+1];//coordUV[(y/2*newWidth/2+i/2)*2+1];// = coordY[(j*newWidth+i)*2+1];
					coordVSUVA[(j/2*ptBC_VSObject->virtualWidthOut/2+i/2)*2] = coordY[(x*newWidth+i)*2];
					coordVSUVA[(j/2*ptBC_VSObject->virtualWidthOut/2+i/2)*2+1] = coordY[(x*newWidth+i)*2+1];
				}
			}
		}
	}

	ISPprintLog("------\n\n\n");
	for (j = 0; j < 200; j++)
	{
		ISPprintLog("weight 1-%d  2-%d  sum-%d\n",VScoordWEIGHTA[2*j*ptBC_VSObject->virtualWidthOut],VScoordWEIGHTA[2*j*ptBC_VSObject->virtualWidthOut+1],VScoordWEIGHTA[2*j*ptBC_VSObject->virtualWidthOut]+VScoordWEIGHTA[2*j*ptBC_VSObject->virtualWidthOut+1]);
	}

//��ά����תһά����
	for (j = 0; j < ptBC_VSObject->virtualHeightOut/2; j++)
	{
		a = j*ptBC_VSObject->virtualWidthOut/2;
		for (i = 0; i < ptBC_VSObject->virtualWidthOut/2; i++)
		{
				x=coordVSUVA[(a+i)*2];
				y=coordVSUVA[(a+i)*2+1];
				coordVSUUVA[a + i] = y/2 * ptBC_VSOpen->u32InputWidth + x/2*2;
		}
	}


	g1A_exit_flag = 0;
	g2A_exit_flag = 0;
	g3A_exit_flag = 0;
	g4A_exit_flag = 0;
	g5A_exit_flag = 0;
	g6A_exit_flag = 0;
	g7A_exit_flag = 0;


	int ret = 0;
	ret = pthread_create(&g_hThread1A, NULL, (void *)ProcessBCThread1A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread1A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread2A, NULL, (void *)ProcessBCThread2A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread2A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread3A, NULL, (void *)ProcessBCThread3A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread3A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread4A, NULL, (void *)ProcessBCThread4A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread4A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread5A, NULL, (void *)ProcessBCThread5A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread5A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread6A, NULL, (void *)ProcessBCThread6A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread6A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread7A, NULL, (void *)ProcessBCThread7A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread7A create failed.");
		return -1;
	}

	#ifdef VSTHRE12

	g8A_exit_flag = 0;
	g9A_exit_flag = 0;
	g10A_exit_flag = 0;
	g11A_exit_flag = 0;

	ret = pthread_create(&g_hThread8A, NULL, (void *)ProcessBCThread8A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread8A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread9A, NULL, (void *)ProcessBCThread9A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread9A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread10A, NULL, (void *)ProcessBCThread10A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread10A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread11A, NULL, (void *)ProcessBCThread11A, NULL);
	if (ret != 0)
	{
		printf("ProcessBCThread11A create failed.");
		return -1;
	}
    //CPU_SET(1, &mask);
#endif
	return SUCCESS_GPUALG;
}




int BC_VSProcessA(TPlatformObject* ptPlatformObject,
					TBC_VSObject* ptBC_VSObject,
					TISPImageInfo* ptBC_VSInput, TISPImageInfo* ptBC_VSOutput)
{
	pSrcYA = ptBC_VSInput->tImageBuffer[0].pu8ImageDataY;
	pSrcUA = ptBC_VSInput->tImageBuffer[0].pu8ImageDataU;
	VSYA = ptBC_VSOutput->tImageBuffer[0].pu8ImageDataY;
	VSUVA = ptBC_VSOutput->tImageBuffer[0].pu8ImageDataU;

	TLDDataThread1AOUTBool = 0;
	TLDDataThread2AOUTBool = 0;
	TLDDataThread3AOUTBool = 0;
	TLDDataThread4AOUTBool = 0;
	TLDDataThread5AOUTBool = 0;
	TLDDataThread6AOUTBool = 0;
	TLDDataThread7AOUTBool = 0;
	TLDDataThread1ABool = 1;
	TLDDataThread2ABool = 1;
	TLDDataThread3ABool = 1;
	TLDDataThread4ABool = 1;
	TLDDataThread5ABool = 1;
	TLDDataThread6ABool = 1;
	TLDDataThread7ABool = 1;
	pthread_mutex_lock(&mutex_thread1A);
	pthread_cond_signal(&cond_thread1A);
	pthread_mutex_unlock(&mutex_thread1A);
	pthread_mutex_lock(&mutex_thread2A);
	pthread_cond_signal(&cond_thread2A);
	pthread_mutex_unlock(&mutex_thread2A);
	pthread_mutex_lock(&mutex_thread3A);
	pthread_cond_signal(&cond_thread3A);
	pthread_mutex_unlock(&mutex_thread3A);
	pthread_mutex_lock(&mutex_thread4A);
	pthread_cond_signal(&cond_thread4A);
	pthread_mutex_unlock(&mutex_thread4A);
	pthread_mutex_lock(&mutex_thread5A);
	pthread_cond_signal(&cond_thread5A);
	pthread_mutex_unlock(&mutex_thread5A);
	pthread_mutex_lock(&mutex_thread6A);
	pthread_cond_signal(&cond_thread6A);
	pthread_mutex_unlock(&mutex_thread6A);
	pthread_mutex_lock(&mutex_thread7A);
	pthread_cond_signal(&cond_thread7A);
	pthread_mutex_unlock(&mutex_thread7A);

#ifdef VSTHRE12
	TLDDataThread8AOUTBool  = 0;
	TLDDataThread9AOUTBool = 0;
	TLDDataThread10AOUTBool = 0;
	TLDDataThread11AOUTBool = 0;
	TLDDataThread8ABool = 1;
	TLDDataThread9ABool = 1;
	TLDDataThread10ABool = 1;
	TLDDataThread11ABool = 1;
	pthread_mutex_lock(&mutex_thread8A);
	pthread_cond_signal(&cond_thread8A);
	pthread_mutex_unlock(&mutex_thread8A);
	pthread_mutex_lock(&mutex_thread9A);
	pthread_cond_signal(&cond_thread9A);
	pthread_mutex_unlock(&mutex_thread9A);
	pthread_mutex_lock(&mutex_thread10A);
	pthread_cond_signal(&cond_thread10A );
	pthread_mutex_unlock(&mutex_thread10A);
	pthread_mutex_lock(&mutex_thread11A);
	pthread_cond_signal(&cond_thread11A);
	pthread_mutex_unlock(&mutex_thread11A);
#endif
	ISPprintLog("[[[[[MAIN ALL start]]]]]\n");
	bcvs_convert0A();
	pthread_mutex_lock(&mutex_mainA);
#ifdef VSTHRE12
	while((TLDDataThread1AOUTBool == 0)||(TLDDataThread2AOUTBool == 0)||(TLDDataThread3AOUTBool == 0)||(TLDDataThread4AOUTBool == 0)||(TLDDataThread5AOUTBool == 0)||(TLDDataThread6AOUTBool == 0)||(TLDDataThread7AOUTBool == 0)
	||(TLDDataThread8AOUTBool  == 0)||(TLDDataThread9AOUTBool == 0)||(TLDDataThread10AOUTBool == 0)||(TLDDataThread11AOUTBool == 0))
#else
	while((TLDDataThread1AOUTBool == 0)||(TLDDataThread2AOUTBool == 0)||(TLDDataThread3AOUTBool == 0)||(TLDDataThread4AOUTBool == 0)||(TLDDataThread5AOUTBool == 0)||(TLDDataThread6AOUTBool == 0)||(TLDDataThread7AOUTBool == 0))
#endif
	{
		pthread_cond_wait(&cond_mainA,&mutex_mainA);
	}
	pthread_mutex_unlock(&mutex_mainA);
	ISPprintLog("[[[[[MAIN ALL END]]]]]\n");

	ptBC_VSOutput->tImageBuffer[0].u32Width = ptBC_VSObject->virtualWidthOut;
	ptBC_VSOutput->tImageBuffer[0].u32Height= ptBC_VSObject->virtualHeightOut;
	ptBC_VSOutput->tImageBuffer[0].u32PitchY= ptBC_VSObject->virtualWidthOut;
	ptBC_VSOutput->tImageBuffer[0].u32PitchUV= ptBC_VSObject->virtualWidthOut;
	return SUCCESS_GPUALG;
}

int BC_VSCloseA(TPlatformObject* ptPlatformObject, TBC_VSObject* ptBC_VSObject)
{
	int ret = 0;

	g1A_exit_flag = 1;
	g2A_exit_flag = 1;
	g3A_exit_flag = 1;
	g4A_exit_flag = 1;
	g5A_exit_flag = 1;
	g6A_exit_flag = 1;
	g7A_exit_flag = 1;
	g8A_exit_flag = 1;
	g9A_exit_flag = 1;
	g10A_exit_flag = 1;
	g11A_exit_flag = 1;


	TLDDataThread1AOUTBool = 0;
	TLDDataThread2AOUTBool = 0;
	TLDDataThread3AOUTBool = 0;
	TLDDataThread4AOUTBool = 0;
	TLDDataThread5AOUTBool = 0;
	TLDDataThread6AOUTBool = 0;
	TLDDataThread7AOUTBool = 0;
	TLDDataThread1ABool = 1;
	TLDDataThread2ABool = 1;
	TLDDataThread3ABool = 1;
	TLDDataThread4ABool = 1;
	TLDDataThread5ABool = 1;
	TLDDataThread6ABool = 1;
	TLDDataThread7ABool = 1;
	pthread_mutex_lock(&mutex_thread1A);
	pthread_cond_signal(&cond_thread1A);
	pthread_mutex_unlock(&mutex_thread1A);
	pthread_mutex_lock(&mutex_thread2A);
	pthread_cond_signal(&cond_thread2A);
	pthread_mutex_unlock(&mutex_thread2A);
	pthread_mutex_lock(&mutex_thread3A);
	pthread_cond_signal(&cond_thread3A);
	pthread_mutex_unlock(&mutex_thread3A);
	pthread_mutex_lock(&mutex_thread4A);
	pthread_cond_signal(&cond_thread4A);
	pthread_mutex_unlock(&mutex_thread4A);
	pthread_mutex_lock(&mutex_thread5A);
	pthread_cond_signal(&cond_thread5A);
	pthread_mutex_unlock(&mutex_thread5A);
	pthread_mutex_lock(&mutex_thread6A);
	pthread_cond_signal(&cond_thread6A);
	pthread_mutex_unlock(&mutex_thread6A);
	pthread_mutex_lock(&mutex_thread7A);
	pthread_cond_signal(&cond_thread7A);
	pthread_mutex_unlock(&mutex_thread7A);

#ifdef VSTHRE12
	TLDDataThread8AOUTBool  = 0;
	TLDDataThread9AOUTBool = 0;
	TLDDataThread10AOUTBool = 0;
	TLDDataThread11AOUTBool = 0;
	TLDDataThread8ABool = 1;
	TLDDataThread9ABool = 1;
	TLDDataThread10ABool = 1;
	TLDDataThread11ABool = 1;
	pthread_mutex_lock(&mutex_thread8A);
	pthread_cond_signal(&cond_thread8A);
	pthread_mutex_unlock(&mutex_thread8A);
	pthread_mutex_lock(&mutex_thread9A);
	pthread_cond_signal(&cond_thread9A);
	pthread_mutex_unlock(&mutex_thread9A);
	pthread_mutex_lock(&mutex_thread10A);
	pthread_cond_signal(&cond_thread10A );
	pthread_mutex_unlock(&mutex_thread10A);
	pthread_mutex_lock(&mutex_thread11A);
	pthread_cond_signal(&cond_thread11A);
	pthread_mutex_unlock(&mutex_thread11A);
#endif

	ret = pthread_join(g_hThread1A,NULL);
	if (ret != 0)
	{
		printf("Thread1A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread2A,NULL);
	if (ret != 0)
	{
		printf("Thread2A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread3A,NULL);
	if (ret != 0)
	{
		printf("Thread3A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread4A,NULL);
	if (ret != 0)
	{
		printf("Thread4A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread5A,NULL);
	if (ret != 0)
	{
		printf("Thread5A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread6A,NULL);
	if (ret != 0)
	{
		printf("Thread6A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread7A,NULL);
	if (ret != 0)
	{
		printf("Thread7A pthread_join failed!\n");
		return -1;
	}

	#ifdef VSTHRE12

	ret = pthread_join(g_hThread8A,NULL);
	if (ret != 0)
	{
		printf("Thread8A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread9A,NULL);
	if (ret != 0)
	{
		printf("Thread9A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread10A,NULL);
	if (ret != 0)
	{
		printf("Thread10A pthread_join failed!\n");
		return -1;
	}

	ret = pthread_join(g_hThread11A,NULL);
	if (ret != 0)
	{
		printf("Thread11A pthread_join failed!\n");
		return -1;
	}

	#endif

	free(ptBC_VSObject->pK);
	free(ptBC_VSObject->pWeight1);
	free(ptBC_VSObject->pWeight2);
	free(ptBC_VSObject->pKUV);
	free(ptBC_VSObject->pWeight1UV);
	free(ptBC_VSObject->pWeight2UV);
	free(ptBC_VSObject->pVertIdxShift);
	free(ptBC_VSObject->pCoordY);
	free(ptBC_VSObject->pCoordUV);

	//free(coordVSYA);
	//free(coordVSY2A);
	free(coordVSUVA);
	free(coordVSYYA);
	free(coordVSYY2A);
	free(coordVSUUVA);
	free(VScoordYYFXYCHARA);
	free(VScoordYYFXYCHAR2A);
	free(VScoordWEIGHTA);

	free(pOrigXLowA);
	free(pOrigYLowA);
	free(pWeightXA);
	free(pWeightYA);

	ptBC_VSObject->pK = NULL;
	ptBC_VSObject->pWeight1 = NULL;
	ptBC_VSObject->pWeight2 = NULL;
	ptBC_VSObject->pKUV = NULL;
	ptBC_VSObject->pWeight1UV = NULL;
	ptBC_VSObject->pWeight2UV = NULL;
	ptBC_VSObject->pVertIdxShift = NULL;
	ptBC_VSObject->pCoordY = NULL;
	ptBC_VSObject->pCoordUV = NULL;

	//coordVSYA =NULL;
	//coordVSY2A == NULL;
	coordVSUVA == NULL;
	coordVSYYA == NULL;
	coordVSYY2A == NULL;
	coordVSUUVA == NULL;
	VScoordYYFXYCHARA == NULL;
	VScoordYYFXYCHAR2A == NULL;
	VScoordWEIGHTA == NULL;

	pOrigXLowA == NULL;
	pOrigYLowA == NULL;
	pWeightXA == NULL;
	pWeightYA== NULL;



	return 0;
}
