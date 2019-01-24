//#define ARM
#define PC

#define _GNU_SOURCE

#ifdef ARM
#include <sched.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <arm_neon.h>
#include <sys/prctl.h>
#endif

#include "CPUBC.h"
#include "ISP_Alglog.h"
/////////////////////////////////////////////////////////////////////////////////////////
#define UUVSETBL12	//define to run in 12 threads. if not defined, run in 8 threads
//#define CPUCOMBINE

//#define RESOLUTION_2560_1440	//the resolution set here
//#define RESOLUTION_2688_1512
#define RESOLUTION_1920_1080
//#define RESOLUTION_2048_1536_TO_1920_1080


#define CROP16_9
/////////////////////////////////////////////////////////////////////////////////////////
#define CLIP(x) ((x)<0?0:(x)>255?255:(x))

#ifdef RESOLUTION_2560_1440
	#define PITCHY 2560
	#define PITCHYADD1 2561
#endif	
#ifdef RESOLUTION_2688_1512
	#define PITCHY 2688
	#define PITCHYADD1 2689
#endif	
#ifdef RESOLUTION_1920_1080
	#define PITCHY 1920
	#define PITCHYADD1 1921
#endif	
#ifdef RESOLUTION_2048_1536_TO_1920_1080
	#define PITCHY 2048
	#define PITCHYADD1 2049
#endif

unsigned int* BCcoordYY = NULL;
unsigned int* BCcoordYYFX = NULL;
unsigned int* BCcoordYYFY = NULL;
unsigned char* BCcoordYYFXYCHAR = NULL;
unsigned int* BCcoordUUV = NULL;
unsigned char* BCYData = NULL;
unsigned char* BCUVData = NULL;
unsigned char* pSrcYBC = NULL;
unsigned char* pSrcUBC = NULL;

#ifdef ARM
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
#endif

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

int BCDataThread1Bool = 0;
int BCDataThread2Bool = 0;
int BCDataThread3Bool = 0;
int BCDataThread4Bool = 0;
int BCDataThread5Bool = 0;
int BCDataThread6Bool = 0;
int BCDataThread7Bool = 0;
int BCDataThread8Bool = 0;
int BCDataThread9Bool = 0;
int BCDataThread10Bool = 0;
int BCDataThread11Bool = 0;

int BCDataThread1OUTBool = 0;
int BCDataThread2OUTBool = 0;
int BCDataThread3OUTBool = 0;
int BCDataThread4OUTBool = 0;
int BCDataThread5OUTBool = 0;
int BCDataThread6OUTBool = 0;
int BCDataThread7OUTBool = 0;
int BCDataThread8OUTBool = 0;
int BCDataThread9OUTBool = 0;
int BCDataThread10OUTBool = 0;
int BCDataThread11OUTBool = 0;

#ifdef ARM
pthread_mutex_t mutexBC_thread1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread3 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread4 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread5 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread5 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread6 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread6 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread7 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread7 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread8 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread8 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread9 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread9 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread10 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread10 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread11 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread11 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_main = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_main = PTHREAD_COND_INITIALIZER;
#endif


void bc_convert()
{
    int j;
    unsigned short *uv1;

#ifdef RESOLUTION_2560_1440
    for (j = 1440 * 2560 / 12 * 0; j < 1440 * 2560 / 12 * 1; j += 8)
#endif

#ifdef RESOLUTION_2688_1512
    for (j = 2688 * 1512 / 12 * 0; j < 2688 * 1512 / 12 * 1; j += 8)
#endif

#ifdef RESOLUTION_1920_1080
    for (j = 0; j < 1920 * 1080; j++)
#endif

#ifdef RESOLUTION_2048_1536_TO_1920_1080
    for (j = 1920 * 1080 / 12 * 0; j < 1920 * 1080 / 12 * 1; j += 8)
#endif

    {
        unsigned char fxy[4];
        fxy[0] = *(BCcoordYYFXYCHAR + j * 4);
        fxy[1] = *(BCcoordYYFXYCHAR + j * 4 + 1);
        fxy[2] = *(BCcoordYYFXYCHAR + j * 4 + 2);
        fxy[3] = *(BCcoordYYFXYCHAR + j * 4 + 3);

        unsigned char x0y0;
        unsigned char x1y0;
        unsigned char x0y1;
        unsigned char x1y1;

        x0y0 = (pSrcYBC[BCcoordYY[j]]);
        x1y0 = (pSrcYBC[BCcoordYY[j] + 1]);
        x0y1 = (pSrcYBC[BCcoordYY[j] + PITCHY]);
        x1y1 = (pSrcYBC[BCcoordYY[j] + PITCHYADD1]);

        unsigned int temp;
        temp = fxy[0] * x0y0 + fxy[1] * x1y0 + fxy[2] * x0y1 + fxy[3] * x1y1;
        //unsigned char result = temp / 16;
        unsigned char result = temp >> 8;
        *(BCYData + j) = result;
    }


#ifdef RESOLUTION_2560_1440
    for (j = 0; j < 1440 * 2560 / 48; j++)
#endif

#ifdef RESOLUTION_2688_1512
    for (j = 0; j < 2688 * 1512 / 48; j++)
#endif

#ifdef RESOLUTION_1920_1080
    for (j = 0; j < 1920 * 1080 / 4; j++)
#endif

#ifdef RESOLUTION_2048_1536_TO_1920_1080
    for (j = 0; j < 1920 * 1080 / 48; j++)
#endif

    {
        uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
        (BCUVData[j << 1]) = ((*uv1) & 0xff);
        BCUVData[(j << 1) + 1] = ((*uv1 >> 8) & 0xff);
    }
}


#ifdef ARM
void bc_convert0 ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*0; j < 1440*2560/12*1; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*0; j < 2688*1512/12*1; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*0; j < 1920*1080/12*1; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*0; j < 1920*1080/12*1; j += 8)
	#endif
	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 0; j < 1440*2560/8; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 0; j < 1512*2688/8; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 0; j < 1080*1920/8; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 0; j < 1080*1920/8; j += 8)
	#endif

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 0; j < 1440*2560/48; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 0; j < 2688*1512/48; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 0; j < 1920*1080/48; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 0; j < 1920*1080/48; j++)
	#endif

#else

	#ifdef RESOLUTION_2560_1440
	for (j = 0; j < 1440*2560/32; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 0; j < 1512*2688/32; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 0; j < 1080*1920/32; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 0; j < 1080*1920/32; j++)
	#endif

#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert1 ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*1; j < 1440*2560/12*2; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*1; j < 2688*1512/12*2; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*1; j < 1920*1080/12*2; j += 8)
	#endif
	
	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*1; j < 1920*1080/12*2; j += 8)
	#endif

#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/8*1; j < 1440*2560/8*2; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/8*1; j < 1512*2688/8*2; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/8*1; j < 1080*1920/8*2; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/8*1; j < 1080*1920/8*2; j += 8)
	#endif

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*1; j < 1440*2560/48*2; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*1; j < 2688*1512/48*2; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*1; j < 1920*1080/48*2; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*1; j < 1920*1080/48*2; j++)
	#endif

#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/32*1; j < 1440*2560/32*2; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/32*1; j < 1512*2688/32*2; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/32*1; j < 1080*1920/32*2; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/32*1; j < 1080*1920/32*2; j++)
	#endif

#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}



void bc_convert2 ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*2; j < 1440*2560/12*3; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*2; j < 2688*1512/12*3; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*2; j < 1920*1080/12*3; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*2; j < 1920*1080/12*3; j += 8)
	#endif
	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/8*2; j < 1440*2560/8*3; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/8*2; j < 1512*2688/8*3; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/8*2; j < 1080*1920/8*3; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/8*2; j < 1080*1920/8*3; j += 8)
	#endif

#endif

	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*2; j < 1440*2560/48*3; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*2; j < 2688*1512/48*3; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*2; j < 1920*1080/48*3; j++)
	#endif
	
	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*2; j < 1920*1080/48*3; j++)
	#endif
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/32*2; j < 1440*2560/32*3; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/32*2; j < 1512*2688/32*3; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/32*2; j < 1080*1920/32*3; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/32*2; j < 1080*1920/32*3; j++)
	#endif

#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert3 ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*3; j < 1440*2560/12*4; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*3; j < 2688*1512/12*4; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*3; j < 1920*1080/12*4; j += 8)
	#endif
	
	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*3; j < 1920*1080/12*4; j += 8)
	#endif
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/8*3; j < 1440*2560/8*4; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/8*3; j < 1512*2688/8*4; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/8*3; j < 1080*1920/8*4; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/8*3; j < 1080*1920/8*4; j += 8)
	#endif

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*3; j < 1440*2560/48*4; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*3; j < 2688*1512/48*4; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*3; j < 1920*1080/48*4; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*3; j < 1920*1080/48*4; j++)
	#endif	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/32*3; j < 1440*2560/32*4; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/32*3; j < 1512*2688/32*4; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/32*3; j < 1080*1920/32*4; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/32*3; j < 1080*1920/32*4; j++)
	#endif
#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert4()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*4; j < 1440*2560/12*5; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*4; j < 2688*1512/12*5; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*4; j < 1920*1080/12*5; j += 8)
	#endif
	
	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*4; j < 1920*1080/12*5; j += 8)
	#endif
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/8*4; j < 1440*2560/8*5; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/8*4; j < 1512*2688/8*5; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/8*4; j < 1080*1920/8*5; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/8*4; j < 1080*1920/8*5; j += 8)
	#endif
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*4; j < 1440*2560/48*5; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*4; j < 2688*1512/48*5; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*4; j < 1920*1080/48*5; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*4; j < 1920*1080/48*5; j++)
	#endif	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/32*4; j < 1440*2560/32*5; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/32*4; j < 1512*2688/32*5; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/32*4; j < 1080*1920/32*5; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/32*4; j < 1080*1920/32*5; j++)
	#endif
#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert5()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*5; j < 1440*2560/12*6; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*5; j < 2688*1512/12*6; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*5; j < 1920*1080/12*6; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*5; j < 1920*1080/12*6; j += 8)
	#endif	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/8*5; j < 1440*2560/8*6; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/8*5; j < 1512*2688/8*6; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/8*5; j < 1080*1920/8*6; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/8*5; j < 1080*1920/8*6; j += 8)
	#endif
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*5; j < 1440*2560/48*6; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*5; j < 2688*1512/48*6; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*5; j < 1920*1080/48*6; j++)
	#endif
	
	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*5; j < 1920*1080/48*6; j++)
	#endif
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/32*5; j < 1440*2560/32*6; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/32*5; j < 1512*2688/32*6; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/32*5; j < 1080*1920/32*6; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/32*5; j < 1080*1920/32*6; j++)
	#endif
#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert6()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*6; j < 1440*2560/12*7; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*6; j < 2688*1512/12*7; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*6; j < 1920*1080/12*7; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*6; j < 1920*1080/12*7; j += 8)
	#endif	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/8*6; j < 1440*2560/8*7; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/8*6; j < 1512*2688/8*7; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/8*6; j < 1080*1920/8*7; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/8*6; j < 1080*1920/8*7; j += 8)
	#endif
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*6; j < 1440*2560/48*7; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*6; j < 2688*1512/48*7; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*6; j < 1920*1080/48*7; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*6; j < 1920*1080/48*7; j++)
	#endif	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/32*6; j < 1440*2560/32*7; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/32*6; j < 1512*2688/32*7; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/32*6; j < 1080*1920/32*7; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/32*6; j < 1080*1920/32*7; j++)
	#endif
#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert7()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*7; j < 1440*2560/12*8; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*7; j < 2688*1512/12*8; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*7; j < 1920*1080/12*8; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*7; j < 1920*1080/12*8; j += 8)
	#endif	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/8*7; j < 1440*2560/8*8; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/8*7; j < 1512*2688/8*8; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/8*7; j < 1080*1920/8*8; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/8*7; j < 1080*1920/8*8; j += 8)
	#endif
#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*7; j < 1440*2560/48*8; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*7; j < 2688*1512/48*8; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*7; j < 1920*1080/48*8; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*7; j < 1920*1080/48*8; j++)
	#endif	
#else

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/32*7; j < 1440*2560/32*8; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 1512*2688/32*7; j < 1512*2688/32*8; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1080*1920/32*7; j < 1080*1920/32*8; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1080*1920/32*7; j < 1080*1920/32*8; j++)
	#endif
#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert8()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*8; j < 1440*2560/12*9; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*8; j < 2688*1512/12*9; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*8; j < 1920*1080/12*9; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*8; j < 1920*1080/12*9; j += 8)
	#endif	
#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*8; j < 1440*2560/48*9; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*8; j < 2688*1512/48*9; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*8; j < 1920*1080/48*9; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*8; j < 1920*1080/48*9; j++)
	#endif	
#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert9()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*9; j < 1440*2560/12*10; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*9; j < 2688*1512/12*10; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*9; j < 1920*1080/12*10; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*9; j < 1920*1080/12*10; j += 8)
	#endif	
#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*9; j < 1440*2560/48*10; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*9; j < 2688*1512/48*10; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*9; j < 1920*1080/48*10; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*9; j < 1920*1080/48*10; j++)
	#endif	
#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert10()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*10; j < 1440*2560/12*11; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*10; j < 2688*1512/12*11; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*10; j < 1920*1080/12*11; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*10; j < 1920*1080/12*11; j += 8)
	#endif	
#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);
		
		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*10; j < 1440*2560/48*11; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*10; j < 2688*1512/48*11; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*10; j < 1920*1080/48*11; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*10; j < 1920*1080/48*11; j++)
	#endif	
#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert11()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/12*11; j < 1440*2560/12*12; j += 8)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/12*11; j < 2688*1512/12*12; j += 8)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/12*11; j < 1920*1080/12*12; j += 8)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/12*11; j < 1920*1080/12*12; j += 8)
	#endif	
#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHAR+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBC[BCcoordYY[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);  
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);  
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);  
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);  
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYData+j, result);
	}

#ifdef UUVSETBL12

	#ifdef RESOLUTION_2560_1440
	for (j = 1440*2560/48*11; j < 1440*2560/48*12; j++)
	#endif

	#ifdef RESOLUTION_2688_1512
	for (j = 2688*1512/48*11; j < 2688*1512/48*12; j++)
	#endif

	#ifdef RESOLUTION_1920_1080
	for (j = 1920*1080/48*11; j < 1920*1080/48*12; j++)
	#endif

	#ifdef RESOLUTION_2048_1536_TO_1920_1080
	for (j = 1920*1080/48*11; j < 1920*1080/48*12; j++)
	#endif	
#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBC[BCcoordUUV[j]];
		(BCUVData[j<<1]) = ((*uv1)&0xff);
		BCUVData[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}



void ProcessBarrelCThread1()
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
	sprintf(threName,"BC_THREAD_1");
	prctl(PR_SET_NAME, (unsigned long)threName);

	while(0 == g1_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread1 enter\n");
		pthread_mutex_lock(&mutexBC_thread1);
		while(BCDataThread1Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread1,&mutexBC_thread1);
		}
		pthread_mutex_unlock(&mutexBC_thread1);
		BCDataThread1Bool=0;
		ISPprintLog("ProcessBarrelCThread1 start\n");
		bc_convert1();
		BCDataThread1OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
	}
}


void ProcessBarrelCThread2()
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
	sprintf(threName,"BC_THREAD_2");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g2_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread2 enter\n");

		pthread_mutex_lock(&mutexBC_thread2);
		while(BCDataThread2Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread2,&mutexBC_thread2);
		}
		pthread_mutex_unlock(&mutexBC_thread2);
		BCDataThread2Bool=0;
		ISPprintLog("ProcessBarrelCThread2 start\n");

		bc_convert2();

		BCDataThread2OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
	}
}



void ProcessBarrelCThread3()
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
	sprintf(threName,"BC_THREAD_3");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g3_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread3 enter\n");

		pthread_mutex_lock(&mutexBC_thread3);
		while(BCDataThread3Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread3,&mutexBC_thread3);
		}
		pthread_mutex_unlock(&mutexBC_thread3);
		BCDataThread3Bool=0;
		ISPprintLog("ProcessBarrelCThread3 start\n");

		bc_convert3();

		BCDataThread3OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
	}
}



void ProcessBarrelCThread4()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	char threName[100] = {0};
	sprintf(threName,"BC_THREAD_4");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g4_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread4 enter\n");
		
		pthread_mutex_lock(&mutexBC_thread4);
		while(BCDataThread4Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread4,&mutexBC_thread4);
		}
		pthread_mutex_unlock(&mutexBC_thread4);
		BCDataThread4Bool=0;
		ISPprintLog("ProcessBarrelCThread4 start\n");

		bc_convert4();

		BCDataThread4OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		
	}
}

void ProcessBarrelCThread5()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	char threName[100] = {0};
	sprintf(threName,"BC_THREAD_5");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g5_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread5 enter\n");

		pthread_mutex_lock(&mutexBC_thread5);
		while(BCDataThread5Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread5,&mutexBC_thread5);
		}
		pthread_mutex_unlock(&mutexBC_thread5);
		BCDataThread5Bool=0;
		ISPprintLog("ProcessBarrelCThread5 start\n");

		bc_convert5();

		BCDataThread5OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		}
}

void ProcessBarrelCThread6()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(2, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	char threName[100] = {0};
	sprintf(threName,"BC_THREAD_6");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g6_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread6 enter\n");

		pthread_mutex_lock(&mutexBC_thread6);
		while(BCDataThread6Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread6,&mutexBC_thread6);
		}
		pthread_mutex_unlock(&mutexBC_thread6);
		BCDataThread6Bool=0;

		ISPprintLog("ProcessBarrelCThread6 start\n");

		bc_convert6();

		BCDataThread6OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		}
}


void ProcessBarrelCThread7()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(3, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	char threName[100] = {0};
	sprintf(threName,"BC_THREAD_7");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g7_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread7 enter\n");

		pthread_mutex_lock(&mutexBC_thread7);
		while(BCDataThread7Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread7,&mutexBC_thread7);
		}
		pthread_mutex_unlock(&mutexBC_thread7);
		BCDataThread7Bool=0;

		ISPprintLog("ProcessBarrelCThread7 start\n");
		bc_convert7();

		BCDataThread7OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		}
}

void ProcessBarrelCThread8()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(3, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	char threName[100] = {0};
	sprintf(threName,"BC_THREAD_8");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g8_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread8 enter\n");

		pthread_mutex_lock(&mutexBC_thread8);
		while(BCDataThread8Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread8,&mutexBC_thread8);
		}
		pthread_mutex_unlock(&mutexBC_thread8);
		BCDataThread8Bool=0;

		ISPprintLog("ProcessBarrelCThread8 start\n");

		bc_convert8();

		BCDataThread8OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		}
}

void ProcessBarrelCThread9()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(3, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	char threName[100] = {0};
	sprintf(threName,"BC_THREAD_9");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g9_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread9 enter\n");

		pthread_mutex_lock(&mutexBC_thread9);
		while(BCDataThread9Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread9,&mutexBC_thread9);
		}
		pthread_mutex_unlock(&mutexBC_thread9);
		BCDataThread9Bool=0;

		ISPprintLog("ProcessBarrelCThread9 start\n");

		bc_convert9();

		BCDataThread9OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		}
}

void ProcessBarrelCThread10()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(3, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	char threName[100] = {0};
	sprintf(threName,"BC_THREAD_10");
	prctl(PR_SET_NAME, (unsigned long)threName);

    while(0 == g10_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread10 enter\n");

		pthread_mutex_lock(&mutexBC_thread10);
		while(BCDataThread10Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread10,&mutexBC_thread10);
		}
		pthread_mutex_unlock(&mutexBC_thread10);
		BCDataThread10Bool=0;

		ISPprintLog("ProcessBarrelCThread10 start\n");

		bc_convert10();

		BCDataThread10OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		}
}

void ProcessBarrelCThread11()
{
#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(3, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif				

	prctl(PR_SET_NAME, (unsigned long)("BC_THREAD_11"));

    while(0 == g11_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread11 enter\n");
		pthread_mutex_lock(&mutexBC_thread11);
		while(BCDataThread11Bool == 0)
		{
			pthread_cond_wait(&BCCond_thread11,&mutexBC_thread11);
		}
		pthread_mutex_unlock(&mutexBC_thread11);
		BCDataThread11Bool=0;
		ISPprintLog("ProcessBarrelCThread11 start\n");
		bc_convert11();

		BCDataThread11OUTBool = 1;
		pthread_mutex_lock(&mutexBC_main);
		pthread_cond_signal(&BCCond_main);
		pthread_mutex_unlock(&mutexBC_main);
		}
}
#endif

int BarrelCorrectInit(int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight,
					float i_f32Ratio1, float i_f32Ratio2, 
				 	float* coordY, float* coordUV)
{
	float vary, vary1, varall; 

	int nI, nJ;
	float deltx,delty;
	float scaleDownV = 1.0f;
	float scaleDownH = 1.0f;
	int nNewHalfW, nNewHalfH, nNewWidthStep;
	int nOrgJ, nOrgI, nHalfWidth, nHalfHeight, nWidthStep, nDiff;
	int nWeight1j,nWeight2j,nWeight1i,nWeight2i;
	int nOrgJLow,nOrgJHigh, nOrgILow,nOrgIHigh;
	int nLeftBound, nRightBound, nUpBound, nDownBound;
	float Hratio,Vratio;
	int nBHalfW,nBHalfH;

	nHalfWidth  = i_nWidth/2;
	nHalfHeight = i_nHeight/2;
	nNewHalfW = i_nNewWidth/2;
	nNewHalfH = i_nNewHeight/2;
	nWidthStep = i_nWidth;
	nNewWidthStep = i_nNewWidth;

	vary = nHalfWidth*nHalfWidth;
	nBHalfW =(nHalfWidth/(1.0+i_f32Ratio2*(vary*vary)-vary*i_f32Ratio1));
	vary = nHalfHeight*nHalfHeight;
	nBHalfH =(nHalfHeight/(1.0+i_f32Ratio2*(vary*vary)-vary*i_f32Ratio1));

	// Hratio = (float)(i_nWidth)/((float)((nBHalfW/4)*8));
	// Vratio = (float)(i_nHeight)/((float)(nBHalfH*2));
	Hratio = ((float)(i_nNewWidth))/((float)((nBHalfW/4)*8));
	Vratio = ((float)(i_nNewHeight))/((float)(nBHalfH*2));
	ISPprintLog("i_nNewWidth:%d, i_nNewHeight:%d\n",i_nNewWidth, i_nNewHeight);
	ISPprintLog("i_nWidth:%d, nBHalfW:%d\n",i_nWidth, nBHalfW);
	ISPprintLog("i_nHeight:%d, nBHalfH:%d\n",i_nHeight, nBHalfH);
	ISPprintLog("Hratio:%f, Vratio:%f\n",Hratio, Vratio);

#ifndef CROP16_9
	if(Hratio < Vratio)
	{
		scaleDownV = Vratio;
		scaleDownH = Vratio;
	}
	else
	{
		scaleDownV = Hratio;
		scaleDownH = Hratio;
	}
	ISPprintLog(" NOT DEFINE CROP16_9 scaleDownV:%f, scaleDownH:%f\n",scaleDownV, scaleDownH);
	
#else
	scaleDownV = Vratio;
	scaleDownH = Hratio;
	ISPprintLog("DEFINE CROP16_9 scaleDownV:%f, scaleDownH:%f\n",scaleDownV, scaleDownH);

#endif


	for(nI=0;nI<i_nNewHeight;nI++)
	{
		delty = (nI-nNewHalfH)/scaleDownV;
		vary = delty*delty;

		for (nJ=0;nJ<i_nNewWidth;nJ++)
		{
			deltx = (nJ-nNewHalfW)/scaleDownH;
			vary1 = deltx*deltx+vary;
			varall = (1.0+ vary1*vary1 * i_f32Ratio2 -vary1*i_f32Ratio1);

			nOrgJ = (deltx*varall+nHalfWidth)*1024;
			nOrgI = (delty*varall+nHalfHeight)*1024;
			nOrgJLow = nOrgJ/1024;
			nOrgJHigh = nOrgJLow+1;
			nOrgILow = nOrgI/1024;
			nOrgIHigh = nOrgILow+1;

			if(nOrgJLow>0 && nOrgJHigh < i_nWidth-1 && nOrgILow>0 && nOrgIHigh < i_nHeight-1)
			{
				nDiff=nOrgJ-(nOrgJLow<<10);
				coordY[(nI*i_nNewWidth+nJ)*2+0] = ((float)nOrgJLow + (float)nDiff/(float)1024);
				nDiff=nOrgI-nOrgILow*1024;
				coordY[(nI*i_nNewWidth+nJ)*2+1] = ((float)nOrgILow + (float)nDiff/(float)1024);

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

int BarrelCorrectOpen(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject, TBCObject* ptBCObject, TBCOpen* ptBCOpen)
{
	int width = ptBCOpen->u32InputWidth;
	int height = ptBCOpen->u32InputHeight;
	int newWidth = ptBCOpen->u32OutputWidth;
	int newHeight =  ptBCOpen->u32OutputHeight;
	int ratio1;
	int ratio2;
	float f32Ratio1;
	float f32Ratio2;
	float* coordY = NULL;
	float* coordUV = NULL;

	coordY = (float*)malloc(newWidth*newHeight*sizeof(float)*2);
	if (coordY == NULL)
	{
		return ERR_CREATE_HOST_BUFFER;
	}
	coordUV = (float*)malloc(newWidth*newHeight*sizeof(float)*2/4);
	if (coordUV == NULL)
	{
		return ERR_CREATE_HOST_BUFFER;
	}
	
	ptBCObject->srcWidth = width;
	ptBCObject->srcHeight = height;
	ptBCObject->srcPitchY = ptBCOpen->u32InputPitchY;
	ptBCObject->srcPitchUV = ptBCOpen->u32InputPitchUV;
	ptBCObject->dstWidth = newWidth;
	ptBCObject->dstHeight = newHeight;
	ptBCObject->pCoordY = coordY;
	ptBCObject->pCoordUV = coordUV;

	ratio1 = ptBCOpen->u32Ratio1;
	ratio2 = ptBCOpen->u32Ratio2;

	ISPprintLog("ratio1 = %d\n", ratio1);
	ISPprintLog("ratio2 = %d\n", ratio2);

	f32Ratio1 = (float)(ratio1)/10000000000.0;
	f32Ratio2= (float)(ratio2)/10000000000000000.0;

	BarrelCorrectInit(width, height, newWidth, newHeight, f32Ratio1,f32Ratio2,coordY, coordUV);
	ISPprintLog("BarrelCorrectInit succeed!\n");

	BCcoordYY = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFX = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFY = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFXYCHAR = (unsigned char*)malloc(newWidth*newHeight*4*sizeof(unsigned char));
	BCcoordUUV = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int)/4);

	//trans 2dim to 1dim
	int a,i,j,x,y;
	for (j = 0; j < newHeight; j++)
	{
		a = j*newWidth;
		for (i = 0; i < newWidth; i++)
		{
			x = coordY[(a+i)*2];
			y = coordY[(a+i)*2+1];

			BCcoordYYFX[(a+i)] = (coordY[(a+i)*2] - x)*16;
			BCcoordYYFY[(a+i)] = (coordY[(a+i)*2+1] - y)*16;
			BCcoordYY[(a+i)] = y*width + x;

			BCcoordYYFXYCHAR[4*(a+i)] = CLIP((16- BCcoordYYFX[(a+i)])*(16-BCcoordYYFY[(a+i)]));
			BCcoordYYFXYCHAR[4*(a+i)+1] = BCcoordYYFX[(a+i)]*(16-BCcoordYYFY[(a+i)]);
			BCcoordYYFXYCHAR[4*(a+i)+2] = (16- BCcoordYYFX[(a+i)])*BCcoordYYFY[(a+i)];
			BCcoordYYFXYCHAR[4*(a+i)+3] = BCcoordYYFX[(a+i)]*BCcoordYYFY[(a+i)];
		} 
	}
	for (j = 0; j < newHeight/2; j++)
	{
		a = j*newWidth/2;
		for (i = 0; i < newWidth/2; i++)
		{
			x=coordUV[(a+i)*2];
			y=coordUV[(a+i)*2+1];
			BCcoordUUV[a + i] = y/2 * width + x/2*2;
		} 
	}

#ifdef CPUCOMBINE
	cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(5, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1) 
    {
		ISPprintLog("sched_setaffinity\n");
    }
#endif	

#ifdef ARM
	g1_exit_flag = 0;
	g2_exit_flag = 0;
	g3_exit_flag = 0;
	g4_exit_flag = 0;
	g5_exit_flag = 0;
	g6_exit_flag = 0;
	g7_exit_flag = 0;

	int ret = 0;
	ret = pthread_create(&g_hThread1, NULL, (void *)ProcessBarrelCThread1, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread1 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread2, NULL, (void *)ProcessBarrelCThread2, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread2 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread3, NULL, (void *)ProcessBarrelCThread3, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread3 create failed.");
		return -1;
	}	
	ret = pthread_create(&g_hThread4, NULL, (void *)ProcessBarrelCThread4, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread4 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread5, NULL, (void *)ProcessBarrelCThread5, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread5 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread6, NULL, (void *)ProcessBarrelCThread6, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread6 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread7, NULL, (void *)ProcessBarrelCThread7, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread7 create failed.");
		return -1;
	}

#ifdef UUVSETBL12
	g8_exit_flag = 0;
	g9_exit_flag = 0;
	g10_exit_flag = 0;
	g11_exit_flag = 0;
	
	ret = pthread_create(&g_hThread8, NULL, (void *)ProcessBarrelCThread8, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread8 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread9, NULL, (void *)ProcessBarrelCThread9, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread9 create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread10, NULL, (void *)ProcessBarrelCThread10, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread10 create failed.");
		return -1;
	}	
	ret = pthread_create(&g_hThread11, NULL, (void *)ProcessBarrelCThread11, NULL);
	if (ret != 0) 
	{
		printf("ProcessBarrelCThread11 create failed.");
		return -1;
	}
#endif

#endif

	return SUCCESS_GPUALG;
}

int BarrelCorrectProcess(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject, 
							TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput)
{
	pSrcYBC = ptBarrelInput->tImageBuffer[0].pu8ImageDataY;
	pSrcUBC = ptBarrelInput->tImageBuffer[0].pu8ImageDataU;

	BCYData = ptBarrelOutput->tImageBuffer[0].pu8ImageDataY;
	BCUVData = ptBarrelOutput->tImageBuffer[0].pu8ImageDataU;

#ifdef ARM
	BCDataThread1OUTBool = 0;
	BCDataThread2OUTBool = 0;
	BCDataThread3OUTBool = 0;
	BCDataThread4OUTBool = 0;
	BCDataThread5OUTBool = 0;
	BCDataThread6OUTBool = 0;
	BCDataThread7OUTBool = 0;
	BCDataThread1Bool = 1;
	BCDataThread2Bool = 1;
	BCDataThread3Bool = 1;
	BCDataThread4Bool = 1;
	BCDataThread5Bool = 1;
	BCDataThread6Bool = 1;
	BCDataThread7Bool = 1;
	pthread_mutex_lock(&mutexBC_thread1);
	pthread_cond_signal(&BCCond_thread1);
	pthread_mutex_unlock(&mutexBC_thread1);
	pthread_mutex_lock(&mutexBC_thread2);
	pthread_cond_signal(&BCCond_thread2);
	pthread_mutex_unlock(&mutexBC_thread2);
	pthread_mutex_lock(&mutexBC_thread3);
	pthread_cond_signal(&BCCond_thread3);
	pthread_mutex_unlock(&mutexBC_thread3);
	pthread_mutex_lock(&mutexBC_thread4);
	pthread_cond_signal(&BCCond_thread4);
	pthread_mutex_unlock(&mutexBC_thread4);
	pthread_mutex_lock(&mutexBC_thread5);
	pthread_cond_signal(&BCCond_thread5);
	pthread_mutex_unlock(&mutexBC_thread5);
	pthread_mutex_lock(&mutexBC_thread6);
	pthread_cond_signal(&BCCond_thread6);
	pthread_mutex_unlock(&mutexBC_thread6);
	pthread_mutex_lock(&mutexBC_thread7);
	pthread_cond_signal(&BCCond_thread7);
	pthread_mutex_unlock(&mutexBC_thread7);

#ifdef UUVSETBL12
	BCDataThread8OUTBool = 0;
	BCDataThread9OUTBool = 0;
	BCDataThread10OUTBool = 0;
	BCDataThread11OUTBool = 0;

	BCDataThread8Bool = 1;
	BCDataThread9Bool = 1;
	BCDataThread10Bool = 1;
	BCDataThread11Bool = 1;

	pthread_mutex_lock(&mutexBC_thread8);
	pthread_cond_signal(&BCCond_thread8);
	pthread_mutex_unlock(&mutexBC_thread8);
	pthread_mutex_lock(&mutexBC_thread9);
	pthread_cond_signal(&BCCond_thread9);
	pthread_mutex_unlock(&mutexBC_thread9);
	pthread_mutex_lock(&mutexBC_thread10);
	pthread_cond_signal(&BCCond_thread10);
	pthread_mutex_unlock(&mutexBC_thread10);
	pthread_mutex_lock(&mutexBC_thread11);
	pthread_cond_signal(&BCCond_thread11);
	pthread_mutex_unlock(&mutexBC_thread11);
#endif

	ISPprintLog("BarrelCorrectProcess\n");

	bc_convert0();
	
	ISPprintLog("[[[[[MAIN ALL start]]]]]\n");
	pthread_mutex_lock(&mutexBC_main);

#ifdef UUVSETBL12
	while((BCDataThread1OUTBool == 0)||(BCDataThread2OUTBool == 0)||(BCDataThread3OUTBool == 0)||(BCDataThread4OUTBool == 0)||(BCDataThread5OUTBool == 0)||(BCDataThread6OUTBool == 0)
	||(BCDataThread7OUTBool == 0)||(BCDataThread8OUTBool == 0)||(BCDataThread9OUTBool == 0)||(BCDataThread10OUTBool == 0)||(BCDataThread11OUTBool == 0))
#else
	while((BCDataThread1OUTBool == 0)||(BCDataThread2OUTBool == 0)||(BCDataThread3OUTBool == 0)||(BCDataThread4OUTBool == 0)||(BCDataThread5OUTBool == 0)||(BCDataThread6OUTBool == 0)||(BCDataThread7OUTBool == 0))
#endif
	{
		pthread_cond_wait(&BCCond_main,&mutexBC_main);
	}
	pthread_mutex_unlock(&mutexBC_main);
	ISPprintLog("[[[[[MAIN ALL END]]]]]\n");
#endif

    bc_convert();

	ptBarrelOutput->tImageBuffer[0].u32Width= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32Height= ptBCObject->dstHeight;
	ptBarrelOutput->tImageBuffer[0].u32PitchY= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32PitchUV= ptBCObject->dstWidth;

	return SUCCESS_GPUALG;
}

int BarrelCorrectClose(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject)
{
	free(BCcoordYY);
	free(BCcoordYYFX);
	free(BCcoordYYFY);
	free(BCcoordYYFXYCHAR);
	free(BCcoordUUV);
	free(ptBCObject->pCoordY);
	free(ptBCObject->pCoordUV);
	ptBCObject->pCoordY = NULL;
	ptBCObject->pCoordUV = NULL;

	return 0;
}
