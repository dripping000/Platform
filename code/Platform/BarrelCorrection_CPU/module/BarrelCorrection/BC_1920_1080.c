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


unsigned int* BCcoordYYB = NULL;
unsigned int* BCcoordYYFXB = NULL;
unsigned int* BCcoordYYFYB = NULL;
unsigned char* BCcoordYYFXYCHARB = NULL;
unsigned int* BCcoordUUVB = NULL;
unsigned char* BCYDataB = NULL;
unsigned char* BCUVDataB = NULL;
unsigned char* pSrcYBCB = NULL;
unsigned char* pSrcUBCB = NULL;

float* coordY_m1B = NULL ;
float* coordUV_m1B = NULL ;
float* coordY_m2B = NULL ;
float* coordUV_m2B = NULL ;


pthread_t g_hThread1B;
pthread_t g_hThread2B;
pthread_t g_hThread3B;
pthread_t g_hThread4B;
pthread_t g_hThread5B;
pthread_t g_hThread6B;
pthread_t g_hThread7B;
pthread_t g_hThread8B;
pthread_t g_hThread9B;
pthread_t g_hThread10B;
pthread_t g_hThread11B;

/*
pthread_attr_t g_hThread1B_attr;
pthread_attr_t g_hThread2B_attr;
pthread_attr_t g_hThread3B_attr;
pthread_attr_t g_hThread4B_attr;
pthread_attr_t g_hThread5B_attr;
pthread_attr_t g_hThread6B_attr;
pthread_attr_t g_hThread7B_attr;
pthread_attr_t g_hThread8B_attr;
pthread_attr_t g_hThread9B_attr;
pthread_attr_t g_hThread10B_attr;
pthread_attr_t g_hThread11B_attr;
*/


static int g1B_exit_flag = 1;
static int g2B_exit_flag = 1;
static int g3B_exit_flag = 1;
static int g4B_exit_flag = 1;
static int g5B_exit_flag = 1;
static int g6B_exit_flag = 1;
static int g7B_exit_flag = 1;
static int g8B_exit_flag = 1;
static int g9B_exit_flag = 1;
static int g10B_exit_flag = 1;
static int g11B_exit_flag = 1;

/*
int g1B_exit_flag ;
int g2B_exit_flag ;
int g3B_exit_flag ;
int g4B_exit_flag ;
int g5B_exit_flag ;
int g6B_exit_flag ;
int g7B_exit_flag ;
int g8B_exit_flag ;
int g9B_exit_flag ;
int g10B_exit_flag ;
int g11B_exit_flag ;
*/

int BCDataThread1BBool = 0;
int BCDataThread2BBool = 0;
int BCDataThread3BBool = 0;
int BCDataThread4BBool = 0;
int BCDataThread5BBool = 0;
int BCDataThread6BBool = 0;
int BCDataThread7BBool = 0;
int BCDataThread8BBool = 0;
int BCDataThread9BBool = 0;
int BCDataThread10BBool = 0;
int BCDataThread11BBool = 0;

int BCDataThread1BOUTBool = 0;
int BCDataThread2BOUTBool = 0;
int BCDataThread3BOUTBool = 0;
int BCDataThread4BOUTBool = 0;
int BCDataThread5BOUTBool = 0;
int BCDataThread6BOUTBool = 0;
int BCDataThread7BOUTBool = 0;
int BCDataThread8BOUTBool = 0;
int BCDataThread9BOUTBool = 0;
int BCDataThread10BOUTBool = 0;
int BCDataThread11BOUTBool = 0;

pthread_mutex_t mutexBC_thread1B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread1B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread2B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread2B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread3B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread3B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread4B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread4B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread5B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread5B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread6B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread6B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread7B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread7B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread8B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread8B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread9B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread9B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread10B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread10B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread11B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread11B = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_mainB = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_mainB = PTHREAD_COND_INITIALIZER;

long diffB(struct timeval start, struct timeval end)
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

void bc_convert0B ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*0; j < 1920*1080/12*1; j += 8)

#else

	for (j = 0; j < 1080*1920/8; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 0; j < 1920*1080/48*1; j++)

#else

	for (j = 0; j < 1080*1920/32; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert1B ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*1; j < 1920*1080/12*2; j += 8)

#else

	for (j = 1080*1920/8*1; j < 1080*1920/8*2; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*1; j < 1920*1080/48*2; j++)

#else

	for (j = 1080*1920/32*1; j < 1080*1920/32*2; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}



void bc_convert2B ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*2; j < 1920*1080/12*3; j += 8)

#else

	for (j = 1080*1920/8*2; j < 1080*1920/8*3; j += 8)

#endif

	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*2; j < 1920*1080/48*3; j++)

#else

	for (j = 1080*1920/32*2; j < 1080*1920/32*3; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert3B ()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*3; j < 1920*1080/12*4; j += 8)

#else

	for (j = 1080*1920/8*3; j < 1080*1920/8*4; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*3; j < 1920*1080/48*4; j++)

#else

	for (j = 1080*1920/32*3; j < 1080*1920/32*4; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert4B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*4; j < 1920*1080/12*5; j += 8)

#else

	for (j = 1080*1920/8*4; j < 1080*1920/8*5; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*4; j < 1920*1080/48*5; j++)

#else

	for (j = 1080*1920/32*4; j < 1080*1920/32*5; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert5B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*5; j < 1920*1080/12*6; j += 8)

#else

	for (j = 1080*1920/8*5; j < 1080*1920/8*6; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*5; j < 1920*1080/48*6; j++)

#else

	for (j = 1080*1920/32*5; j < 1080*1920/32*6; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert6B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*6; j < 1920*1080/12*7; j += 8)

#else

	for (j = 1080*1920/8*6; j < 1080*1920/8*7; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*6; j < 1920*1080/48*7; j++)

#else

	for (j = 1080*1920/32*6; j < 1080*1920/32*7; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert7B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*7; j < 1920*1080/12*8; j += 8)

#else

	for (j = 1080*1920/8*7; j < 1080*1920/8*8; j += 8)

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*7; j < 1920*1080/48*8; j++)

#else

	for (j = 1080*1920/32*7; j < 1080*1920/32*8; j++)

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

void bc_convert8B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*8; j < 1920*1080/12*9; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*8; j < 1920*1080/48*9; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert9B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*9; j < 1920*1080/12*10; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*9; j < 1920*1080/48*10; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert10B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*10; j < 1920*1080/12*11; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*10; j < 1920*1080/48*11; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}


void bc_convert11B()
{
	int j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (j = 1920*1080/12*11; j < 1920*1080/12*12; j += 8)

#else

	return;

#endif
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    uint8x8_t result = vshrn_n_u16 (temp, 8);
    	vst1_u8 (BCYDataB+j, result);
	}

#ifdef UUVSETBL12

	for (j = 1920*1080/48*11; j < 1920*1080/48*12; j++)

#else

	return;

#endif
	{
		uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
		(BCUVDataB[j<<1]) = ((*uv1)&0xff);
		BCUVDataB[(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}



void ProcessBarrelCThread1B()
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

	while(0 == g1B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread1B enter\n");
		pthread_mutex_lock(&mutexBC_thread1B);
		while(BCDataThread1BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread1B,&mutexBC_thread1B);
		}
		pthread_mutex_unlock(&mutexBC_thread1B);
		BCDataThread1BBool=0;
		ISPprintLog("ProcessBarrelCThread1B start\n");
		bc_convert1B();
		BCDataThread1BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
	}
}


void ProcessBarrelCThread2B()
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

    while(0 == g2B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread2B enter\n");

		pthread_mutex_lock(&mutexBC_thread2B);
		while(BCDataThread2BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread2B,&mutexBC_thread2B);
		}
		pthread_mutex_unlock(&mutexBC_thread2B);
		BCDataThread2BBool=0;
		ISPprintLog("ProcessBarrelCThread2B start\n");

		bc_convert2B();

		BCDataThread2BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
	}
}



void ProcessBarrelCThread3B()
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

    while(0 == g3B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread3B enter\n");

		pthread_mutex_lock(&mutexBC_thread3B);
		while(BCDataThread3BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread3B,&mutexBC_thread3B);
		}
		pthread_mutex_unlock(&mutexBC_thread3B);
		BCDataThread3BBool=0;
		ISPprintLog("ProcessBarrelCThread3B start\n");

		bc_convert3B();

		BCDataThread3BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
	}
}



void ProcessBarrelCThread4B()
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

    while(0 == g4B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread4B enter\n");

		pthread_mutex_lock(&mutexBC_thread4B);
		while(BCDataThread4BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread4B,&mutexBC_thread4B);
		}
		pthread_mutex_unlock(&mutexBC_thread4B);
		BCDataThread4BBool=0;
		ISPprintLog("ProcessBarrelCThread4B start\n");

		bc_convert4B();

		BCDataThread4BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);

	}
}

void ProcessBarrelCThread5B()
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

    while(0 == g5B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread5B enter\n");

		pthread_mutex_lock(&mutexBC_thread5B);
		while(BCDataThread5BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread5B,&mutexBC_thread5B);
		}
		pthread_mutex_unlock(&mutexBC_thread5B);
		BCDataThread5BBool=0;
		ISPprintLog("ProcessBarrelCThread5B start\n");

		bc_convert5B();

		BCDataThread5BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
		}
}

void ProcessBarrelCThread6B()
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

    while(0 == g6B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread6B enter\n");

		pthread_mutex_lock(&mutexBC_thread6B);
		while(BCDataThread6BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread6B,&mutexBC_thread6B);
		}
		pthread_mutex_unlock(&mutexBC_thread6B);
		BCDataThread6BBool=0;

		ISPprintLog("ProcessBarrelCThread6B start\n");

		bc_convert6B();

		BCDataThread6BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
		}
}


void ProcessBarrelCThread7B()
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

    while(0 == g7B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread7B enter\n");

		pthread_mutex_lock(&mutexBC_thread7B);
		while(BCDataThread7BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread7B,&mutexBC_thread7B);
		}
		pthread_mutex_unlock(&mutexBC_thread7B);
		BCDataThread7BBool=0;

		ISPprintLog("ProcessBarrelCThread7B start\n");
		bc_convert7B();

		BCDataThread7BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
		}
}

void ProcessBarrelCThread8B()
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

    while(0 == g8B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread8B enter\n");

		pthread_mutex_lock(&mutexBC_thread8B);
		while(BCDataThread8BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread8B,&mutexBC_thread8B);
		}
		pthread_mutex_unlock(&mutexBC_thread8B);
		BCDataThread8BBool=0;

		ISPprintLog("ProcessBarrelCThread8B start\n");

		bc_convert8B();

		BCDataThread8BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
		}
}

void ProcessBarrelCThread9B()
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

    while(0 == g9B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread9B enter\n");

		pthread_mutex_lock(&mutexBC_thread9B);
		while(BCDataThread9BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread9B,&mutexBC_thread9B);
		}
		pthread_mutex_unlock(&mutexBC_thread9B);
		BCDataThread9BBool=0;

		ISPprintLog("ProcessBarrelCThread9B start\n");

		bc_convert9B();

		BCDataThread9BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
		}
}

void ProcessBarrelCThread10B()
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

    while(0 == g10B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread10B enter\n");

		pthread_mutex_lock(&mutexBC_thread10B);
		while(BCDataThread10BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread10B,&mutexBC_thread10B);
		}
		pthread_mutex_unlock(&mutexBC_thread10B);
		BCDataThread10BBool=0;

		ISPprintLog("ProcessBarrelCThread10B start\n");

		bc_convert10B();

		BCDataThread10BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
		}
}

void ProcessBarrelCThread11B()
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

    while(0 == g11B_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread11B enter\n");
		pthread_mutex_lock(&mutexBC_thread11B);
		while(BCDataThread11BBool == 0)
		{
			pthread_cond_wait(&BCCond_thread11B,&mutexBC_thread11B);
		}
		pthread_mutex_unlock(&mutexBC_thread11B);
		BCDataThread11BBool=0;
		ISPprintLog("ProcessBarrelCThread11B start\n");
		bc_convert11B();

		BCDataThread11BOUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainB);
		pthread_cond_signal(&BCCond_mainB);
		pthread_mutex_unlock(&mutexBC_mainB);
		}
}

int BarrelCorrectInitB(int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight,
					float i_f32Ratio1, float i_f32Ratio2,
				 	float* coordY, float* coordUV,int flagROIMode,float f32x0,float f32y0,float f32w,float f32h)
{
	float vary, vary1, varall;

	int nI, nJ;
	float deltx,delty;
	float scaleDownV = 1.0f;
	float scaleDownH = 1.0f;
	int nNewHalfW, nNewHalfH, nNewWidthStep;
	int nOrgJ, nOrgI, nHalfWidth, nHalfHeight, nWidthStep, ndiffB;
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

	coordY_m1B= (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2);

	coordUV_m1B = (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2/4);

	coordY_m2B= (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2);

	coordUV_m2B = (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2/4);

	if( (coordY_m1B == NULL) ||(coordUV_m1B== NULL)||(coordY_m2B == NULL)||(coordUV_m2B == NULL))
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

			nOrgJLow = fOrgJ;
			nOrgILow = fOrgI;

			//nOrgJLow = nOrgJ/1024;
			nOrgJHigh = nOrgJLow+1;
			//nOrgILow = nOrgI/1024;
			nOrgIHigh = nOrgILow+1;

			if(nOrgJLow>0 && nOrgJHigh < i_nWidth-1 && nOrgILow>0 && nOrgIHigh < i_nHeight-1)
			{
				//ndiffB=nOrgJ-(nOrgJLow<<10);
				//coordY[(nI*i_nNewWidth+nJ)*2+0] = ((float)nOrgJLow + (float)ndiffB/(float)1024);
				//ndiffB=nOrgI-nOrgILow*1024;
				//coordY[(nI*i_nNewWidth+nJ)*2+1] = ((float)nOrgILow + (float)ndiffB/(float)1024);

				coordY[(nI*i_nNewWidth+nJ)*2+0] = fOrgJ;
				coordY[(nI*i_nNewWidth+nJ)*2+1] = fOrgI;
/*
				if((960 == nI)&&(640 == nJ))
				{
					printf("==============>coordY[(nI*i_nNewWidth+nJ)*2+0]:%f\n",fOrgJ);
					printf("==============>coordY[(nI*i_nNewWidth+nJ)*2+1]:%f\n",fOrgI);

				}
*/

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
				coordY_m1B[( (nI-nI_min)*array_width+ nJ-nJ_min)*2+0] = fOrgJ;
				coordY_m1B[((nI-nI_min)*array_width+nJ-nJ_min)*2+1] = fOrgI;
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


				coordY[(nI*i_nNewWidth+nJ)*2+0] =  (1-u)*(1-v)*coordY_m1B[(pYLow*array_width+pXLow)*2+0]+(1-u)*v*coordY_m1B[(pYHigh*array_width+pXLow)*2+0]+u*(1-v)*coordY_m1B[(pYLow*array_width+pXHigh)*2+0]+u*v*coordY_m1B[(pYHigh*array_width+pXHigh)*2+0];
				coordY[(nI*i_nNewWidth+nJ)*2+1] =  (1-u)*(1-v)*coordY_m1B[(pYLow*array_width+pXLow)*2+1]+(1-u)*v*coordY_m1B[(pYHigh*array_width+pXLow)*2+1]+u*(1-v)*coordY_m1B[(pYLow*array_width+pXHigh)*2+1]+u*v*coordY_m1B[(pYHigh*array_width+pXHigh)*2+1];

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
				coordY_m2B[( nI*ROIWidth+ nJ)*2+0] = fOrgJ;
				coordY_m2B[( nI*ROIWidth+nJ)*2+1] = fOrgI;

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

				coordY[(nI*i_nNewWidth+nJ)*2+0] =  (1-u)*(1-v)*coordY_m2B[(pYLow*ROIWidth+pXLow)*2+0]+(1-u)*v*coordY_m2B[(pYHigh*ROIWidth+pXLow)*2+0]+u*(1-v)*coordY_m2B[(pYLow*ROIWidth+pXHigh)*2+0]+u*v*coordY_m2B[(pYHigh*ROIWidth+pXHigh)*2+0];
				coordY[(nI*i_nNewWidth+nJ)*2+1] =  (1-u)*(1-v)*coordY_m2B[(pYLow*ROIWidth+pXLow)*2+1]+(1-u)*v*coordY_m2B[(pYHigh*ROIWidth+pXLow)*2+1]+u*(1-v)*coordY_m2B[(pYLow*ROIWidth+pXHigh)*2+1]+u*v*coordY_m2B[(pYHigh*ROIWidth+pXHigh)*2+1];

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

int BarrelCorrectOpenB(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject, TBCObject* ptBCObject, TBCOpen* ptBCOpen)
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

	printf("ratio1 = %d\n", ratio1);
	printf("ratio2 = %d\n", ratio2);

	f32Ratio1 = (float)(ratio1)/10000000000.0;
	f32Ratio2= (float)(ratio2)/10000000000000000.0;

	BarrelCorrectInitB(width, height, newWidth, newHeight, f32Ratio1,f32Ratio2,coordY, coordUV,flagROIMode ,f32x0,f32y0,f32w,f32h);
	ISPprintLog("BarrelCorrectInitC succeed!\n");

	BCcoordYYB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFXB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFYB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFXYCHARB = (unsigned char*)malloc(newWidth*newHeight*4*sizeof(unsigned char));
	BCcoordUUVB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int)/4);


	if ( (NULL == BCcoordYYB ) ||(NULL == BCcoordYYFXB ) ||(NULL == BCcoordYYFYB ) ||(NULL == BCcoordYYFXYCHARB )||(NULL == BCcoordUUVB) )
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

			BCcoordYYFXB[(a+i)] = (coordY[(a+i)*2] - x)*16;
			BCcoordYYFYB[(a+i)] = (coordY[(a+i)*2+1] - y)*16;
			BCcoordYYB[(a+i)] = y*width + x;

			BCcoordYYFXYCHARB[4*(a+i)] = CLIP((16- BCcoordYYFXB[(a+i)])*(16-BCcoordYYFYB[(a+i)]));
			BCcoordYYFXYCHARB[4*(a+i)+1] = BCcoordYYFXB[(a+i)]*(16-BCcoordYYFYB[(a+i)]);
			BCcoordYYFXYCHARB[4*(a+i)+2] = (16- BCcoordYYFXB[(a+i)])*BCcoordYYFYB[(a+i)];
			BCcoordYYFXYCHARB[4*(a+i)+3] = BCcoordYYFXB[(a+i)]*BCcoordYYFYB[(a+i)];
		}
	}
	for (j = 0; j < newHeight/2; j++)
	{
		a = j*newWidth/2;
		for (i = 0; i < newWidth/2; i++)
		{
			x=coordUV[(a+i)*2];
			y=coordUV[(a+i)*2+1];
			BCcoordUUVB[a + i] = y/2 * width + x/2*2;
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

	g1B_exit_flag = 0;
	g2B_exit_flag = 0;
	g3B_exit_flag = 0;
	g4B_exit_flag = 0;
	g5B_exit_flag = 0;
	g6B_exit_flag = 0;
	g7B_exit_flag = 0;

	int ret = 0;

	ret = pthread_create(&g_hThread1B, NULL, (void *)ProcessBarrelCThread1B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread1B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread2B, NULL, (void *)ProcessBarrelCThread2B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread2B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread3B, NULL, (void *)ProcessBarrelCThread3B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread3B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread4B, NULL, (void *)ProcessBarrelCThread4B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread4B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread5B, NULL, (void *)ProcessBarrelCThread5B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread5B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread6B, NULL, (void *)ProcessBarrelCThread6B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread6B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread7B, NULL, (void *)ProcessBarrelCThread7B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread7B create failed.");
		return -1;
	}

#ifdef UUVSETBL12
	g8B_exit_flag = 0;
	g9B_exit_flag = 0;
	g10B_exit_flag = 0;
	g11B_exit_flag = 0;

	ret = pthread_create(&g_hThread8B, NULL, (void *)ProcessBarrelCThread8B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread8B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread9B, NULL, (void *)ProcessBarrelCThread9B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread9B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread10B, NULL, (void *)ProcessBarrelCThread10B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread10B create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread11B, NULL, (void *)ProcessBarrelCThread11B, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread11B create failed.");
		return -1;
	}
#endif

	return SUCCESS_GPUALG;
}

int BarrelCorrectProcessB(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject,
							TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput)
{
	struct timeval time1, time2;
	struct timezone tzone;
	double total_time = 0;
	double total_time_single = 0;
	double total_time_single_max = 0;
	double total_time_single_min = 10000;

	gettimeofday(&time1, &tzone);

	pSrcYBCB = ptBarrelInput->tImageBuffer[0].pu8ImageDataY;
	pSrcUBCB = ptBarrelInput->tImageBuffer[0].pu8ImageDataU;

	BCYDataB = ptBarrelOutput->tImageBuffer[0].pu8ImageDataY;
	BCUVDataB = ptBarrelOutput->tImageBuffer[0].pu8ImageDataU;


	BCDataThread1BOUTBool = 0;
	BCDataThread2BOUTBool = 0;
	BCDataThread3BOUTBool = 0;
	BCDataThread4BOUTBool = 0;
	BCDataThread5BOUTBool = 0;
	BCDataThread6BOUTBool = 0;
	BCDataThread7BOUTBool = 0;
	BCDataThread1BBool = 1;
	BCDataThread2BBool = 1;
	BCDataThread3BBool = 1;
	BCDataThread4BBool = 1;
	BCDataThread5BBool = 1;
	BCDataThread6BBool = 1;
	BCDataThread7BBool = 1;
	pthread_mutex_lock(&mutexBC_thread1B);
	pthread_cond_signal(&BCCond_thread1B);
	pthread_mutex_unlock(&mutexBC_thread1B);
	pthread_mutex_lock(&mutexBC_thread2B);
	pthread_cond_signal(&BCCond_thread2B);
	pthread_mutex_unlock(&mutexBC_thread2B);
	pthread_mutex_lock(&mutexBC_thread3B);
	pthread_cond_signal(&BCCond_thread3B);
	pthread_mutex_unlock(&mutexBC_thread3B);
	pthread_mutex_lock(&mutexBC_thread4B);
	pthread_cond_signal(&BCCond_thread4B);
	pthread_mutex_unlock(&mutexBC_thread4B);
	pthread_mutex_lock(&mutexBC_thread5B);
	pthread_cond_signal(&BCCond_thread5B);
	pthread_mutex_unlock(&mutexBC_thread5B);
	pthread_mutex_lock(&mutexBC_thread6B);
	pthread_cond_signal(&BCCond_thread6B);
	pthread_mutex_unlock(&mutexBC_thread6B);
	pthread_mutex_lock(&mutexBC_thread7B);
	pthread_cond_signal(&BCCond_thread7B);
	pthread_mutex_unlock(&mutexBC_thread7B);

#ifdef UUVSETBL12
	BCDataThread8BOUTBool = 0;
	BCDataThread9BOUTBool = 0;
	BCDataThread10BOUTBool = 0;
	BCDataThread11BOUTBool = 0;

	BCDataThread8BBool = 1;
	BCDataThread9BBool = 1;
	BCDataThread10BBool = 1;
	BCDataThread11BBool = 1;

	pthread_mutex_lock(&mutexBC_thread8B);
	pthread_cond_signal(&BCCond_thread8B);
	pthread_mutex_unlock(&mutexBC_thread8B);
	pthread_mutex_lock(&mutexBC_thread9B);
	pthread_cond_signal(&BCCond_thread9B);
	pthread_mutex_unlock(&mutexBC_thread9B);
	pthread_mutex_lock(&mutexBC_thread10B);
	pthread_cond_signal(&BCCond_thread10B);
	pthread_mutex_unlock(&mutexBC_thread10B);
	pthread_mutex_lock(&mutexBC_thread11B);
	pthread_cond_signal(&BCCond_thread11B);
	pthread_mutex_unlock(&mutexBC_thread11B);
#endif

	ISPprintLog("BarrelCorrectProcessB\n");

	bc_convert0B();

	ISPprintLog("[[[[[MAIN ALL start]]]]]\n");
	pthread_mutex_lock(&mutexBC_mainB);

#ifdef UUVSETBL12
	while((BCDataThread1BOUTBool == 0)||(BCDataThread2BOUTBool == 0)||(BCDataThread3BOUTBool == 0)||(BCDataThread4BOUTBool == 0)||(BCDataThread5BOUTBool == 0)||(BCDataThread6BOUTBool == 0)
	||(BCDataThread7BOUTBool == 0)||(BCDataThread8BOUTBool == 0)||(BCDataThread9BOUTBool == 0)||(BCDataThread10BOUTBool == 0)||(BCDataThread11BOUTBool == 0))
#else
	while((BCDataThread1BOUTBool == 0)||(BCDataThread2BOUTBool == 0)||(BCDataThread3BOUTBool == 0)||(BCDataThread4BOUTBool == 0)||(BCDataThread5BOUTBool == 0)||(BCDataThread6BOUTBool == 0)||(BCDataThread7BOUTBool == 0))
#endif
	{
		pthread_cond_wait(&BCCond_mainB,&mutexBC_mainB);
	}
	pthread_mutex_unlock(&mutexBC_mainB);
	ISPprintLog("[[[[[MAIN ALL END]]]]]\n");

	ptBarrelOutput->tImageBuffer[0].u32Width= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32Height= ptBCObject->dstHeight;
	ptBarrelOutput->tImageBuffer[0].u32PitchY= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32PitchUV= ptBCObject->dstWidth;
	//ptBarrelOutput->tImageBuffer[0].u32PitchY= 1664;
	//ptBarrelOutput->tImageBuffer[0].u32PitchUV= 1664;

	gettimeofday(&time2, &tzone);
	total_time_single = diffB(time1, time2) / 1000.0;
	ISPprintLog("======B=======>single time is %fms\n", total_time_single);

	return SUCCESS_GPUALG;
}

int BarrelCorrectCloseB(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject)
{

	int ret = 0;

	g1B_exit_flag = 1;
	g2B_exit_flag = 1;
	g3B_exit_flag = 1;
	g4B_exit_flag = 1;
	g5B_exit_flag = 1;
	g6B_exit_flag = 1;
	g7B_exit_flag = 1;
	g8B_exit_flag = 1;
	g9B_exit_flag = 1;
	g10B_exit_flag = 1;
	g11B_exit_flag = 1;

	BCDataThread1BOUTBool = 0;
	BCDataThread2BOUTBool = 0;
	BCDataThread3BOUTBool = 0;
	BCDataThread4BOUTBool = 0;
	BCDataThread5BOUTBool = 0;
	BCDataThread6BOUTBool = 0;
	BCDataThread7BOUTBool = 0;
	BCDataThread1BBool = 1;
	BCDataThread2BBool = 1;
	BCDataThread3BBool = 1;
	BCDataThread4BBool = 1;
	BCDataThread5BBool = 1;
	BCDataThread6BBool = 1;
	BCDataThread7BBool = 1;
	pthread_mutex_lock(&mutexBC_thread1B);
	pthread_cond_signal(&BCCond_thread1B);
	pthread_mutex_unlock(&mutexBC_thread1B);
	pthread_mutex_lock(&mutexBC_thread2B);
	pthread_cond_signal(&BCCond_thread2B);
	pthread_mutex_unlock(&mutexBC_thread2B);
	pthread_mutex_lock(&mutexBC_thread3B);
	pthread_cond_signal(&BCCond_thread3B);
	pthread_mutex_unlock(&mutexBC_thread3B);
	pthread_mutex_lock(&mutexBC_thread4B);
	pthread_cond_signal(&BCCond_thread4B);
	pthread_mutex_unlock(&mutexBC_thread4B);
	pthread_mutex_lock(&mutexBC_thread5B);
	pthread_cond_signal(&BCCond_thread5B);
	pthread_mutex_unlock(&mutexBC_thread5B);
	pthread_mutex_lock(&mutexBC_thread6B);
	pthread_cond_signal(&BCCond_thread6B);
	pthread_mutex_unlock(&mutexBC_thread6B);
	pthread_mutex_lock(&mutexBC_thread7B);
	pthread_cond_signal(&BCCond_thread7B);
	pthread_mutex_unlock(&mutexBC_thread7B);

#ifdef UUVSETBL12
	BCDataThread8BOUTBool = 0;
	BCDataThread9BOUTBool = 0;
	BCDataThread10BOUTBool = 0;
	BCDataThread11BOUTBool = 0;

	BCDataThread8BBool = 1;
	BCDataThread9BBool = 1;
	BCDataThread10BBool = 1;
	BCDataThread11BBool = 1;

	pthread_mutex_lock(&mutexBC_thread8B);
	pthread_cond_signal(&BCCond_thread8B);
	pthread_mutex_unlock(&mutexBC_thread8B);
	pthread_mutex_lock(&mutexBC_thread9B);
	pthread_cond_signal(&BCCond_thread9B);
	pthread_mutex_unlock(&mutexBC_thread9B);
	pthread_mutex_lock(&mutexBC_thread10B);
	pthread_cond_signal(&BCCond_thread10B);
	pthread_mutex_unlock(&mutexBC_thread10B);
	pthread_mutex_lock(&mutexBC_thread11B);
	pthread_cond_signal(&BCCond_thread11B);
	pthread_mutex_unlock(&mutexBC_thread11B);
#endif


	ret = pthread_join(g_hThread1B,NULL);
	if (ret != 0)
	{
		printf("Thread1B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread2B,NULL);
	if (ret != 0)
	{
		printf("Thread2B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread3B,NULL);
	if (ret != 0)
	{
		printf("Thread3B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread4B,NULL);
	if (ret != 0)
	{
		printf("Thread4B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread5B,NULL);
	if (ret != 0)
	{
		printf("Thread5B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread6B,NULL);
	if (ret != 0)
	{
		printf("Thread6B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread7B,NULL);
	if (ret != 0)
	{
		printf("Thread7B pthread_join failed!\n");
		return -1;
	}

	#ifdef UUVSETBL12

	ret = pthread_join(g_hThread8B,NULL);
	if (ret != 0)
	{
		printf("Thread8B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread9B,NULL);
	if (ret != 0)
	{
		printf("Thread9B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread10B,NULL);
	if (ret != 0)
	{
		printf("Thread10B pthread_join failed!\n");
		return -1;
	}
	ret = pthread_join(g_hThread11B,NULL);
	if (ret != 0)
	{
		printf("Thread11B pthread_join failed!\n");
		return -1;
	}

       #endif

	free(BCcoordYYB);
	free(BCcoordYYFXB);
	free(BCcoordYYFYB);
	free(BCcoordYYFXYCHARB);
	free(BCcoordUUVB);
	free(ptBCObject->pCoordY);
	free(ptBCObject->pCoordUV);
	free(coordY_m1B);
	free(coordUV_m1B);
	free(coordY_m2B);
	free(coordUV_m2B);

	BCcoordYYB = NULL;
	BCcoordYYFXB = NULL;
	BCcoordYYFYB = NULL;
	BCcoordYYFXYCHARB = NULL;
	BCcoordUUVB = NULL;
	ptBCObject->pCoordY = NULL;
	ptBCObject->pCoordUV = NULL;
	coordY_m1B = NULL;
	coordUV_m1B = NULL;
	coordY_m2B = NULL;
	coordUV_m2B = NULL;


	return 0;
}
