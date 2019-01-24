#ifndef _BC_H
#define _BC_H
#include "ISPVideoAlg.h"


typedef struct tagBCOpen
{
	unsigned int u32InputWidth;
	unsigned int u32InputHeight;

	unsigned int u32InputPitchY;
	unsigned int u32InputPitchUV;

	unsigned int u32OutputWidth;
	unsigned int u32OutputHeight;
	
	unsigned int flagROIMode;
	float f32x0;
	float f32y0;
	float f32w;
	float f32h;
	
	unsigned int u32SharpThre;
	unsigned int u32Ratio1;
	unsigned int u32Ratio2;
	EMISPImageFormat emFormat;
}TBCOpen;


typedef struct tagBC_VSParam
{
	int s32BarrelParam1;
	int s32BarrelParam2;

	int s32TeleZoomInRatio;
	int s32TeleCurvLevel;
	int s32TeleMultVal;
	int s32WideRatio;

	int s32MidWidth;
	int s32MidHeight;

	int s32AdjHeight;
	float f32WidthScale;
}TBC_VSParam;

typedef struct tagBC_VSOpen
{
	unsigned int u32InputWidth;
	unsigned int u32InputHeight;

	unsigned int u32InputPitchY;
	unsigned int u32InputPitchUV;

	unsigned int u32OutputWidth;
	unsigned int u32OutputHeight;

    TBC_VSParam tBC_VSParam;
	EMISPImageFormat emFormat;	
}TBC_VSOpen;


typedef struct tagBarrelCorrectionOpen
{
	TBCOpen tBCOpen;
	TBC_VSOpen tBC_VSOpen;
	int flag;
	char s8BinaryPath[200];
}TBarrelCorrectionOpen;


#endif