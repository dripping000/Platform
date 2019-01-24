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


unsigned int* BCcoordYYC = NULL;
unsigned int* BCcoordYYFXC = NULL;
unsigned int* BCcoordYYFYC = NULL;
unsigned char* BCcoordYYFXYCHARC = NULL;
unsigned int* BCcoordUUVC = NULL;
unsigned char* BCYDataC = NULL;
unsigned char* BCUVDataC = NULL;
unsigned char* pSrcYBCC = NULL;
unsigned char* pSrcUBCC = NULL;

float* coordY_m1C = NULL ;
float* coordUV_m1C = NULL ;
float* coordY_m2C = NULL ;
float* coordUV_m2C = NULL ;

pthread_t g_hThread1C;
pthread_t g_hThread2C;
pthread_t g_hThread3C;
pthread_t g_hThread4C;
pthread_t g_hThread5C;
pthread_t g_hThread6C;
pthread_t g_hThread7C;
pthread_t g_hThread8C;
pthread_t g_hThread9C;
pthread_t g_hThread10C;
pthread_t g_hThread11C;

int g1C_exit_flag;
int g2C_exit_flag;
int g3C_exit_flag;
int g4C_exit_flag;
int g5C_exit_flag;
int g6C_exit_flag;
int g7C_exit_flag;
int g8C_exit_flag;
int g9C_exit_flag;
int g10C_exit_flag;
int g11C_exit_flag;

int BCDataThread1CBool = 0;
int BCDataThread2CBool = 0;
int BCDataThread3CBool = 0;
int BCDataThread4CBool = 0;
int BCDataThread5CBool = 0;
int BCDataThread6CBool = 0;
int BCDataThread7CBool = 0;
int BCDataThread8CBool = 0;
int BCDataThread9CBool = 0;
int BCDataThread10CBool = 0;
int BCDataThread11CBool = 0;

int BCDataThread1COUTBool = 0;
int BCDataThread2COUTBool = 0;
int BCDataThread3COUTBool  = 0;
int BCDataThread4COUTBool = 0;
int BCDataThread5COUTBool = 0;
int BCDataThread6COUTBool = 0;
int BCDataThread7COUTBool = 0;
int BCDataThread8COUTBool = 0;
int BCDataThread9COUTBool = 0;
int BCDataThread10COUTBool = 0;
int BCDataThread11COUTBool = 0;

pthread_mutex_t mutexBC_thread1C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread1C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread2C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread2C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread3C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread3C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread4C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread4C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread5C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread5C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread6C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread6C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread7C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread7C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread8C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread8C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread9C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread9C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread10C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread10C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_thread11C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_thread11C = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexBC_mainC = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t BCCond_mainC = PTHREAD_COND_INITIALIZER;

long diffC(struct timeval start, struct timeval end)
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

void bc_convert0C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 0; i < 1200/12*1; i++)

#else

	for (i = 0; i < 1200/8*1; i++)


#endif
{

	for (j = 0; j < 1600; j += 8)
	{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	    	uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
	}
}

#ifdef UUVSETBL12

	for (i = 0; i < 1200/2/12*1; i++)

#else

	for (i = 0; i < 1200/2/8*1; i++)

#endif
{

	 for (j = 0; j < 1600/2; j ++)
	{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
	}
}

}
void bc_convert1C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*1; i < 1200/12*2; i++)


#else

	for (i = 1200/8*1; i < 1200/8*2; i++)

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*1; i < 1200/2/12*2; i++)

#else

	for (i = 1200/2/8*1; i < 1200/2/8*2; i++)

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert2C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*2; i < 1200/12*3; i++)


#else

	for (i = 1200/8*2; i < 1200/8*3; i++)

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*2; i < 1200/2/12*3; i++)

#else

	for ( i < 1200/2/8*2; i < 1200/2/8*3; i++)

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}


void bc_convert3C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*3; i < 1200/12*4; i++)


#else

	for (i = 1200/8*3; i < 1200/8*4; i++)

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*3; i < 1200/2/12*4; i++)

#else

	for ( i < 1200/2/8*3; i < 1200/2/8*4; i++)

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert4C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*4; i < 1200/12*5; i++)


#else

	for (i = 1200/8*4; i < 1200/8*5; i++)

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*4; i < 1200/2/12*5; i++)

#else

	for ( i < 1200/2/8*4; i < 1200/2/8*5; i++)

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert5C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*5; i < 1200/12*6; i++)


#else

	for (i = 1200/8*5; i < 1200/8*6; i++)

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*5; i < 1200/2/12*6; i++)

#else

	for ( i < 1200/2/8*5; i < 1200/2/8*6; i++)

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert6C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*6; i < 1200/12*7; i++)


#else

	for (i = 1200/8*6; i < 1200/8*7; i++)

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*6; i < 1200/2/12*7; i++)

#else

	for ( i < 1200/2/8*6; i < 1200/2/8*7; i++)

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert7C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*7; i < 1200/12*8; i++)


#else

	for (i = 1200/8*7; i < 1200/8*8; i++)

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*7; i < 1200/2/12*8; i++)

#else

	for ( i < 1200/2/8*7; i < 1200/2/8*8; i++)

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert8C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*8; i < 1200/12*9; i++)


#else

	return;

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*8; i < 1200/2/12*9; i++)

#else

	return;

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert9C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*9; i < 1200/12*10; i++)


#else

	return;

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*9; i < 1200/2/12*10; i++)

#else

	return;

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert10C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*10; i < 1200/12*11; i++)


#else

	return;

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*10; i < 1200/2/12*11; i++)

#else

	return;

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void bc_convert11C ()
{
	int i,j;
	unsigned short *uv1;

#ifdef UUVSETBL12

	for (i = 1200/12*11; i < 1200/12*12; i++)


#else

	return;

#endif
	{

		for (j = 0; j < 1600; j += 8)
		{
		uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARC+j*4);

		uint8x8_t x0y0;
		uint8x8_t x1y0;
		uint8x8_t x0y1;
		uint8x8_t x1y1;

		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]],x0y0,0);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]],x0y0,1);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]],x0y0,2);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]],x0y0,3);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]],x0y0,4);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]],x0y0,5);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]],x0y0,6);
		x0y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]],x0y0,7);

		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+1],x1y0,0);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+1],x1y0,1);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+1],x1y0,2);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+1],x1y0,3);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+1],x1y0,4);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+1],x1y0,5);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+1],x1y0,6);
		x1y0 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+1],x1y0,7);

		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHY],x0y1,0);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHY],x0y1,1);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHY],x0y1,2);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHY],x0y1,3);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHY],x0y1,4);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHY],x0y1,5);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHY],x0y1,6);
		x0y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHY],x0y1,7);

		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j]+PITCHYADD1],x1y1,0);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+1]+PITCHYADD1],x1y1,1);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+2]+PITCHYADD1],x1y1,2);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+3]+PITCHYADD1],x1y1,3);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+4]+PITCHYADD1],x1y1,4);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+5]+PITCHYADD1],x1y1,5);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+6]+PITCHYADD1],x1y1,6);
		x1y1 = vset_lane_u8(pSrcYBCC[BCcoordYYC[1600*i+j+7]+PITCHYADD1],x1y1,7);

		uint16x8_t temp;
		temp = vmull_u8(x0y0, fxy.val[0]);
		temp = vmlal_u8(temp, x1y0, fxy.val[1]);
		temp = vmlal_u8(temp, x0y1, fxy.val[2]);
		temp = vmlal_u8(temp, x1y1, fxy.val[3]);
	       uint8x8_t result = vshrn_n_u16 (temp, 8);
    		vst1_u8 (BCYDataC+1664*i+j, result);
		}
      }

#ifdef UUVSETBL12

	for (i = 1200/2/12*11; i < 1200/2/12*12; i++)

#else

	return;

#endif
	{
 		for (j = 0; j < 1600/2; j ++)
		{
		uv1 = (unsigned short *)&pSrcUBCC[BCcoordUUVC[1600/2*i+j]];
		(BCUVDataC[1664*i+(j<<1)]) = ((*uv1)&0xff);
		BCUVDataC[1664*i+(j<<1) +1] = ((*uv1>>8)&0xff);
		}
	}

}

void ProcessBarrelCThread1C()
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

	while(0 == g1C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread1C enter\n");
		pthread_mutex_lock(&mutexBC_thread1C);
		while(BCDataThread1CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread1C,&mutexBC_thread1C);
		}
		pthread_mutex_unlock(&mutexBC_thread1C);
		BCDataThread1CBool=0;
		ISPprintLog("ProcessBarrelCThread1C start\n");
		bc_convert1C();
		BCDataThread1COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
	}
}


void ProcessBarrelCThread2C()
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

    while(0 == g2C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread2C enter\n");

		pthread_mutex_lock(&mutexBC_thread2C);
		while(BCDataThread2CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread2C,&mutexBC_thread2C);
		}
		pthread_mutex_unlock(&mutexBC_thread2C);
		BCDataThread2CBool=0;
		ISPprintLog("ProcessBarrelCThread2C start\n");

		bc_convert2C();

		BCDataThread2COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
	}
}



void ProcessBarrelCThread3C()
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

    while(0 == g3C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread3C enter\n");

		pthread_mutex_lock(&mutexBC_thread3C);
		while( BCDataThread3CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread3C,&mutexBC_thread3C);
		}
		pthread_mutex_unlock(&mutexBC_thread3C);
		 BCDataThread3CBool=0;
		ISPprintLog("ProcessBarrelCThread3C start\n");

		bc_convert3C();

		BCDataThread3COUTBool  = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
	}
}



void ProcessBarrelCThread4C()
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

    while(0 == g4C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread4C enter\n");

		pthread_mutex_lock(&mutexBC_thread4C);
		while(BCDataThread4CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread4C,&mutexBC_thread4C);
		}
		pthread_mutex_unlock(&mutexBC_thread4C);
		BCDataThread4CBool=0;
		ISPprintLog("ProcessBarrelCThread4C start\n");

		bc_convert4C();

		BCDataThread4COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);

	}
}

void ProcessBarrelCThread5C()
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

    while(0 == g5C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread5C enter\n");

		pthread_mutex_lock(&mutexBC_thread5C);
		while(BCDataThread5CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread5C,&mutexBC_thread5C);
		}
		pthread_mutex_unlock(&mutexBC_thread5C);
		BCDataThread5CBool=0;
		ISPprintLog("ProcessBarrelCThread5C start\n");

		bc_convert5C();

		BCDataThread5COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
		}
}

void ProcessBarrelCThread6C()
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

    while(0 == g6C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread6C enter\n");

		pthread_mutex_lock(&mutexBC_thread6C);
		while(BCDataThread6CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread6C,&mutexBC_thread6C);
		}
		pthread_mutex_unlock(&mutexBC_thread6C);
		BCDataThread6CBool=0;

		ISPprintLog("ProcessBarrelCThread6C start\n");

		bc_convert6C();

		BCDataThread6COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
		}
}


void ProcessBarrelCThread7C()
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

    while(0 == g7C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread7C enter\n");

		pthread_mutex_lock(&mutexBC_thread7C);
		while(BCDataThread7CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread7C,&mutexBC_thread7C);
		}
		pthread_mutex_unlock(&mutexBC_thread7C);
		BCDataThread7CBool=0;

		ISPprintLog("ProcessBarrelCThread7C start\n");
		bc_convert7C();

		BCDataThread7COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
		}
}

void ProcessBarrelCThread8C()
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

    while(0 == g8C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread8C enter\n");

		pthread_mutex_lock(&mutexBC_thread8C);
		while(BCDataThread8CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread8C,&mutexBC_thread8C);
		}
		pthread_mutex_unlock(&mutexBC_thread8C);
		BCDataThread8CBool=0;

		ISPprintLog("ProcessBarrelCThread8C start\n");

		bc_convert8C();

		BCDataThread8COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
		}
}

void ProcessBarrelCThread9C()
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

    while(0 == g9C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread9C enter\n");

		pthread_mutex_lock(&mutexBC_thread9C);
		while(BCDataThread9CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread9C,&mutexBC_thread9C);
		}
		pthread_mutex_unlock(&mutexBC_thread9C);
		BCDataThread9CBool=0;

		ISPprintLog("ProcessBarrelCThread9C start\n");

		bc_convert9C();

		BCDataThread9COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
		}
}

void ProcessBarrelCThread10C()
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

    while(0 == g10C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread10C enter\n");

		pthread_mutex_lock(&mutexBC_thread10C);
		while(BCDataThread10CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread10C,&mutexBC_thread10C);
		}
		pthread_mutex_unlock(&mutexBC_thread10C);
		BCDataThread10CBool=0;

		ISPprintLog("ProcessBarrelCThread10C start\n");

		bc_convert10C();

		BCDataThread10COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
		}
}

void ProcessBarrelCThread11C()
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

    while(0 == g11C_exit_flag)
	{
		ISPprintLog("ProcessBarrelCThread11C enter\n");
		pthread_mutex_lock(&mutexBC_thread11C);
		while(BCDataThread11CBool == 0)
		{
			pthread_cond_wait(&BCCond_thread11C,&mutexBC_thread11C);
		}
		pthread_mutex_unlock(&mutexBC_thread11C);
		BCDataThread11CBool=0;
		ISPprintLog("ProcessBarrelCThread11C start\n");
		bc_convert11C();

		BCDataThread11COUTBool = 1;
		pthread_mutex_lock(&mutexBC_mainC);
		pthread_cond_signal(&BCCond_mainC);
		pthread_mutex_unlock(&mutexBC_mainC);
		}
}

int BarrelCorrectInitC(int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight,
					float i_f32Ratio1, float i_f32Ratio2,
				 	float* coordY, float* coordUV,int flagROIMode,float f32x0,float f32y0,float f32w,float f32h)
{
	float vary, vary1, varall;

	int nI, nJ;
	float deltx,delty;
	float scaleDownV = 1.0f;
	float scaleDownH = 1.0f;
	int nNewHalfW, nNewHalfH, nNewWidthStep;
	int nOrgJ, nOrgI, nHalfWidth, nHalfHeight, nWidthStep, ndiffC;
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

	coordY_m1C= (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2);

	coordUV_m1C = (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2/4);

	coordY_m2C= (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2);

	coordUV_m2C = (float*)malloc(i_nWidth*i_nHeight*sizeof(float)*2/4);

	if( (coordY_m1C == NULL) ||(coordUV_m1C== NULL)||(coordY_m2C == NULL)||(coordUV_m2C == NULL))
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

			nOrgJ = (deltx*varall+nHalfWidth)*1024;
			nOrgI = (delty*varall+nHalfHeight)*1024;
			nOrgJLow = nOrgJ/1024;
			nOrgJHigh = nOrgJLow+1;
			nOrgILow = nOrgI/1024;
			nOrgIHigh = nOrgILow+1;

			if(nOrgJLow>0 && nOrgJHigh < i_nWidth-1 && nOrgILow>0 && nOrgIHigh < i_nHeight-1)
			{
				ndiffC=nOrgJ-(nOrgJLow<<10);
				coordY[(nI*i_nNewWidth+nJ)*2+0] = ((float)nOrgJLow + (float)ndiffC/(float)1024);
				ndiffC=nOrgI-nOrgILow*1024;
				coordY[(nI*i_nNewWidth+nJ)*2+1] = ((float)nOrgILow + (float)ndiffC/(float)1024);

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
				coordY_m1C[( (nI-nI_min)*array_width+ nJ-nJ_min)*2+0] = fOrgJ;
				coordY_m1C[((nI-nI_min)*array_width+nJ-nJ_min)*2+1] = fOrgI;
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


				coordY[(nI*i_nNewWidth+nJ)*2+0] =  (1-u)*(1-v)*coordY_m1C[(pYLow*array_width+pXLow)*2+0]+(1-u)*v*coordY_m1C[(pYHigh*array_width+pXLow)*2+0]+u*(1-v)*coordY_m1C[(pYLow*array_width+pXHigh)*2+0]+u*v*coordY_m1C[(pYHigh*array_width+pXHigh)*2+0];
				coordY[(nI*i_nNewWidth+nJ)*2+1] =  (1-u)*(1-v)*coordY_m1C[(pYLow*array_width+pXLow)*2+1]+(1-u)*v*coordY_m1C[(pYHigh*array_width+pXLow)*2+1]+u*(1-v)*coordY_m1C[(pYLow*array_width+pXHigh)*2+1]+u*v*coordY_m1C[(pYHigh*array_width+pXHigh)*2+1];

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
				coordY_m2C[( nI*ROIWidth+ nJ)*2+0] = fOrgJ;
				coordY_m2C[( nI*ROIWidth+nJ)*2+1] = fOrgI;

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

				coordY[(nI*i_nNewWidth+nJ)*2+0] =  (1-u)*(1-v)*coordY_m2C[(pYLow*ROIWidth+pXLow)*2+0]+(1-u)*v*coordY_m2C[(pYHigh*ROIWidth+pXLow)*2+0]+u*(1-v)*coordY_m2C[(pYLow*ROIWidth+pXHigh)*2+0]+u*v*coordY_m2C[(pYHigh*ROIWidth+pXHigh)*2+0];
				coordY[(nI*i_nNewWidth+nJ)*2+1] =  (1-u)*(1-v)*coordY_m2C[(pYLow*ROIWidth+pXLow)*2+1]+(1-u)*v*coordY_m2C[(pYHigh*ROIWidth+pXLow)*2+1]+u*(1-v)*coordY_m2C[(pYLow*ROIWidth+pXHigh)*2+1]+u*v*coordY_m2C[(pYHigh*ROIWidth+pXHigh)*2+1];

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

int BarrelCorrectOpenC(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject, TBCObject* ptBCObject, TBCOpen* ptBCOpen)
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

	BarrelCorrectInitC(width, height, newWidth, newHeight, f32Ratio1,f32Ratio2,coordY, coordUV,flagROIMode ,f32x0,f32y0,f32w,f32h);
	ISPprintLog("BarrelCorrectInitC succeed!\n");

	BCcoordYYC = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFXC = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFYC = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
	BCcoordYYFXYCHARC = (unsigned char*)malloc(newWidth*newHeight*4*sizeof(unsigned char));
	BCcoordUUVC = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int)/4);

	if ( (NULL == BCcoordYYC ) ||(NULL == BCcoordYYFXC ) ||(NULL == BCcoordYYFYC ) ||(NULL == BCcoordYYFXYCHARC )||(NULL == BCcoordUUVC) )
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

			BCcoordYYFXC[(a+i)] = (coordY[(a+i)*2] - x)*16;
			BCcoordYYFYC[(a+i)] = (coordY[(a+i)*2+1] - y)*16;
			BCcoordYYC[(a+i)] = y*width + x;

			BCcoordYYFXYCHARC[4*(a+i)] = CLIP((16- BCcoordYYFXC[(a+i)])*(16-BCcoordYYFYC[(a+i)]));
			BCcoordYYFXYCHARC[4*(a+i)+1] = BCcoordYYFXC[(a+i)]*(16-BCcoordYYFYC[(a+i)]);
			BCcoordYYFXYCHARC[4*(a+i)+2] = (16- BCcoordYYFXC[(a+i)])*BCcoordYYFYC[(a+i)];
			BCcoordYYFXYCHARC[4*(a+i)+3] = BCcoordYYFXC[(a+i)]*BCcoordYYFYC[(a+i)];
		}
	}
	for (j = 0; j < newHeight/2; j++)
	{
		a = j*newWidth/2;
		for (i = 0; i < newWidth/2; i++)
		{
			x=coordUV[(a+i)*2];
			y=coordUV[(a+i)*2+1];
			BCcoordUUVC[a + i] = y/2 * width + x/2*2;
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

	g1C_exit_flag = 0;
	g2C_exit_flag = 0;
	g3C_exit_flag = 0;
	g4C_exit_flag = 0;
	g5C_exit_flag = 0;
	g6C_exit_flag = 0;
	g7C_exit_flag = 0;

	int ret = 0;
	ret = pthread_create(&g_hThread1C, NULL, (void *)ProcessBarrelCThread1C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread1C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread2C, NULL, (void *)ProcessBarrelCThread2C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread2C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread3C, NULL, (void *)ProcessBarrelCThread3C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread3C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread4C, NULL, (void *)ProcessBarrelCThread4C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread4C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread5C, NULL, (void *)ProcessBarrelCThread5C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread5C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread6C, NULL, (void *)ProcessBarrelCThread6C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread6C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread7C, NULL, (void *)ProcessBarrelCThread7C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread7C create failed.");
		return -1;
	}

#ifdef UUVSETBL12
	g8C_exit_flag = 0;
	g9C_exit_flag = 0;
	g10C_exit_flag = 0;
	g11C_exit_flag = 0;

	ret = pthread_create(&g_hThread8C, NULL, (void *)ProcessBarrelCThread8C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread8C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread9C, NULL, (void *)ProcessBarrelCThread9C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread9C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread10C, NULL, (void *)ProcessBarrelCThread10C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread10C create failed.");
		return -1;
	}
	ret = pthread_create(&g_hThread11C, NULL, (void *)ProcessBarrelCThread11C, NULL);
	if (ret != 0)
	{
		printf("ProcessBarrelCThread11C create failed.");
		return -1;
	}
#endif

	return SUCCESS_GPUALG;
}

int BarrelCorrectProcessC(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject,
							TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput)
{
	struct timeval time1, time2;
	struct timezone tzone;
	double total_time = 0;
	double total_time_single = 0;
	double total_time_single_max = 0;
	double total_time_single_min = 10000;

	gettimeofday(&time1, &tzone);

	pSrcYBCC = ptBarrelInput->tImageBuffer[0].pu8ImageDataY;
	pSrcUBCC = ptBarrelInput->tImageBuffer[0].pu8ImageDataU;

	BCYDataC = ptBarrelOutput->tImageBuffer[0].pu8ImageDataY;
	BCUVDataC = ptBarrelOutput->tImageBuffer[0].pu8ImageDataU;

	BCDataThread1COUTBool = 0;
	BCDataThread2COUTBool = 0;
	BCDataThread3COUTBool  = 0;
	BCDataThread4COUTBool = 0;
	BCDataThread5COUTBool = 0;
	BCDataThread6COUTBool = 0;
	BCDataThread7COUTBool = 0;
	BCDataThread1CBool = 1;
	BCDataThread2CBool = 1;
	 BCDataThread3CBool = 1;
	BCDataThread4CBool = 1;
	BCDataThread5CBool = 1;
	BCDataThread6CBool = 1;
	BCDataThread7CBool = 1;
	pthread_mutex_lock(&mutexBC_thread1C);
	pthread_cond_signal(&BCCond_thread1C);
	pthread_mutex_unlock(&mutexBC_thread1C);
	pthread_mutex_lock(&mutexBC_thread2C);
	pthread_cond_signal(&BCCond_thread2C);
	pthread_mutex_unlock(&mutexBC_thread2C);
	pthread_mutex_lock(&mutexBC_thread3C);
	pthread_cond_signal(&BCCond_thread3C);
	pthread_mutex_unlock(&mutexBC_thread3C);
	pthread_mutex_lock(&mutexBC_thread4C);
	pthread_cond_signal(&BCCond_thread4C);
	pthread_mutex_unlock(&mutexBC_thread4C);
	pthread_mutex_lock(&mutexBC_thread5C);
	pthread_cond_signal(&BCCond_thread5C);
	pthread_mutex_unlock(&mutexBC_thread5C);
	pthread_mutex_lock(&mutexBC_thread6C);
	pthread_cond_signal(&BCCond_thread6C);
	pthread_mutex_unlock(&mutexBC_thread6C);
	pthread_mutex_lock(&mutexBC_thread7C);
	pthread_cond_signal(&BCCond_thread7C);
	pthread_mutex_unlock(&mutexBC_thread7C);

#ifdef UUVSETBL12
	BCDataThread8COUTBool = 0;
	BCDataThread9COUTBool = 0;
	BCDataThread10COUTBool = 0;
	BCDataThread11COUTBool = 0;

	BCDataThread8CBool = 1;
	BCDataThread9CBool = 1;
	BCDataThread10CBool = 1;
	BCDataThread11CBool = 1;

	pthread_mutex_lock(&mutexBC_thread8C);
	pthread_cond_signal(&BCCond_thread8C);
	pthread_mutex_unlock(&mutexBC_thread8C);
	pthread_mutex_lock(&mutexBC_thread9C);
	pthread_cond_signal(&BCCond_thread9C);
	pthread_mutex_unlock(&mutexBC_thread9C);
	pthread_mutex_lock(&mutexBC_thread10C);
	pthread_cond_signal(&BCCond_thread10C);
	pthread_mutex_unlock(&mutexBC_thread10C);
	pthread_mutex_lock(&mutexBC_thread11C);
	pthread_cond_signal(&BCCond_thread11C);
	pthread_mutex_unlock(&mutexBC_thread11C);
#endif

	ISPprintLog("BarrelCorrectProcessC\n");

	bc_convert0C();

	ISPprintLog("[[[[[MAIN ALL start]]]]]\n");
	pthread_mutex_lock(&mutexBC_mainC);

#ifdef UUVSETBL12
	while((BCDataThread1COUTBool == 0)||(BCDataThread2COUTBool == 0)||(BCDataThread3COUTBool  == 0)||(BCDataThread4COUTBool == 0)||(BCDataThread5COUTBool == 0)||(BCDataThread6COUTBool == 0)
	||(BCDataThread7COUTBool == 0)||(BCDataThread8COUTBool == 0)||(BCDataThread9COUTBool == 0)||(BCDataThread10COUTBool == 0)||(BCDataThread11COUTBool == 0))
#else
	while((BCDataThread1COUTBool == 0)||(BCDataThread2COUTBool == 0)||(BCDataThread3COUTBool  == 0)||(BCDataThread4COUTBool == 0)||(BCDataThread5COUTBool == 0)||(BCDataThread6COUTBool == 0)||(BCDataThread7COUTBool == 0))
#endif
	{
		pthread_cond_wait(&BCCond_mainC,&mutexBC_mainC);
	}
	pthread_mutex_unlock(&mutexBC_mainC);
	ISPprintLog("[[[[[MAIN ALL END]]]]]\n");

	ptBarrelOutput->tImageBuffer[0].u32Width= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32Height= ptBCObject->dstHeight;
	//ptBarrelOutput->tImageBuffer[0].u32PitchY= ptBCObject->dstWidth;
	//ptBarrelOutput->tImageBuffer[0].u32PitchUV= ptBCObject->dstWidth;
	ptBarrelOutput->tImageBuffer[0].u32PitchY= 1664;
	ptBarrelOutput->tImageBuffer[0].u32PitchUV= 1664;

	gettimeofday(&time2, &tzone);
	total_time_single = diffC(time1, time2) / 1000.0;
	ISPprintLog("=======C======>single time is %fms\n", total_time_single);

	return SUCCESS_GPUALG;
}

int BarrelCorrectCloseC(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject)
{
	free(BCcoordYYC);
	free(BCcoordYYFXC);
	free(BCcoordYYFYC);
	free(BCcoordYYFXYCHARC);
	free(BCcoordUUVC);
	free(ptBCObject->pCoordY);
	free(ptBCObject->pCoordUV);
	free(coordY_m1C);
	free(coordUV_m1C);
	free(coordY_m2C);
	free(coordUV_m2C);

	BCcoordYYC = NULL;
	BCcoordYYFXC = NULL;
	BCcoordYYFYC = NULL;
	BCcoordYYFXYCHARC = NULL;
	BCcoordUUVC = NULL;
	ptBCObject->pCoordY = NULL;
	ptBCObject->pCoordUV = NULL;
	coordY_m1C = NULL;
	coordUV_m1C = NULL;
	coordY_m2C = NULL;
	coordUV_m2C = NULL;

	return 0;
}
