#ifndef _CPUBC_COMMON_H
#define _CPUBC_COMMON_H
#include "opencl.h"

typedef struct tagBCObject
{
	cl_kernel BarrelCorrectKernelY;
	cl_kernel ImageSharpKernel;
	cl_kernel BarrelCorrectKernelUV;

	int srcWidth;
	int srcHeight;

	int srcPitchY;
	int srcPitchUV;

	int dstWidth;
	int dstHeight;

	cl_mem memSrcImageY;
	cl_mem memSrcImageSharpY;
	cl_mem memSrcImageUV;
	cl_mem memDstImageY;
	cl_mem memDstImageUV;
	cl_mem memCoordY;
	cl_mem memCoordUV;

	int srcYRowPitch;
	int srcUVRowPitch;
	int dstYRowPitch;
	int dstUVRowPitch;

	int sharpThre;

	float* pCoordY;
	float* pCoordUV;

	unsigned char* pSrcImageY;
	unsigned char* pSrcImageUV;

	unsigned char* pDstImageY;
	unsigned char* pDstImageUV;
	int ouputMaptFlag;
	char path[200];
}TBCObject;

typedef struct tagBC_VSObject
{
	cl_kernel BarrelCorrectKernelY;
	cl_kernel BarrelCorrectKernelUV;

	int srcWidth;
	int srcHeight;

	int srcPitchY;
	int srcPitchUV;

	int dstWidth;
	int dstHeight;

	cl_mem memSrcImageY;
	cl_mem memSrcImageUV;
	cl_mem memDstImageY;
	cl_mem memDstImageUV;
	cl_mem memCoordY;
	cl_mem memCoordUV;

	int srcYRowPitch;
	int srcUVRowPitch;
	int dstYRowPitch;
	int dstUVRowPitch;

	float* pCoordY;
	float* pCoordUV;

	unsigned char* pSrcImageY;
	unsigned char* pSrcImageUV;
	unsigned char* pDstImageY;
	unsigned char* pDstImageUV;
	char path[200];

	float scale;
	float* pVertRatio;
	int* pVertIdxShift;
	int size;
	
	cl_kernel VirtualStitchHKernelY;
	cl_kernel VirtualStitchVKernelY;
	cl_kernel VirtualStitchHKernelUV;
	cl_kernel VirtualStitchVKernelUV;

	cl_mem memVertRatio;
	cl_mem memVertIdxShift;
	cl_mem memVirtualImgY;
	cl_mem memVirtualImgUV;
	cl_mem memTempY;
	cl_mem memTempUV;
	cl_mem memWeight1;
	cl_mem memWeight2;
	cl_mem memK;
	cl_mem memWeight1UV;
	cl_mem memWeight2UV;
	cl_mem memKUV;
	cl_mem memVirtualCoordY;
	cl_mem memVirtualCoordUV;

	int virtualWidthOut;
	int virtualHeightOut;
	int virtualRowPitchY;
	int virtualRowPitchUV;
	unsigned char* pVirtualImgOutY;
	unsigned char* pVirtualImgOutUV;

	unsigned char* pTempY;
	unsigned char* pTempUV;

	float* pVirtualCoordY;
	float* pVirtualCoordUV;
	int* pK;
	int* pWeight1;
	int* pWeight2;
	int* pKUV;
	int* pWeight1UV;
	int* pWeight2UV;

	int ouputMaptFlag;
}TBC_VSObject;

typedef struct tagBarrelCorrectionObject
{
	cl_program program;
	TBCObject tBCObject;
	TBC_VSObject tBC_VSObject;
	int flag;
}TBarrelCorrectionObject;

#endif
