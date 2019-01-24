#ifndef _CPUBARRELCORRECTION_H
#define _CPUBARRELCORRECTION_H

#include "CPUCommon.h"
#include "CPUBCCommon.h"
#include "CPUBC.h"

#ifdef __cplusplus
extern "C" {
#endif

int BarrelCorrectionOpen(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject,
                            TBarrelCorrectionOpen* ptBarrelCorrectionOpen);

int BarrelCorrectionProcess(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject,
                            TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput);

int BarrelCorrectionClose(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject);

#ifdef __cplusplus
}
#endif

#endif
