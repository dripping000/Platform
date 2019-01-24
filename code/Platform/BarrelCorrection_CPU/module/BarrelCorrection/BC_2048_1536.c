#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include "CPUBC.h"
#include <pthread.h>
#include <string.h>
#include <arm_neon.h>
#include "ISP_Alglog.h"
#include <sys/prctl.h>
/////////////////////////////////////////////////////////////////////////////////////////
#define UUVSETBL12	//define to run in 12 threads. if not defined, run in 8 threads
//#define CPUCOMBINE


#define CROP16_9
/////////////////////////////////////////////////////////////////////////////////////////
#define CLIP(x) ((x)<0?0:(x)>255?255:(x))


#define PITCHY 2048
#define PITCHYADD1 2049


unsigned int* BCcoordYYA = NULL;
unsigned int* BCcoordYYFXA = NULL;
unsigned int* BCcoordYYFYA = NULL;
unsigned char* BCcoordYYFXYCHARA = NULL;
unsigned int* BCcoordUUVA = NULL;
unsigned char* BCYDataA = NULL;
unsigned char* BCUVDataA = NULL;
unsigned char* pSrcYBCA = NULL;
unsigned char* pSrcUBCA = NULL;

float* coordY_m1A = NULL ;
float* coordUV_m1A = NULL ;
float* coordY_m2A = NULL ;
float* coordUV_m2A = NULL ;

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

int BCDataThread1ABool = 0;
int BCDataThread2ABool = 0;
int BCDataThread3ABool = 0;
int BCDataThread4ABool = 0;
int BCDataThread5ABool = 0;
int BCDataThread6ABool = 0;
int BCDataThread7ABool = 0;
int BCDataThread8ABool = 0;
int BCDataThread9ABool = 0;
int BCDataThread10ABool = 0;
int BCDataThread11ABool = 0;

int BCDataThread1AOUTBool  = 0;
int BCDataThread2AOUTBool = 0;
int BCDataThread3AOUTBool = 0;
int BCDataThread4AOUTBool = 0;
int BCDataThread5AOUTBool = 0;
int BCDataThread6AOUTBool = 0;
int BCDataThread7AOUTBool = 0;
int BCDataThread8AOUTBool = 0;
int BCDataThread9AOUTBool = 0;
int BCDataThread10AOUTBool = 0;
int BCDataThread11AOUTBool = 0;

pthread_mutex_t mutexBC_thread1A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread1A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread2A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread2A= PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread3A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread3A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread4A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread4A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread5A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread5A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread6A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread6A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread7A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread7A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread8A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread8A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread9A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread9A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread10A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread10A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread11A = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread11A = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_mainA = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_mainA = PTHREAD_COND_INITIALIZER;

long diffA(struct timeval start, struct timeval end)
{
	struct timeval temp;
	if ((end.tv_usec-start.tv_usec)<0)
	{
	 		temp.tv_sec = end.tv_sec-start.tv_sec-1;
	 		temp.tv_usec = 1000000+end.tv_usec-start.tv_usec;
	}
	else
	{
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_usec = end.tv_usec-start.tv_usec;
	}
	return (temp.tv_sec * 1000000 + temp.tv_usec);
}

void bc_convert0A ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*0; j < 2048*1536/12*1; j += 8)

#else

	for (j = 0; j < 2048*1536/8; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 0; j < 2048*1536/48; j++)

#else

	for (j = 0; j < 2048*1536/32; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}

}

void bc_convert1A ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*1; j < 2048*1536/12*2; j += 8)

#else

	for (j = 2048*1536/8*1; j < 2048*1536/8*2; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*1; j < 2048*1536/48*2; j++)

#else

	for (j = 2048*1536/32*1; j < 2048*1536/32*2; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}



void bc_convert2A ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*2; j < 2048*1536/12*3; j += 8)

#else

	for (j = 2048*1536/8*2; j < 2048*1536/8*3; j += 8)

#endif

	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*2; j < 2048*1536/48*3; j++)

#else

	for (j = 2048*1536/32*2; j < 2048*1536/32*3; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert3A ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*3; j < 2048*1536/12*4; j += 8)

#else

	for (j = 2048*1536/8*3; j < 2048*1536/8*4; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*3; j < 2048*1536/48*4; j++)

#else

	for (j = 2048*1536/32*3; j < 2048*1536/32*4; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert4A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*4; j < 2048*1536/12*5; j += 8)

#else

	for (j = 2048*1536/8*4; j < 2048*1536/8*5; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*4; j < 2048*1536/48*5; j++)

#else

	for (j = 2048*1536/32*4; j < 2048*1536/32*5; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert5A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*5; j < 2048*1536/12*6; j += 8)

#else

	for (j = 2048*1536/8*5; j < 2048*1536/8*6; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*5; j < 2048*1536/48*6; j++)

#else

	for (j = 2048*1536/32*5; j < 2048*1536/32*6; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert6A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*6; j < 2048*1536/12*7; j += 8)

#else

	for (j = 2048*1536/8*6; j < 2048*1536/8*7; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*6; j < 2048*1536/48*7; j++)

#else

	for (j = 2048*1536/32*6; j < 2048*1536/32*7; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert7A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*7; j < 2048*1536/12*8; j += 8)

#else

	for (j = 2048*1536/8*7; j < 2048*1536/8*8; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*7; j < 2048*1536/48*8; j++)

#else

	for (j = 2048*1536/32*7; j < 2048*1536/32*8; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert8A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*8; j < 2048*1536/12*9; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*8; j < 2048*1536/48*9; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert9A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*9; j < 2048*1536/12*10; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*9; j < 2048*1536/48*10; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert10A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*10; j < 2048*1536/12*11; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*10; j < 2048*1536/48*11; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert11A()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 2048*1536/12*11; j < 2048*1536/12*12; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARA+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCA[BCcoordYYA[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataA+j, result);
	}

#ifdef UUVSETBL12

	for (j = 2048*1536/48*11; j < 2048*1536/48*12; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCA[BCcoordUUVA[j]];
		(BCUVDataA[j<<1]) = ((*uv1)&0xff);
		BCUVDataA[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}



void ProcessBarrelCThread1A()
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

	while(0 == g1A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread1A enter\n");
		pthread_mutex_lock(&mutexBC_thread1A);
		while(BCDataThread1ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread1A,&mutexBC_thread1A);
		}
		pthread_mutex_unlock(&mutexBC_thread1A);
		BCDataThread1ABool=0;
		ISPprintLog("ProcessBarrelCThread1A start\n");
		bc_convert1A();
		BCDataThread1AOUTBool  = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
	}
}


void ProcessBarrelCThread2A()
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

    while(0 == g2A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread2A enter\n");

		pthread_mutex_lock(&mutexBC_thread2A);
		while(BCDataThread2ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread2A,&mutexBC_thread2A);
		}
		pthread_mutex_unlock(&mutexBC_thread2A);
		BCDataThread2ABool=0;
		ISPprintLog("ProcessBarrelCThread2A start\n");

		bc_convert2A();

		BCDataThread2AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
	}
}



void ProcessBarrelCThread3A()
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

    while(0 == g3A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread3A enter\n");

		pthread_mutex_lock(&mutexBC_thread3A);
		while(BCDataThread3ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread3A,&mutexBC_thread3A);
		}
		pthread_mutex_unlock(&mutexBC_thread3A);
		BCDataThread3ABool=0;
		ISPprintLog("ProcessBarrelCThread3A start\n");

		bc_convert3A();

		BCDataThread3AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
	}
}



void ProcessBarrelCThread4A()
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

    while(0 == g4A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread4A enter\n");

		pthread_mutex_lock(&mutexBC_thread4A);
		while(BCDataThread4ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread4A,&mutexBC_thread4A);
		}
		pthread_mutex_unlock(&mutexBC_thread4A);
		BCDataThread4ABool=0;
		ISPprintLog("ProcessBarrelCThread4A start\n");

		bc_convert4A();

		BCDataThread4AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);

	}
}

void ProcessBarrelCThread5A()
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

    while(0 == g5A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread5A enter\n");

		pthread_mutex_lock(&mutexBC_thread5A);
		while(BCDataThread5ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread5A,&mutexBC_thread5A);
		}
		pthread_mutex_unlock(&mutexBC_thread5A);
		BCDataThread5ABool=0;
		ISPprintLog("ProcessBarrelCThread5A start\n");

		bc_convert5A();

		BCDataThread5AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
		}
}

void ProcessBarrelCThread6A()
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

    while(0 == g6A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread6A enter\n");

		pthread_mutex_lock(&mutexBC_thread6A);
		while(BCDataThread6ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread6A,&mutexBC_thread6A);
		}
		pthread_mutex_unlock(&mutexBC_thread6A);
		BCDataThread6ABool=0;

		ISPprintLog("ProcessBarrelCThread6A start\n");

		bc_convert6A();

		BCDataThread6AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
		}
}


void ProcessBarrelCThread7A()
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

    while(0 == g7A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread7A enter\n");

		pthread_mutex_lock(&mutexBC_thread7A);
		while(BCDataThread7ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread7A,&mutexBC_thread7A);
		}
		pthread_mutex_unlock(&mutexBC_thread7A);
		BCDataThread7ABool=0;

		ISPprintLog("ProcessBarrelCThread7A start\n");
		bc_convert7A();

		BCDataThread7AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
		}
}

void ProcessBarrelCThread8A()
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

    while(0 == g8A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread8A enter\n");

		pthread_mutex_lock(&mutexBC_thread8A);
		while(BCDataThread8ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread8A,&mutexBC_thread8A);
		}
		pthread_mutex_unlock(&mutexBC_thread8A);
		BCDataThread8ABool=0;

		ISPprintLog("ProcessBarrelCThread8A start\n");

		bc_convert8A();

		BCDataThread8AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
		}
}

void ProcessBarrelCThread9A()
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

    while(0 == g9A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread9A enter\n");

		pthread_mutex_lock(&mutexBC_thread9A);
		while(BCDataThread9ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread9A,&mutexBC_thread9A);
		}
		pthread_mutex_unlock(&mutexBC_thread9A);
		BCDataThread9ABool=0;

		ISPprintLog("ProcessBarrelCThread9A start\n");

		bc_convert9A();

		BCDataThread9AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
		}
}

void ProcessBarrelCThread10A()
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

    while(0 == g10A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread10A enter\n");

		pthread_mutex_lock(&mutexBC_thread10A);
		while(BCDataThread10ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread10A,&mutexBC_thread10A);
		}
		pthread_mutex_unlock(&mutexBC_thread10A);
		BCDataThread10ABool=0;

		ISPprintLog("ProcessBarrelCThread10A start\n");

		bc_convert10A();

		BCDataThread10AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
		}
}

void ProcessBarrelCThread11A()
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

    while(0 == g11A_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread11A enter\n");
		pthread_mutex_lock(&mutexBC_thread11A);
		while(BCDataThread11ABool == 0)
		{
			pthread_cond_wait(&BCCond_thread11A,&mutexBC_thread11A);
		}
		pthread_mutex_unlock(&mutexBC_thread11A);
		BCDataThread11ABool=0;
		ISPprintLog("ProcessBarrelCThread11A start\n");
		bc_convert11A();

		BCDataThread11AOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainA);
		pthread_cond_signal(&BCCond_mainA);
		pthread_mutex_unlock(&mutexBC_mainA);
		}
}

int BarrelCorrectInitA(int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight,
					float i_f32Ratio1, float i_f32Ratio2,
				 	float* coordY, float* coordUV,int flagROIMode,float f32x0,float f32y0,float f32w,float f32h)
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


	int x0, y0;
	int ROIWidth, ROIHeight;
	float w;
	float pX,pY;
	float fOrgJ,fOrgI,fDiff;
	int pXLow,pXHigh,pYLow,pYHigh;
	float u,v;
	int cropWidth_half,cropHeight_half;
	int ZoomWidth,ZoomHeight;
	int nI_min,nJ_min;
	int nI_max,nJ_max;
	int array_width,array_height;

	coordY_m1A= (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2);

	coordUV_m1A = (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2/4);

	coordY_m2A= (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2);

	coordUV_m2A = (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2/4);

	if( (coordY_m1A == NULL) ||(coordUV_m1A== NULL)||(coordY_m2A == NULL)||(coordUV_m2A == NULL))
	{
		return ERR_CREATE_HOST_BUFFER;
	}


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


	printf("i_nNewWidth:%d, i_nNewHeight:%d\n",i_nNewWidth, i_nNewHeight);
	printf("i_nWidth:%d, nBHalfW:%d\n",i_nWidth, nBHalfW);
	printf("i_nHeight:%d, nBHalfH:%d\n",i_nHeight, nBHalfH);
	printf("Hratio:%f, Vratio:%f\n",Hratio, Vratio);

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

/*
	flagROIMode = 1;
 	f32w = 0.8;
       f32h = 0.6;
       f32x0 = 0;
	f32y0 = 0;
	*/

	if(((f32x0 + f32w)>1)||((f32y0 + f32h)>1))
	{
		ISPprintLog("roi region set wrong!\n");
		return ERR_ALGORITHM_TYPE;
	}


	ISPprintLog("INPUT PARAM........\n");
	ISPprintLog("w0  is %f\n", f32w);
	ISPprintLog("h0  is %f\n", f32h);
	ISPprintLog("x0  is %f\n", f32x0);
	ISPprintLog("y0  is %f\n", f32y0);

	if (0 == flagROIMode)
	{
		ISPprintLog("choose not to zoom!\n");

	for(nI=0;nI<i_nNewHeight;nI++)
	{
		delty = (nI-nNewHalfH)/scaleDownV;
		vary = delty*delty;

		for (nJ=0;nJ<i_nNewWidth;nJ++)
		{
			deltx = (nJ-nNewHalfW)/scaleDownH;
			vary1 = deltx*deltx+vary;
			varall = (1.0+ vary1*vary1 * i_f32Ratio2 -vary1*i_f32Ratio1);

			//nOrgJ = (deltx*varall+nHalfWidth)*1024;
			//nOrgI = (delty*varall+nHalfHeight)*1024;
			fOrgJ = (deltx*varall+nHalfWidth);
			fOrgI = (delty*varall+nHalfHeight);

			//nOrgJLow = nOrgJ/1024;
			nOrgJLow = fOrgJ;
			nOrgJHigh = nOrgJLow+1;
			//nOrgILow = nOrgI/1024;
			nOrgILow = fOrgI;
			nOrgIHigh = nOrgILow+1;

			if(nOrgJLow>0 && nOrgJHigh < i_nWidth-1 && nOrgILow>0 && nOrgIHigh < i_nHeight-1)
			{
				//nDiff=nOrgJ-(nOrgJLow<<10);
				//coordY[(nI*i_nNewWidth+nJ)*2+0] = ((float)nOrgJLow + (float)nDiff/(float)1024);
				//nDiff=nOrgI-nOrgILow*1024;
				//coordY[(nI*i_nNewWidth+nJ)*2+1] = ((float)nOrgILow + (float)nDiff/(float)1024);

				coordY[(nI*i_nNewWidth+nJ)*2+0] = fOrgJ;
				coordY[(nI*i_nNewWidth+nJ)*2+1] = fOrgI;

				if (nJ % 2 == 0 && nI % 2 == 0)
				{
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+0] = coordY[(nI*i_nNewWidth+nJ)*2+0];
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+1] = coordY[(nI*i_nNewWidth+nJ)*2+1];
				}
			}
		}
	}

	}
	else if (1 == flagROIMode) //1  //zoom1 : need to crop, crop height, then zoom
	{

		ROIWidth = f32w * i_nNewWidth;
		ROIHeight = f32h* i_nNewHeight;
		ZoomWidth = ROIHeight *((float)i_nNewWidth/(float)i_nNewHeight);
		ZoomHeight = ROIWidth* ((float)i_nNewHeight/(float)i_nNewWidth);   // adjust

		if( ROIHeight>=ZoomHeight)
		{
			w = f32w;
			cropHeight_half = (f32h * i_nNewHeight -ZoomHeight)/2;
			cropWidth_half = 0;
			nI_min = cropHeight_half;
			nJ_min= 0;
			nI_max = ZoomHeight+cropHeight_half;
			nJ_max = ROIWidth;
			array_width = ROIWidth;
			array_height = ZoomHeight;

		}
		else
		{
			w = f32h;
			cropWidth_half = (f32w * i_nNewWidth-ZoomWidth)/2;
			cropHeight_half = 0;
			nI_min= 0;
			nJ_min= cropWidth_half;
			nI_max = ROIHeight;
			nJ_max = ZoomWidth+cropWidth_half;
			array_width = ROIWidth - cropWidth_half*2;
			array_height = ROIHeight;

		}
		x0 = f32x0 * i_nNewWidth;
		y0 = f32y0 * i_nNewHeight;

		ISPprintLog("choose to zoom in ROIMode 1!\n");

		ISPprintLog(" ROIWidth:%d,ROIHeight:%d,cropWidth_half:%d,cropHeight_half:%d,W is %f\n",ROIWidth,ROIHeight,cropWidth_half,cropHeight_half,w);
		ISPprintLog(" x0:%d,y0:%d\n",x0,y0);
		ISPprintLog(" nI_min:%d,nJ_min:%d,nI_max:%d,nJ_max:%d\n",nI_min,nJ_min,nI_max,nJ_max);

		for(nI = nI_min; nI < nI_max; nI ++)
	      {
		delty = ( nI-nNewHalfH + y0)/scaleDownV;
		vary = delty*delty;

		for (nJ = nJ_min; nJ < nJ_max; nJ ++)
		{

			deltx = ( nJ -nNewHalfW + x0)/scaleDownH;
			vary1 = deltx*deltx+vary;
			varall = (1.0+ vary1*vary1 * i_f32Ratio2 -vary1*i_f32Ratio1);

			fOrgJ = (deltx*varall+nHalfWidth);
			fOrgI = (delty*varall+nHalfHeight);
			nOrgJLow = fOrgJ;
			nOrgJHigh = nOrgJLow+1;
			nOrgILow = fOrgI;
			nOrgIHigh = nOrgILow+1;

			if(nOrgJLow>=0 && nOrgJHigh <= i_nWidth-1 && nOrgILow>=0 && nOrgIHigh <= i_nHeight-1)
			{
				coordY_m1A[( (nI-nI_min)*array_width+ nJ-nJ_min)*2+0] = fOrgJ;
				coordY_m1A[((nI-nI_min)*array_width+nJ-nJ_min)*2+1] = fOrgI;
			}

		   }
	       }

		for(nI = 0; nI < i_nNewHeight; nI++)
	      {

			for (nJ=0; nJ<  i_nNewWidth; nJ++)
			{

                            pX = w*(float)nJ;
				pY = w*(float)nI;
				pXLow = pX;
				pXHigh = pX + 1;
				pYLow = pY;
				pYHigh = pY + 1;
				u = pX -pXLow;
				v = pY -pYLow;

			if(pXHigh>= array_width)
			{
				pXHigh = array_width - 1;
			}
			if(pYHigh>= array_height)
			{
				pYHigh = array_height - 1;
			}


				coordY[(nI*i_nNewWidth+nJ)*2+0] =  (1-u)*(1-v)*coordY_m1A[(pYLow*array_width+pXLow)*2+0]+(1-u)*v*coordY_m1A[(pYHigh*array_width+pXLow)*2+0]+u*(1-v)*coordY_m1A[(pYLow*array_width+pXHigh)*2+0]+u*v*coordY_m1A[(pYHigh*array_width+pXHigh)*2+0];
				coordY[(nI*i_nNewWidth+nJ)*2+1] =  (1-u)*(1-v)*coordY_m1A[(pYLow*array_width+pXLow)*2+1]+(1-u)*v*coordY_m1A[(pYHigh*array_width+pXLow)*2+1]+u*(1-v)*coordY_m1A[(pYLow*array_width+pXHigh)*2+1]+u*v*coordY_m1A[(pYHigh*array_width+pXHigh)*2+1];

				if (nJ % 2 == 0 && nI % 2 == 0)
				{
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+0] = coordY[(nI*i_nNewWidth+nJ)*2+0];
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+1] = coordY[(nI*i_nNewWidth+nJ)*2+1];
				}


	             }
	     }

	}

	else if (2 == flagROIMode)  //zoom2: just zoom
	{

		x0 = f32x0 * i_nNewWidth;
		y0 = f32y0 * i_nNewHeight;
		ROIWidth = f32w * i_nNewWidth;
		ROIHeight = f32h * i_nNewHeight;

		ISPprintLog("choose to zoom in ROIMode 2 !\n");

		ISPprintLog(" ROIWidth:%d,i_nNewWidth:%d,W is %f\n",ROIWidth,i_nNewWidth,w);

		for( nI = 0; nI < ROIHeight; nI ++)
	      {
		delty = ( nI-nNewHalfH + y0)/scaleDownV;
		vary = delty*delty;

		for (nJ  =0; nJ <  ROIWidth; nJ ++)
		{


			deltx = ( nJ -nNewHalfW + x0)/scaleDownH;
			vary1 = deltx*deltx+vary;
			varall = (1.0+ vary1*vary1 * i_f32Ratio2 -vary1*i_f32Ratio1);

			fOrgJ = (deltx*varall+nHalfWidth);
			fOrgI = (delty*varall+nHalfHeight);
			nOrgJLow = fOrgJ;
			nOrgJHigh = nOrgJLow+1;
			nOrgILow = fOrgI;
			nOrgIHigh = nOrgILow+1;

			if(nOrgJLow>=0 && nOrgJHigh <= i_nWidth-1 && nOrgILow>=0 && nOrgIHigh <= i_nHeight-1)
			{
				coordY_m2A[( nI*ROIWidth+ nJ)*2+0] = fOrgJ;
				coordY_m2A[( nI*ROIWidth+nJ)*2+1] = fOrgI;

			}

		   }
	       }

		for(nI = 0; nI < i_nNewHeight; nI++)
	      {

			for (nJ=0; nJ<  i_nNewWidth; nJ++)
			{

                            pX = f32w*(float)nJ;
				pY = f32h*(float)nI;
				pXLow = pX;
				pXHigh = pX + 1;
				pYLow = pY;
				pYHigh = pY + 1;
				u = pX -(float)pXLow;
				v = pY -(float)pYLow;

			if(pXHigh>= ROIWidth)
			{
				pXHigh = ROIWidth - 1;
			}
			if(pYHigh>= ROIHeight)
			{
				pYHigh = ROIHeight - 1;
			}

				coordY[(nI*i_nNewWidth+nJ)*2+0] =  (1-u)*(1-v)*coordY_m2A[(pYLow*ROIWidth+pXLow)*2+0]+(1-u)*v*coordY_m2A[(pYHigh*ROIWidth+pXLow)*2+0]+u*(1-v)*coordY_m2A[(pYLow*ROIWidth+pXHigh)*2+0]+u*v*coordY_m2A[(pYHigh*ROIWidth+pXHigh)*2+0];
				coordY[(nI*i_nNewWidth+nJ)*2+1] =  (1-u)*(1-v)*coordY_m2A[(pYLow*ROIWidth+pXLow)*2+1]+(1-u)*v*coordY_m2A[(pYHigh*ROIWidth+pXLow)*2+1]+u*(1-v)*coordY_m2A[(pYLow*ROIWidth+pXHigh)*2+1]+u*v*coordY_m2A[(pYHigh*ROIWidth+pXHigh)*2+1];

				if (nJ % 2 == 0 && nI % 2 == 0)
				{
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+0] = coordY[(nI*i_nNewWidth+nJ)*2+0];
					coordUV[((nI/2)*nNewHalfW+(nJ/2))*2+1] = coordY[(nI*i_nNewWidth+nJ)*2+1];
				}

			}

	       }


	}
	else
	{
		ISPprintLog("roi_flag not support!\n");
		return ERR_ALGORITHM_TYPE;
	}


	return 0;
}

int BarrelCorrectOpenA(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject, TBCObject* ptBCObject, TBCOpen* ptBCOpen)
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

	int flagROIMode = ptBCOpen->flagROIMode;
	float f32x0 = ptBCOpen->f32x0;
	float f32y0 = ptBCOpen->f32y0;
	float f32w = ptBCOpen ->f32w;
	float f32h = ptBCOpen ->f32h;

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

	BarrelCorrectInitA(width, height, newWidth, newHeight, f32Ratio1,f32Ratio2,coordY, coordUV,flagROIMode ,f32x0,f32y0,f32w,f32h);
	ISPprintLog("BarrelCorrectInitC succeed!\n");

	BCcoordYYA = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFXA = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFYA = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFXYCHARA = (unsigned char*)malloc(newWidth*newHeight*4*sizeof(unsigned char));
	BCcoordUUVA = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int)/4);

	if ( (NULL == BCcoordYYA ) ||(NULL == BCcoordYYFXA ) ||(NULL == BCcoordYYFYA ) ||(NULL == BCcoordYYFXYCHARA )||(NULL == BCcoordUUVA) )
	{
		return ERR_CREATE_HOST_BUFFER;
	}

	//trans 2dim to 1dim
	int a,i,j,x,y;
	for (j = 0; j < newHeight; j++)
	{
		a = j*newWidth;
		for (i = 0; i < newWidth; i++)
		{
			x = coordY[(a+i)*2];
			y = coordY[(a+i)*2+1];

			BCcoordYYFXA[(a+i)] = (coordY[(a+i)*2] - x)*16;
			BCcoordYYFYA[(a+i)] = (coordY[(a+i)*2+1] - y)*16;
			BCcoordYYA[(a+i)] = y*width + x;

			BCcoordYYFXYCHARA[4*(a+i)] = CLIP((16- BCcoordYYFXA[(a+i)])*(16-BCcoordYYFYA[(a+i)]));
			BCcoordYYFXYCHARA[4*(a+i)+1] = BCcoordYYFXA[(a+i)]*(16-BCcoordYYFYA[(a+i)]);
			BCcoordYYFXYCHARA[4*(a+i)+2] = (16- BCcoordYYFXA[(a+i)])*BCcoordYYFYA[(a+i)];
			BCcoordYYFXYCHARA[4*(a+i)+3] = BCcoordYYFXA[(a+i)]*BCcoordYYFYA[(a+i)];
		}
	}
	for (j = 0; j < newHeight/2; j++)
	{
		a = j*newWidth/2;
		for (i = 0; i < newWidth/2; i++)
		{
			x=coordUV[(a+i)*2];
			y=coordUV[(a+i)*2+1];
			BCcoordUUVA[a + i] = y/2 * width + x/2*2;
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

	g1A_exit_flag = 0;
	g2A_exit_flag = 0;
	g3A_exit_flag = 0;
	g4A_exit_flag = 0;
	g5A_exit_flag = 0;
	g6A_exit_flag = 0;
	g7A_exit_flag = 0;

	int ret = 0;
	ret = pthread_create(&g_hThread1A, NULL, (void *)ProcessBarrelCThread1A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread1A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread2A, NULL, (void *)ProcessBarrelCThread2A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread2A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread3A, NULL, (void *)ProcessBarrelCThread3A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread3A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread4A, NULL, (void *)ProcessBarrelCThread4A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread4A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread5A, NULL, (void *)ProcessBarrelCThread5A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread5A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread6A, NULL, (void *)ProcessBarrelCThread6A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread6A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread7A, NULL, (void *)ProcessBarrelCThread7A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread7A create failed.");
		return -1;
	}

#ifdef UUVSETBL12
	g8A_exit_flag = 0;
	g9A_exit_flag = 0;
	g10A_exit_flag = 0;
	g11A_exit_flag = 0;

	ret = pthread_create(&g_hThread8A, NULL, (void *)ProcessBarrelCThread8A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread8A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread9A, NULL, (void *)ProcessBarrelCThread9A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread9A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread10A, NULL, (void *)ProcessBarrelCThread10A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread10A create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread11A, NULL, (void *)ProcessBarrelCThread11A, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread11A create failed.");
		return -1;
	}
#endif

	return SUCCESS_GPUALG;
}

int BarrelCorrectProcessA(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject,
							TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput)
{
	struct timeval time1, time2;
	struct timezone tzone;
	double total_time = 0;
	double total_time_single = 0;
	double total_time_single_max = 0;
	double total_time_single_min = 10000;

	gettimeofday(&time1, &tzone);

	pSrcYBCA = ptBarrelInput->tImageBuffer[0].pu8ImageDataY;
	pSrcUBCA = ptBarrelInput->tImageBuffer[0].pu8ImageDataU;

	BCYDataA = ptBarrelOutput->tImageBuffer[0].pu8ImageDataY;
	BCUVDataA = ptBarrelOutput->tImageBuffer[0].pu8ImageDataU;


	BCDataThread1AOUTBool  = 0;
	BCDataThread2AOUTBool = 0;
	BCDataThread3AOUTBool = 0;
	BCDataThread4AOUTBool = 0;
	BCDataThread5AOUTBool = 0;
	BCDataThread6AOUTBool = 0;
	BCDataThread7AOUTBool = 0;
	BCDataThread1ABool = 1;
	BCDataThread2ABool = 1;
	BCDataThread3ABool = 1;
	BCDataThread4ABool = 1;
	BCDataThread5ABool = 1;
	BCDataThread6ABool = 1;
	BCDataThread7ABool = 1;
	pthread_mutex_lock(&mutexBC_thread1A);
	pthread_cond_signal(&BCCond_thread1A);
	pthread_mutex_unlock(&mutexBC_thread1A);
	pthread_mutex_lock(&mutexBC_thread2A);
	pthread_cond_signal(&BCCond_thread2A);
	pthread_mutex_unlock(&mutexBC_thread2A);
	pthread_mutex_lock(&mutexBC_thread3A);
	pthread_cond_signal(&BCCond_thread3A);
	pthread_mutex_unlock(&mutexBC_thread3A);
	pthread_mutex_lock(&mutexBC_thread4A);
	pthread_cond_signal(&BCCond_thread4A);
	pthread_mutex_unlock(&mutexBC_thread4A);
	pthread_mutex_lock(&mutexBC_thread5A);
	pthread_cond_signal(&BCCond_thread5A);
	pthread_mutex_unlock(&mutexBC_thread5A);
	pthread_mutex_lock(&mutexBC_thread6A);
	pthread_cond_signal(&BCCond_thread6A);
	pthread_mutex_unlock(&mutexBC_thread6A);
	pthread_mutex_lock(&mutexBC_thread7A);
	pthread_cond_signal(&BCCond_thread7A);
	pthread_mutex_unlock(&mutexBC_thread7A);

#ifdef UUVSETBL12
	BCDataThread8AOUTBool = 0;
	BCDataThread9AOUTBool = 0;
	BCDataThread10AOUTBool = 0;
	BCDataThread11AOUTBool = 0;

	BCDataThread8ABool = 1;
	BCDataThread9ABool = 1;
	BCDataThread10ABool = 1;
	BCDataThread11ABool = 1;

	pthread_mutex_lock(&mutexBC_thread8A);
	pthread_cond_signal(&BCCond_thread8A);
	pthread_mutex_unlock(&mutexBC_thread8A);
	pthread_mutex_lock(&mutexBC_thread9A);
	pthread_cond_signal(&BCCond_thread9A);
	pthread_mutex_unlock(&mutexBC_thread9A);
	pthread_mutex_lock(&mutexBC_thread10A);
	pthread_cond_signal(&BCCond_thread10A);
	pthread_mutex_unlock(&mutexBC_thread10A);
	pthread_mutex_lock(&mutexBC_thread11A);
	pthread_cond_signal(&BCCond_thread11A);
	pthread_mutex_unlock(&mutexBC_thread11A);
#endif


	ISPprintLog("BarrelCorrectProcess\n");

	bc_convert0A();

	ISPprintLog("[[[[[MAIN ALL start]]]]]\n");
	pthread_mutex_lock(&mutexBC_mainA);

#ifdef UUVSETBL12
	while((BCDataThread1AOUTBool  == 0)||(BCDataThread2AOUTBool == 0)||(BCDataThread3AOUTBool == 0)||(BCDataThread4AOUTBool == 0)||(BCDataThread5AOUTBool == 0)||(BCDataThread6AOUTBool == 0)
	||(BCDataThread7AOUTBool == 0)||(BCDataThread8AOUTBool == 0)||(BCDataThread9AOUTBool == 0)||(BCDataThread10AOUTBool == 0)||(BCDataThread11AOUTBool == 0))
#else
	while((BCDataThread1AOUTBool  == 0)||(BCDataThread2AOUTBool == 0)||(BCDataThread3AOUTBool == 0)||(BCDataThread4AOUTBool == 0)||(BCDataThread5AOUTBool == 0)||(BCDataThread6AOUTBool == 0)||(BCDataThread7AOUTBool == 0))
#endif
	{
		pthread_cond_wait(&BCCond_mainA,&mutexBC_mainA);
	}
	pthread_mutex_unlock(&mutexBC_mainA);
	ISPprintLog("[[[[[MAIN ALL END]]]]]\n");

	ptBarrelOutput->tImageBuffer[0].u32Width= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32Height= ptBCObject->dstHeight;
	ptBarrelOutput->tImageBuffer[0].u32PitchY= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32PitchUV= ptBCObject->dstWidth;
	//ptBarrelOutput->tImageBuffer[0].u32PitchY= 1664;
	//ptBarrelOutput->tImageBuffer[0].u32PitchUV= 1664;

	gettimeofday(&time2, &tzone);
	total_time_single = diffA(time1, time2) / 1000.0;
	ISPprintLog("======A=======>single time is %fms\n", total_time_single);

	return SUCCESS_GPUALG;
}

int BarrelCorrectCloseA(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject)
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

	BCDataThread1AOUTBool  = 0;
	BCDataThread2AOUTBool = 0;
	BCDataThread3AOUTBool = 0;
	BCDataThread4AOUTBool = 0;
	BCDataThread5AOUTBool = 0;
	BCDataThread6AOUTBool = 0;
	BCDataThread7AOUTBool = 0;
	BCDataThread1ABool = 1;
	BCDataThread2ABool = 1;
	BCDataThread3ABool = 1;
	BCDataThread4ABool = 1;
	BCDataThread5ABool = 1;
	BCDataThread6ABool = 1;
	BCDataThread7ABool = 1;
	pthread_mutex_lock(&mutexBC_thread1A);
	pthread_cond_signal(&BCCond_thread1A);
	pthread_mutex_unlock(&mutexBC_thread1A);
	pthread_mutex_lock(&mutexBC_thread2A);
	pthread_cond_signal(&BCCond_thread2A);
	pthread_mutex_unlock(&mutexBC_thread2A);
	pthread_mutex_lock(&mutexBC_thread3A);
	pthread_cond_signal(&BCCond_thread3A);
	pthread_mutex_unlock(&mutexBC_thread3A);
	pthread_mutex_lock(&mutexBC_thread4A);
	pthread_cond_signal(&BCCond_thread4A);
	pthread_mutex_unlock(&mutexBC_thread4A);
	pthread_mutex_lock(&mutexBC_thread5A);
	pthread_cond_signal(&BCCond_thread5A);
	pthread_mutex_unlock(&mutexBC_thread5A);
	pthread_mutex_lock(&mutexBC_thread6A);
	pthread_cond_signal(&BCCond_thread6A);
	pthread_mutex_unlock(&mutexBC_thread6A);
	pthread_mutex_lock(&mutexBC_thread7A);
	pthread_cond_signal(&BCCond_thread7A);
	pthread_mutex_unlock(&mutexBC_thread7A);

#ifdef UUVSETBL12
	BCDataThread8AOUTBool = 0;
	BCDataThread9AOUTBool = 0;
	BCDataThread10AOUTBool = 0;
	BCDataThread11AOUTBool = 0;

	BCDataThread8ABool = 1;
	BCDataThread9ABool = 1;
	BCDataThread10ABool = 1;
	BCDataThread11ABool = 1;

	pthread_mutex_lock(&mutexBC_thread8A);
	pthread_cond_signal(&BCCond_thread8A);
	pthread_mutex_unlock(&mutexBC_thread8A);
	pthread_mutex_lock(&mutexBC_thread9A);
	pthread_cond_signal(&BCCond_thread9A);
	pthread_mutex_unlock(&mutexBC_thread9A);
	pthread_mutex_lock(&mutexBC_thread10A);
	pthread_cond_signal(&BCCond_thread10A);
	pthread_mutex_unlock(&mutexBC_thread10A);
	pthread_mutex_lock(&mutexBC_thread11A);
	pthread_cond_signal(&BCCond_thread11A);
	pthread_mutex_unlock(&mutexBC_thread11A);
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

	#ifdef UUVSETBL12

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

	free(BCcoordYYA);
	free(BCcoordYYFXA);
	free(BCcoordYYFYA);
	free(BCcoordYYFXYCHARA);
	free(BCcoordUUVA);
	free(ptBCObject->pCoordY);
	free(ptBCObject->pCoordUV);
	free(coordY_m1A);
	free(coordUV_m1A);
	free(coordY_m2A);
	free(coordUV_m2A);


	BCcoordYYA = NULL;
	BCcoordYYFXA = NULL;
	BCcoordYYFYA = NULL;
	BCcoordYYFXYCHARA = NULL;
	BCcoordUUVA = NULL;
	ptBCObject->pCoordY = NULL;
	ptBCObject->pCoordUV = NULL;
	coordY_m1A = NULL;
	coordUV_m1A = NULL;
	coordY_m2A = NULL;
	coordUV_m2A = NULL;

	return 0;
}
