#ifdef RUN_ON_QUALC
#define _GNU_SOURCE
#include <sched.h>
#endif
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
//#include <time.h>
#include <string.h>
#include "ISPVideoAlg.h"
#include "BC.h"

//#define BC_VS
//#define BC

//#define WRITETOFILE
//#define PICNUM 1

/*
long diff(struct timeval start, struct timeval end)
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
*/
void WriteOutput(TISPImageInfo* ptOutput, FILE* file)
{
	int k = 0;
	printf("u32Height = %d, u32PitchY = %d, u32Width = %d\n",ptOutput->tImageBuffer[0].u32Height,ptOutput->tImageBuffer[0].u32PitchY,ptOutput->tImageBuffer[0].u32Width);
	for (k = 0; k < ptOutput->tImageBuffer[0].u32Height; k++)
	{
		fwrite(ptOutput->tImageBuffer[0].pu8ImageDataY+k*ptOutput->tImageBuffer[0].u32PitchY, 1,ptOutput->tImageBuffer[0].u32Width,file);
	}

	for (k = 0; k < ptOutput->tImageBuffer[0].u32Height/2; k++)
	{
		fwrite(ptOutput->tImageBuffer[0].pu8ImageDataU+ k*ptOutput->tImageBuffer[0].u32PitchUV,1,ptOutput->tImageBuffer[0].u32Width,file);
	}
}


/////  ./CPUDemoBC vin_2048x1536_p2048.yuv out.yuv 2048 1536 1920 1080 0 1 1 4
/////  ./CPUDemoBC 2560x1440_nv12.yuv out.yuv 2560 1440 2560 1440 0 1 1 4
/////  ./CPUDemoBC 2560x1440_nv12.yuv out.yuv 2560 1440 1920 1080 1 1 1 4
/////  ./CPUDemoBC vin_2048x1536_p2048.yuv out.yuv 2048 1536 1920 1080 0 1 1 4
/////  ./CPUDemoBC 2560x1440_nv12.yuv out.yuv 2560    1440    2560  1440  0           1       1         4
/////  ./CPUDemoBC 2560x1440_nv12.yuv out.yuv input_w input_h out_w out_h bc-0/bcvs-1 pichnum writefile cpu
/*
int main(int argc, char** argv)
{
#ifdef RUN_ON_QUALC
	int cpuNum = 0;
	if(argc == 4){
		cpuNum = atoi(argv[10]);
		printf("cpuNum %d\n",cpuNum);
		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(cpuNum, &mask);
		if(sched_setaffinity(0, sizeof(mask), &mask) == -1){
			printf("sched_setaffinity\n");
		}
	}
#endif

	int picNum;
	int k;

	struct timeval time1, time2;
	struct timezone tzone;
	double total_time = 0;
	double total_time_single = 0;
	double total_time_single_max = 0;
	double total_time_single_min = 10000;

	FILE* inputFileOpen = NULL;
	FILE* outputFileOpen = NULL;
	void* algHandle = NULL;

	EMISPAlgType emGPUAlgType = BARRELCORRECTION;

	TISPVersionInfo tVersionInfo;
	void* ptOpen = NULL;

	int inputWidth = atoi(argv[3]);
	int inputHeight = atoi(argv[4]);
	int inputPitchY = inputWidth;
	int inputPitchUV = inputWidth;

	int outputWidth = atoi(argv[5]);
	int outputHeight = atoi(argv[6]);
	int outputPitchY = outputWidth;
	int outputPitchUV = outputWidth;

	char inputFilePath[200] = {};
	char outputFilePath[200] = {};
	char binaryPath[200] = {};
	strcpy(inputFilePath, argv[1]);
	strcpy(outputFilePath, argv[2]);

	unsigned char* pInputBuffer = (unsigned char*)malloc(inputPitchY*inputHeight*3/2);
	unsigned char *ptOutputBuffer = (unsigned char *)malloc(outputPitchY * outputHeight * 3 / 2);

	TBarrelCorrectionOpen tBarrelCOpen;
	TISPImageInfo tInputImageInfo;
	TISPImageInfo tOutputImageInfo;
	inputFileOpen = fopen(inputFilePath, "rb");
	outputFileOpen = fopen(outputFilePath, "wb");

	if (1 == atoi(argv[7]))
	{
		tBarrelCOpen.tBC_VSOpen.emFormat = IMGALG_NV12;
		tBarrelCOpen.tBC_VSOpen.u32InputWidth = inputWidth;
		tBarrelCOpen.tBC_VSOpen.u32InputHeight = inputHeight;
		tBarrelCOpen.tBC_VSOpen.u32InputPitchY = inputPitchY;
		tBarrelCOpen.tBC_VSOpen.u32InputPitchUV = inputPitchUV;
		tBarrelCOpen.tBC_VSOpen.u32OutputWidth = outputWidth;
		tBarrelCOpen.tBC_VSOpen.u32OutputHeight = outputHeight;
		strcpy(tBarrelCOpen.s8BinaryPath, binaryPath);
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.f32WidthScale = 2.5;//1.061947
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32AdjHeight = 248;
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32BarrelParam1 = 800;//1390
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32BarrelParam2 = 0;//140
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32MidHeight = 2064;//1656
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32MidWidth = 3264;//3584
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleCurvLevel = 400;
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleMultVal = 5;
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleZoomInRatio = 600;
		tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32WideRatio = 200;
		tBarrelCOpen.flag = 1;
		ptOpen = (void*)(&tBarrelCOpen);
	}
	else if (0 == atoi(argv[7]))
	{
		tBarrelCOpen.tBCOpen.emFormat = IMGALG_NV12;
		tBarrelCOpen.tBCOpen.u32InputWidth = inputWidth;
		tBarrelCOpen.tBCOpen.u32InputHeight = inputHeight;
		tBarrelCOpen.tBCOpen.u32InputPitchY = inputPitchY;
		tBarrelCOpen.tBCOpen.u32InputPitchUV = inputPitchUV;
		tBarrelCOpen.tBCOpen.u32OutputWidth = outputWidth;
		tBarrelCOpen.tBCOpen.u32OutputHeight = outputHeight;
		strcpy(tBarrelCOpen.s8BinaryPath, binaryPath);
		// tBarrelCOpen.tBCOpen.u32Ratio1 = 1420;
		// tBarrelCOpen.tBCOpen.u32Ratio2 = 130;
		tBarrelCOpen.tBCOpen.u32Ratio1 = 2650;
		tBarrelCOpen.tBCOpen.u32Ratio2 = 410;
		tBarrelCOpen.flag = 0;
		ptOpen = (void*)(&tBarrelCOpen);
	}

#ifdef BC_VS
	int inputWidth = 1920;//2560;
	int inputHeight = 1080;//1440;
	int inputPitchY = 1920;//2560;
	int inputPitchUV = 1920;//2560;
	int outputWidth = 1920;
	int outputHeight = 1080;
	int outputPitchY = 1920;
	int outputPitchUV = 1920;
	char inputFilePath[200] = {"/data/local/tmp/2560x1440_nv12.yuv"};
	char outputFilePath[200] = {"/data/local/tmp/out.yuv"};
	char binaryPath[200] = {"/data/local/tmp/BarrelCorrect.bin"};
	unsigned char* pInputBuffer = (unsigned char*)malloc(inputPitchY*inputHeight*3/2);
	unsigned char *ptOutputBuffer = (unsigned char *)malloc(outputPitchY * outputHeight * 3 / 2);
	TBarrelCorrectionOpen tBarrelCOpen;
	TISPImageInfo tInputImageInfo;
	TISPImageInfo tOutputImageInfo;
	inputFileOpen = fopen(inputFilePath, "rb");
	outputFileOpen = fopen(outputFilePath, "wb");
	tBarrelCOpen.tBC_VSOpen.emFormat = IMGALG_NV12;
	tBarrelCOpen.tBC_VSOpen.u32InputWidth = inputWidth;
	tBarrelCOpen.tBC_VSOpen.u32InputHeight = inputHeight;
	tBarrelCOpen.tBC_VSOpen.u32InputPitchY = inputPitchY;
	tBarrelCOpen.tBC_VSOpen.u32InputPitchUV = inputPitchUV;
	tBarrelCOpen.tBC_VSOpen.u32OutputWidth = outputWidth;
	tBarrelCOpen.tBC_VSOpen.u32OutputHeight = outputHeight;
	strcpy(tBarrelCOpen.s8BinaryPath, binaryPath);
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.f32WidthScale = 1.061947;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32AdjHeight = 248;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32BarrelParam1 = 1390;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32BarrelParam2 = 140;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32MidHeight = 1656;////////????????????????
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32MidWidth = 3584;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleCurvLevel = 400;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleMultVal = 5;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleZoomInRatio = 600;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32WideRatio = 200;
	tBarrelCOpen.flag = 1;
	ptOpen = (void*)(&tBarrelCOpen);
#endif
#ifdef BC

	int inputWidth = 1920;//2048;
	int inputHeight = 1080;//1536;
	int inputPitchY = 1920;//2048;
	int inputPitchUV = 1920;//2048;
	int outputWidth = 1920;
	int outputHeight = 1080;
	int outputPitchY = 1920;
	int outputPitchUV = 1920;
	//char inputFilePath[200] = {"/data/local/tmp/vin_2048x1536_p2048.yuv"};
	//char outputFilePath[200] = {"/data/local/tmp/out.yuv"};
	char inputFilePath[200] = {"./input_1920.yuv"};
	char outputFilePath[200] = {"./output_1920.yuv"};
	//char binaryPath[200] = {"/data/local/tmp/BarrelCorrect.bin"};
	unsigned char* pInputBuffer = (unsigned char*)malloc(inputPitchY*inputHeight*3/2);
	unsigned char *ptOutputBuffer = (unsigned char *)malloc(outputPitchY * outputHeight * 3 / 2);
	TBarrelCorrectionOpen tBarrelCOpen;
	TISPImageInfo tInputImageInfo;
	TISPImageInfo tOutputImageInfo;
	inputFileOpen = fopen(inputFilePath, "rb");
	outputFileOpen = fopen(outputFilePath, "wb");
	tBarrelCOpen.tBCOpen.emFormat = IMGALG_NV12;
	tBarrelCOpen.tBCOpen.u32InputWidth = inputWidth;
	tBarrelCOpen.tBCOpen.u32InputHeight = inputHeight;
	tBarrelCOpen.tBCOpen.u32InputPitchY = inputPitchY;
	tBarrelCOpen.tBCOpen.u32InputPitchUV = inputPitchUV;
	tBarrelCOpen.tBCOpen.u32OutputWidth = outputWidth;
	tBarrelCOpen.tBCOpen.u32OutputHeight = outputHeight;
	strcpy(tBarrelCOpen.s8BinaryPath, binaryPath);
	tBarrelCOpen.tBCOpen.u32Ratio1 = 1420;
	tBarrelCOpen.tBCOpen.u32Ratio2 = 130;
	tBarrelCOpen.flag = 0;
	ptOpen = (void*)(&tBarrelCOpen);
//#endif

	if (ISPVideoAlgInit() != SUCCESS_GPUALG){
		printf("init error!\n");
		fclose(inputFileOpen);
		fclose(outputFileOpen);
		free(pInputBuffer);
		return -1;
	}

	ISPGetVersionInfo(emGPUAlgType, &tVersionInfo);

	if (ISPVideoAlgOpen(&algHandle, emGPUAlgType, ptOpen) != SUCCESS_GPUALG){
		printf("open error!\n");
		ISPVideoAlgRelease();
		fclose(inputFileOpen);
		fclose(outputFileOpen);
		free(pInputBuffer);
		return -1;
	}

	tInputImageInfo.tImageBuffer[0].pu8ImageDataY= pInputBuffer;
	tInputImageInfo.tImageBuffer[0].pu8ImageDataU= pInputBuffer+inputPitchY*inputHeight;
	tInputImageInfo.tImageBuffer[0].emFormat = IMGALG_NV12;
	tInputImageInfo.tImageBuffer[0].u32Width = inputWidth;
	tInputImageInfo.tImageBuffer[0].u32Height = inputHeight;
	tInputImageInfo.tImageBuffer[0].u32PitchY = inputWidth;
	tInputImageInfo.tImageBuffer[0].u32PitchUV = inputWidth;
	tInputImageInfo.pvImageInfo = NULL;

	// output image info
	tOutputImageInfo.tImageBuffer[0].pu8ImageDataY = ptOutputBuffer;
	tOutputImageInfo.tImageBuffer[0].pu8ImageDataU = ptOutputBuffer + outputPitchY * outputHeight;
	tOutputImageInfo.tImageBuffer[0].u32Width = outputWidth;
	tOutputImageInfo.tImageBuffer[0].u32Height = outputHeight;
	tOutputImageInfo.tImageBuffer[0].u32PitchY = outputPitchY;
	tOutputImageInfo.tImageBuffer[0].u32PitchUV = outputPitchUV;

	picNum = atoi(argv[8]);
	for (k = 0; k < picNum; k++)
	{
		fread(pInputBuffer, 1, inputPitchY*inputHeight*3/2, inputFileOpen);
		gettimeofday(&time1, &tzone);
		if (ISPVideoAlgProcess(algHandle, emGPUAlgType, &tInputImageInfo, &tOutputImageInfo) != SUCCESS_GPUALG){
			printf("GPUVideoAlgProcess error!\n");
			fclose(inputFileOpen);
			fclose(outputFileOpen);
			free(pInputBuffer);
			ISPVideoAlgClose(algHandle, emGPUAlgType);
			ISPVideoAlgRelease();
			return -1;
		}

		gettimeofday(&time2, &tzone);
		total_time_single = diff(time1, time2) / 1000.0;
		total_time += diff(time1, time2) / 1000.0;
		printf(".....single time is %fms\n", total_time_single);

		if(total_time_single_max < total_time_single){
			total_time_single_max = total_time_single;
		}
		if(total_time_single_min > total_time_single){
			total_time_single_min = total_time_single;
		}

		if(1 == atoi(argv[9])){
			WriteOutput(&tOutputImageInfo, outputFileOpen);
		}

//		usleep(10*1000);
#ifdef WRITETOFILE
		WriteOutput(&tOutputImageInfo, outputFileOpen);
#endif
	}
	printf("process time is %fms\n", total_time / (double)picNum);
	printf("total_time_single_max time is %fms\n", total_time_single_max);
	printf("total_time_single_min time is %fms\n", total_time_single_min);
	ISPVideoAlgClose(algHandle, emGPUAlgType);
	fclose(inputFileOpen);
	fclose(outputFileOpen);
	free(pInputBuffer);
	ISPVideoAlgRelease();
	return 0;
}
*/


int main()
{
#ifdef RUN_ON_QUALC
	int cpuNum = 4;

	printf("cpuNum %d\n",cpuNum);
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpuNum, &mask);
	if(sched_setaffinity(0, sizeof(mask), &mask) == -1){
		printf("sched_setaffinity\n");
	}
#endif

int while_cnt =0;

for(;while_cnt<1;while_cnt++)
{
	printf("cnt %d\n", while_cnt);

	int picNum;
	int k;

	FILE* inputFileOpen = NULL;
	FILE* outputFileOpen = NULL;
	void* algHandle = NULL;

	EMISPAlgType emGPUAlgType = BARRELCORRECTION;

	TISPVersionInfo tVersionInfo;
	void* ptOpen = NULL;

	int inputWidth = 2048;
	int inputHeight = 1536;
	int inputPitchY = 2048;
	int inputPitchUV = 2048;
	int outputWidth = 1920;
	int outputHeight = 1080;
	int outputPitchY = 1920;
	int outputPitchUV = 1920;
	char inputFilePath[200] = {"./input_1920.yuv"};
	char outputFilePath[200] = {"./output_1920.yuv"};
//	char binaryPath[200] = {"/data/local/tmp/BarrelCorrect.bin"};
	unsigned char* pInputBuffer = (unsigned char*)malloc(inputPitchY*inputHeight*3/2);
	unsigned char *ptOutputBuffer = (unsigned char *)malloc(outputPitchY*outputHeight*3/2);

	memset(pInputBuffer,0,sizeof(unsigned char)*inputPitchY*inputHeight*3/2);
	memset(ptOutputBuffer,0,sizeof(unsigned char)*outputPitchY * outputHeight * 3 / 2);

	if( (NULL ==pInputBuffer)||(NULL==ptOutputBuffer) )
	{
		printf("malloc failed!!!\n");
	}
	TBarrelCorrectionOpen tBarrelCOpen;
	TISPImageInfo tInputImageInfo;
	TISPImageInfo tOutputImageInfo;
	inputFileOpen = fopen(inputFilePath, "rb");
	outputFileOpen = fopen(outputFilePath, "wb");
	if( (NULL ==inputFileOpen)||(NULL==outputFileOpen) )
	{
		printf("open failed!!!\n");
	}
	tBarrelCOpen.tBC_VSOpen.emFormat = IMGALG_NV12;
	tBarrelCOpen.tBC_VSOpen.u32InputWidth = inputWidth;
	tBarrelCOpen.tBC_VSOpen.u32InputHeight = inputHeight;
	tBarrelCOpen.tBC_VSOpen.u32InputPitchY = inputPitchY;
	tBarrelCOpen.tBC_VSOpen.u32InputPitchUV = inputPitchUV;
	tBarrelCOpen.tBC_VSOpen.u32OutputWidth = outputWidth;
	tBarrelCOpen.tBC_VSOpen.u32OutputHeight = outputHeight;
//	strcpy(tBarrelCOpen.s8BinaryPath, binaryPath);

/* BC_VS param
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.f32WidthScale = 1.061947;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32AdjHeight = 248;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32BarrelParam1 = 1390;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32BarrelParam2 = 140;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32MidHeight = 1656;////////????????????????
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32MidWidth = 3584;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleCurvLevel = 400;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleMultVal = 5;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32TeleZoomInRatio = 600;
	tBarrelCOpen.tBC_VSOpen.tBC_VSParam.s32WideRatio = 200;
	tBarrelCOpen.flag = 1;
*/
	tBarrelCOpen.tBCOpen.u32Ratio1 = 1420;
	tBarrelCOpen.tBCOpen.u32Ratio2 = 130;
	tBarrelCOpen.flag = 0;

	ptOpen = (void*)(&tBarrelCOpen);


	if (ISPVideoAlgInit() != SUCCESS_GPUALG){
		printf("init error!\n");
		fclose(inputFileOpen);
		fclose(outputFileOpen);
		free(pInputBuffer);
		return -1;
	}

	GetISPVersionInfo(emGPUAlgType, &tVersionInfo);

	printf("=========>begin to open!\n");
	if (ISPVideoAlgOpen(&algHandle, emGPUAlgType, ptOpen) != SUCCESS_GPUALG){
		printf("open error!\n");
		ISPVideoAlgRelease();
		fclose(inputFileOpen);
		fclose(outputFileOpen);
		free(pInputBuffer);
		return -1;
	}

	tInputImageInfo.tImageBuffer[0].pu8ImageDataY= pInputBuffer;
	tInputImageInfo.tImageBuffer[0].pu8ImageDataU= pInputBuffer+inputPitchY*inputHeight;
	tInputImageInfo.tImageBuffer[0].emFormat = IMGALG_NV12;
	tInputImageInfo.tImageBuffer[0].u32Width = inputWidth;
	tInputImageInfo.tImageBuffer[0].u32Height = inputHeight;
	tInputImageInfo.tImageBuffer[0].u32PitchY = inputWidth;
	tInputImageInfo.tImageBuffer[0].u32PitchUV = inputWidth;
	tInputImageInfo.pvImageInfo = NULL;

	// output image info
	tOutputImageInfo.tImageBuffer[0].pu8ImageDataY = ptOutputBuffer;
	tOutputImageInfo.tImageBuffer[0].pu8ImageDataU = ptOutputBuffer + outputPitchY * outputHeight;
	tOutputImageInfo.tImageBuffer[0].u32Width = outputWidth;
	tOutputImageInfo.tImageBuffer[0].u32Height = outputHeight;
	tOutputImageInfo.tImageBuffer[0].u32PitchY = outputPitchY;
	tOutputImageInfo.tImageBuffer[0].u32PitchUV = outputPitchUV;

	picNum = 1;
	for (k = 0; k < picNum; k++)
	{
		printf("=========>begin to fread!\n");
		fread(pInputBuffer, 1, inputPitchY*inputHeight*3/2, inputFileOpen);
		//gettimeofday(&time1, &tzone);
		printf("=========>begin to process!\n");
		if (ISPVideoAlgProcess(algHandle, emGPUAlgType, &tInputImageInfo, &tOutputImageInfo) != SUCCESS_GPUALG){
			printf("GPUVideoAlgProcess error!\n");
			fclose(inputFileOpen);
			fclose(outputFileOpen);
			free(pInputBuffer);
			ISPVideoAlgClose(algHandle, emGPUAlgType);
			ISPVideoAlgRelease();
			return -1;
		}

			WriteOutput(&tOutputImageInfo, outputFileOpen);

	//		usleep(10*1000);

	}

	printf("=========>begin to close!\n");
	ISPVideoAlgClose(algHandle, emGPUAlgType);
	fclose(inputFileOpen);
	fclose(outputFileOpen);
	free(pInputBuffer);
	free(ptOutputBuffer);
	ISPVideoAlgRelease();
    }
  return 0;
}


