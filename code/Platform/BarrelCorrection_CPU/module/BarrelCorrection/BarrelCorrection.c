
#include <malloc.h>
#include "ISP_Alglog.h"
#include <stdlib.h>
#include "CPUBarrelCorrection.h"


int BarrelCorrectionOpen(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject,
							TBarrelCorrectionOpen* ptBarrelCorrectionOpen)
{
	int result = 0;
	if (result != 0)
	{
		ISPprintLog("InitDevice error!\n");
		return result;
	}

	ptBarrelCorrectionObject->flag = ptBarrelCorrectionOpen->flag;


	int inputwidth = ptBarrelCorrectionOpen->tBCOpen.u32InputWidth;
	int inputheight = ptBarrelCorrectionOpen->tBCOpen.u32InputHeight;
	int outputwidth = ptBarrelCorrectionOpen->tBCOpen.u32OutputWidth;
	int outputheight = ptBarrelCorrectionOpen->tBCOpen.u32OutputHeight;

	int inputwidth_vs = ptBarrelCorrectionOpen->tBC_VSOpen.u32InputWidth;
	int inputheight_vs = ptBarrelCorrectionOpen->tBC_VSOpen.u32InputHeight;
	int outputwidth_vs = ptBarrelCorrectionOpen->tBC_VSOpen.u32OutputWidth;
	int outputheight_vs = ptBarrelCorrectionOpen->tBC_VSOpen.u32OutputHeight;

    
    if (ptBarrelCorrectionObject->flag == 0)
    {
        if ((2048 == inputwidth) && (1536 == inputheight) && (2048 == outputwidth) && (1536 == outputheight))
        {
            /*result = BarrelCorrectOpenA(ptPlatformObject, ptBarrelCorrectionObject,
                &ptBarrelCorrectionObject->tBCObject, &ptBarrelCorrectionOpen->tBCOpen);*/
        }
        else if ((2048 == inputwidth) && (1536 == inputheight) && (1920 == outputwidth) && (1080 == outputheight))
        {
            result = BarrelCorrectOpenB(ptPlatformObject, ptBarrelCorrectionObject,
                &ptBarrelCorrectionObject->tBCObject, &ptBarrelCorrectionOpen->tBCOpen);
        }
        else if ((2048 == inputwidth) && (1536 == inputheight) && (1600 == outputwidth) && (1200 == outputheight))
        {
            /*result = BarrelCorrectOpenC(ptPlatformObject, ptBarrelCorrectionObject,
                &ptBarrelCorrectionObject->tBCObject, &ptBarrelCorrectionOpen->tBCOpen);*/
        }
        else
        {
            result = BarrelCorrectOpenB(ptPlatformObject, ptBarrelCorrectionObject,
                &ptBarrelCorrectionObject->tBCObject, &ptBarrelCorrectionOpen->tBCOpen);
            ISPprintLog("==============>BCOpen====>default!!!\n");
        }

		printf("==============>BCOpen====>inputwidth:%d,inputheight:%d,outputwidth:%d,outputheight:%d\n",inputwidth,inputheight,outputwidth,outputheight);

	}
	else
	{


		if( (2048 == inputwidth_vs)&&(1536==inputheight_vs)&&(2048 ==outputwidth_vs)&&(1536==outputheight_vs))
		{
		 	result = BC_VSOpenA(ptPlatformObject, ptBarrelCorrectionObject,
			&ptBarrelCorrectionObject->tBC_VSObject, &ptBarrelCorrectionOpen->tBC_VSOpen);
		}
		else if ( (2048 == inputwidth_vs)&&(1536==inputheight_vs)&&(1920 ==outputwidth_vs)&&(1080==outputheight_vs))
		{
			result = BC_VSOpenB(ptPlatformObject, ptBarrelCorrectionObject,
			&ptBarrelCorrectionObject->tBC_VSObject, &ptBarrelCorrectionOpen->tBC_VSOpen);
		}
		else
		{
			result = BC_VSOpen(ptPlatformObject, ptBarrelCorrectionObject,
			&ptBarrelCorrectionObject->tBC_VSObject, &ptBarrelCorrectionOpen->tBC_VSOpen);
			 ISPprintLog("==============>BC_VSOpen====>default!!!\n");
		}

		printf("==============>BC_VSOpen====>inputwidth_vs:%d,inputheight_vs:%d,outputwidth_vs:%d,outputheight_vs:%d\n",inputwidth_vs,inputheight_vs,outputwidth_vs,outputheight_vs);

	}

	return result;
}

int BarrelCorrectionProcess(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject,
							TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput)
{
	int result = 0;

	int inputwidth = ptBarrelCorrectionObject->tBCObject.srcWidth;
	int inputheight = ptBarrelCorrectionObject->tBCObject.srcHeight;
	int outputwidth = ptBarrelCorrectionObject->tBCObject.dstWidth;
	int outputheight = ptBarrelCorrectionObject->tBCObject.dstHeight;

	int inputwidth_vs = ptBarrelCorrectionObject->tBC_VSObject.srcWidth;
	int inputheight_vs = ptBarrelCorrectionObject->tBC_VSObject.srcHeight;
	int outputwidth_vs = ptBarrelCorrectionObject->tBC_VSObject.virtualWidthOut;
	int outputheight_vs = ptBarrelCorrectionObject->tBC_VSObject.virtualHeightOut;

	if (ptBarrelCorrectionObject->flag == 0)
	{

		if( (2048 == inputwidth)&&(1536==inputheight)&&(2048 ==outputwidth)&&(1536==outputheight))
		{
			result = BarrelCorrectProcessA(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject, ptBarrelInput, ptBarrelOutput);
		}
		else if ( (2048 == inputwidth)&&(1536==inputheight)&&(1920 ==outputwidth)&&(1080==outputheight))
		{
			result = BarrelCorrectProcessB(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject, ptBarrelInput, ptBarrelOutput);
		}
		else if ( (2048 == inputwidth)&&(1536==inputheight)&&(1600 ==outputwidth)&&(1200==outputheight))
		{
			result = BarrelCorrectProcessC(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject, ptBarrelInput, ptBarrelOutput);
		}
		else
		{
			result = BarrelCorrectProcess(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject, ptBarrelInput, ptBarrelOutput);
			ISPprintLog("==============>BCProcess====>default!!!\n");
		}
		ISPprintLog("==============>BCProcess====>inputwidth:%d,inputheight:%d,outputwidth:%d,outputheight:%d\n",inputwidth,inputheight,outputwidth,outputheight);


	}
	else
	{
		if( (2048 == inputwidth_vs)&&(1536==inputheight_vs)&&(2048 ==outputwidth_vs)&&(1536==outputheight_vs))
		{
			result = BC_VSProcessA(ptPlatformObject, &ptBarrelCorrectionObject->tBC_VSObject, ptBarrelInput, ptBarrelOutput);
		}
		else if ( (2048 == inputwidth_vs)&&(1536==inputheight_vs)&&(1920 ==outputwidth_vs)&&(1080==outputheight_vs))
		{
			result = BC_VSProcessB(ptPlatformObject, &ptBarrelCorrectionObject->tBC_VSObject, ptBarrelInput, ptBarrelOutput);
		}
		else
		{
			result = BC_VSProcess(ptPlatformObject, &ptBarrelCorrectionObject->tBC_VSObject, ptBarrelInput, ptBarrelOutput);
			ISPprintLog("==============>BC_VSProcess====>default!!!\n");

		}

		ISPprintLog("==============>BC_VSProcess====>inputwidth_vs:%d,inputheight_vs:%d,outputwidth_vs:%d,outputheight_vs:%d\n",inputwidth_vs,inputheight_vs,outputwidth_vs,outputheight_vs);

	}

	return result;
}

int BarrelCorrectionClose(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject)
{
	int result = 0;

	int inputwidth = ptBarrelCorrectionObject->tBCObject.srcWidth;
	int inputheight = ptBarrelCorrectionObject->tBCObject.srcHeight;
	int outputwidth = ptBarrelCorrectionObject->tBCObject.dstWidth;
	int outputheight = ptBarrelCorrectionObject->tBCObject.dstHeight;

	int inputwidth_vs = ptBarrelCorrectionObject->tBC_VSObject.srcWidth;
	int inputheight_vs = ptBarrelCorrectionObject->tBC_VSObject.srcHeight;
	int outputwidth_vs = ptBarrelCorrectionObject->tBC_VSObject.virtualWidthOut;
	int outputheight_vs = ptBarrelCorrectionObject->tBC_VSObject.virtualHeightOut;

	ISPprintLog("BarrelCorrectionClose start!\n");
	ISPprintLog("ptBarrelCorrectionObject->flag = %d!\n", ptBarrelCorrectionObject->flag );

	if (ptBarrelCorrectionObject->flag == 0)
	{

		if( (2048 == inputwidth)&&(1536==inputheight)&&(2048 ==outputwidth)&&(1536==outputheight))
		{
			result = BarrelCorrectCloseA(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject);
		}
		else if ( (2048 == inputwidth)&&(1536==inputheight)&&(1920 ==outputwidth)&&(1080==outputheight))
		{
			result = BarrelCorrectCloseB(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject);
		}
		else if ( (2048 == inputwidth)&&(1536==inputheight)&&(1600 ==outputwidth)&&(1200==outputheight))
		{
			result = BarrelCorrectCloseC(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject);
		}
		else
		{
			result = BarrelCorrectClose(ptPlatformObject, &ptBarrelCorrectionObject->tBCObject);
			ISPprintLog("==============>BCClose====>default!!!\n");
		}
		printf("==============>BCClose====>inputwidth:%d,inputheight:%d,outputwidth:%d,outputheight:%d\n",inputwidth,inputheight,outputwidth,outputheight);



	}
	else
	{
		if( (2048 == inputwidth_vs)&&(1536==inputheight_vs)&&(2048 ==outputwidth_vs)&&(1536==outputheight_vs))
		{
			result = BC_VSCloseA(ptPlatformObject, &ptBarrelCorrectionObject->tBC_VSObject);
		}
		else if ( (2048 == inputwidth_vs)&&(1536==inputheight_vs)&&(1920 ==outputwidth_vs)&&(1080==outputheight_vs))
		{
			result = BC_VSCloseB(ptPlatformObject, &ptBarrelCorrectionObject->tBC_VSObject);
		}
		else
		{
			result = BC_VSClose(ptPlatformObject, &ptBarrelCorrectionObject->tBC_VSObject);
			ISPprintLog("==============>BC_VSClose====>default!!!\n");
		}

		printf("==============>BC_VSClose====>inputwidth_vs:%d,inputheight_vs:%d,outputwidth_vs:%d,outputheight_vs:%d\n",inputwidth_vs,inputheight_vs,outputwidth_vs,outputheight_vs);


	}

	return result;
}
