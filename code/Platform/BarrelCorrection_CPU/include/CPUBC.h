#ifndef _CPU_BC_H
#define _CPU_BC_H

#include "opencl.h"

#include "ISPVideoAlg.h"

#include "CPUCommon.h"
#include "CPUBCCommon.h"
#include "BC.h"

#ifdef __cplusplus
extern "C" {
#endif


int BarrelCorrectOpen(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject, 
						TBCObject* ptBCObject, TBCOpen* ptBCOpen);		


int BarrelCorrectProcess(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject, 
							TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput);


int BarrelCorrectClose(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject);

#ifdef __cplusplus
}
#endif
#endif
