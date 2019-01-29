
extern "C"{
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
//#include <sys/time.h>
#include <ctype.h>
#include "ISP_Alglog.h"

#include "BC.h"
#include "CPUBC_VS.h"
#include "CPUBarrelCorrection.h"
#include "CPUBC.h"
#include "CPUCommon.h"

#if defined(HAVE_NEON)
#include <arm_neon.h>
#endif
}

#ifdef RUN_ON_QUALC
	#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
	#endif
#include <sched.h>
#include <android/log.h>
#endif

//#include <pthread.h>
//#include <sys/prctl.h>

#include <list> 
#include<vector>
using namespace std;

TPlatformObject tPlatformObject;
static int initFlag = 0;

static unsigned int get_current_time(void)
{
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return 0;
}

/*=========================================================================
函 数 名： ISPVideoAlgInit
功    能： 设备平台初始化
算法实现： 无
参    数： 
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgInit()
{
	if (initFlag == 0)
	{
		initFlag = 1;
	}
	return SUCCESS_GPUALG;
}

/*=========================================================================
函 数 名： ISPVideoAlgRelease
功    能： 设备平台初始化
算法实现： 无
参    数： 
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgRelease()
{
	if (initFlag == 1)
	{
		initFlag = 0;
	}
	return SUCCESS_GPUALG;
}

/*=========================================================================
函 数 名： ISPVideoAlgOpen
功    能： 算法初始化
算法实现： 无
参    数： pvHandle                    算法句柄[in]
           emIMGAlgType                算法类型[in]
		   pvOpen                      初始化结构体指针[in]
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgOpen(void **pvHandle,
                             EMISPAlgType emIMGAlgType,
                             void *pvOpen)
{
    *pvHandle = NULL;
	EMISPRetCode retCode = SUCCESS_GPUALG;

	if (emIMGAlgType == BARRELCORRECTION)
    {
		TBarrelCorrectionObject* ptBarrelCorrectionObject = NULL;
		TBarrelCorrectionOpen* ptBarrelCorrectionOpen = NULL;
		if (initFlag == 0)
		{
			ISPprintLog("please init platform first!\n");
			return ERR_INIT_DEVICE;
		}
		
		ptBarrelCorrectionObject = (TBarrelCorrectionObject*)malloc(sizeof(TBarrelCorrectionObject));
		if (ptBarrelCorrectionObject == NULL)
		{
			ISPprintLog("Handle creation error!\n");
			return ERR_CREATE_HOST_BUFFER;
		}

		ptBarrelCorrectionOpen = (TBarrelCorrectionOpen*)(pvOpen);

		if (ptBarrelCorrectionOpen->flag == 0)
		{
	    		if (ptBarrelCorrectionOpen->tBCOpen.emFormat   != IMGALG_NV12)
	    		{
	    			free(ptBarrelCorrectionObject);
	    			ptBarrelCorrectionObject = NULL;
	    			return ERR_FORMAT_TYPE; 			
	    		}			
		}
		else
		{
			if (ptBarrelCorrectionOpen->tBC_VSOpen.emFormat != IMGALG_NV12)
			{
				free(ptBarrelCorrectionObject);
				ptBarrelCorrectionObject = NULL;
				return ERR_FORMAT_TYPE; 		
			}
		}
		#ifdef RUN_ON_QUALC
		retCode = (EMISPRetCode)BarrelCorrectionOpen(&tPlatformObject, ptBarrelCorrectionObject, ptBarrelCorrectionOpen);
		#endif		
		if (retCode != SUCCESS_GPUALG)
		{
			ISPprintLog("BC open error!\n");
			free(ptBarrelCorrectionObject);
			ptBarrelCorrectionObject = NULL;
			return retCode;
		}

		*pvHandle = ptBarrelCorrectionObject;
		
		return SUCCESS_GPUALG;
    }
    else
    {
		printf("ERR_ALGORITHM_TYPE!\n");
        return ERR_ALGORITHM_TYPE;
    }
}


/*=========================================================================
函 数 名： ISPVideoAlgProcess
功    能： 算法处理
算法实现： 无
参    数： pvHandle                    算法句柄[in]
		   emIMGAlgType                算法类型[in]
		   ptInputInfo                 输入数据结构体指针[in]
		   ptOutputInfo                输出数据结构体指针[out]
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgProcess(void *pvHandle, 
                                   EMISPAlgType emIMGAlgType,
                                   TISPImageInfo *pt_input_info,
                                   TISPImageInfo *pt_output_info)
{
	EMISPRetCode retCode = SUCCESS_GPUALG;
    if (pvHandle == NULL)
	{
		return ERR_PROCESS_KERNELS;
	}
    if (emIMGAlgType == BARRELCORRECTION)
	{
		#ifdef RUN_ON_QUALC	
		TBarrelCorrectionObject* ptBarrelCorrectionObject  = (TBarrelCorrectionObject* )pvHandle;
		retCode = (EMISPRetCode)BarrelCorrectionProcess(&tPlatformObject, ptBarrelCorrectionObject, pt_input_info, pt_output_info);
		#endif
		if (retCode  != SUCCESS_GPUALG)
		{
			return retCode;
		}

		return SUCCESS_GPUALG;	
	}
	else
	{
		printf("ERR_ALGORITHM_TYPE!\n");
		return ERR_ALGORITHM_TYPE;
	}

}


/*=========================================================================
函 数 名： ISPVideoAlgClose
功    能： 算法资源释放
算法实现： 无
参    数： pvHandle                    算法句柄[in]
           emIMGAlgType                算法类型[in]
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgClose(void *pvHandle,
                                 EMISPAlgType emIMGAlgType)
{
	if (pvHandle == NULL)
	{
		return SUCCESS_GPUALG;
	}
	if (emIMGAlgType == BARRELCORRECTION)
	{
		ISPprintLog("BarrelCorrectionClose start!\n");
		TBarrelCorrectionObject* ptBarrelCorrectionObject  = (TBarrelCorrectionObject* )pvHandle;
		#ifdef RUN_ON_QUALC
		BarrelCorrectionClose(&tPlatformObject, ptBarrelCorrectionObject);
		#endif		
		ISPprintLog("BarrelCorrectionClose end!\n");
		free(ptBarrelCorrectionObject);
		return SUCCESS_GPUALG;
	}
	else
	{
		// ISPprintALog("ERR_ALGORITHM_TYPE!\n");
		return ERR_ALGORITHM_TYPE;
	}	
}


/*=========================================================================
函 数 名： GetISPVersionInfo
功    能： 获取版本等信息
算法实现： 无
参    数： emIMGAlgType               算法类型[in]
          ptVersionInfo              版本信息结构体[out]
返 回 值： 无返回信息
===========================================================================*/
void GetISPVersionInfo(EMISPAlgType emIMGAlgType,
					TISPVersionInfo* ptVersionInfo)
{
 	if (emIMGAlgType == BARRELCORRECTION)
	{
		char version[200] = {};
		sprintf(version, "CPU_BC version 1.0 DATE %s TIME %s",__DATE__,__TIME__);
		memcpy(ptVersionInfo->u8AlgVersionInfo, version, sizeof(version));
	}
	else
	{		
		printf("ERR_ALGORITHM_TYPE!\n");
	}
	ISPprintLog("version is %s\n", ptVersionInfo->u8AlgVersionInfo);
}
