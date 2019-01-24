#ifndef _CPUBC_VS_H
#define _CPUBC_VS_H
#include "ISPVideoAlg.h"
#include "opencl.h"
#include "CPUCommon.h"
#include "CPUBC.h"
#include "CPUBCCommon.h"

#ifdef __cplusplus
extern "C" {
#endif


// ��ʼ���ӿ�
int BC_VSOpen(TPlatformObject* ptPlatformObject,
				TBarrelCorrectionObject* ptBarrelCorrectionObject, 
				TBC_VSObject* ptBC_VSObject, 
				TBC_VSOpen* ptBarrelCorrectionOpen);		

// ÿ֡�����ӿ�
int BC_VSProcess(TPlatformObject* ptPlatformObject,
					TBC_VSObject	* ptBC_VSObject, 
					TISPImageInfo* ptBC_VSInput, 
					TISPImageInfo* ptBC_VSOutput);

// ��Դ�ͷŽӿ�
int BC_VSClose(TPlatformObject* ptPlatformObject, TBC_VSObject* ptBC_VSObject);

#ifdef __cplusplus
}
#endif
#endif