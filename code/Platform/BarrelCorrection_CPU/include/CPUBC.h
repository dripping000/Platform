#ifndef CPU_BC_H
#define CPU_BC_H
#include "ISPVideoAlg.h"
#include "opencl.h"
#include "CPUCommon.h"
#include "BC.h"
#include "CPUBCCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

// ��ʼ������У��ģ��
int BarrelCorrectOpen(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject, 
						TBCObject* ptBCObject, TBCOpen* ptBCOpen);		

// ��������У��ģ��
int BarrelCorrectProcess(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject, 
							TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput);

// �ͷŻ���У��ģ��
int BarrelCorrectClose(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject);

#ifdef __cplusplus
}
#endif
#endif
