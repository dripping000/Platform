#ifndef _ISP_VIDEO_ALG_H_
#define _ISP_VIDEO_ALG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************variables*************************************/
#define MAX_IMAGE_INFO 50
#define HASHSIZE 16
#define MAXHASH (HASHSIZE*HASHSIZE)
// 函数调用信息返回码
typedef enum tagISPRetCode
{
	SUCCESS_GPUALG = 0,
	ERR_ALGORITHM_TYPE,
	ERR_FORMAT_TYPE,
	ERR_INIT_DEVICE,
	ERR_CREATE_HOST_BUFFER,
	ERR_CREATE_GPU_BUFFER,
	ERR_CREATE_HANDLE,
	ERR_CREATE_KERNELS,
	ERR_MAP_IMAGE,
	ERR_UNMAP_IMAGE,
	ERR_SET_PARAMETERS,
	ERR_PROCESS_KERNELS,
	ERR_RELEASE_RESOURCE,
	ERR_ALGORITHM_INIT,
	ERR_BIN_OPEN,
}EMISPRetCode;

// 算法类型
typedef enum tagISPAlgType
{
	BARRELCORRECTION = 0,           // 畸变校正
}EMISPAlgType;

// 输入输出图像格式
typedef enum tagISPImageFormat
{             
	IMGALG_NV12=0,
	IMGALG_BGR,                   
}EMISPImageFormat;

// 版本等信息
typedef struct tagISPVersionInfo
{
	unsigned char u8AlgVersionInfo[200];      // 算法版本号
	unsigned char u8OpenclVersionInfo[200];   // OpenCL版本号
	unsigned char u8PlatformInfo[200];        // 平台信息
}TISPVersionInfo;

// 输入输出图像内存结构体
typedef struct tagISPImageBuffer
{
	unsigned char* pu8ImageDataY;     // 输入/输出图像Y分量指针
	unsigned char* pu8ImageDataU;     // 输入/输出图像U量指针，和图像格式相关，如果为NV12，则为UV分量指针
	unsigned char* pu8ImageDataV;     // 输入/输出图像V分量指针，和图像格式相关，如果为NV12，则无效
	unsigned int u32Width;            // 图像宽度
	unsigned int u32Height;           // 图像高度
	unsigned int u32PitchY;           // 输入/输出图像Y分量跨度
	unsigned int u32PitchUV;          // 输入/输出图像UV分量跨度
	unsigned int u32Flag;             // 输出图像有效标识，1为有效，0为无效
	int fd;                           // ION buffer id
	EMISPImageFormat emFormat;           // 图像格式
	
	void* pvbuffertag;				  //与该TISPImageBuffer对应tag（每个TISPImageBuffer都唯一对应，用于释放）
}TISPImageBuffer;

// 输入输出图像信息结构体
typedef struct tagISPImageInfo
{
	TISPImageBuffer tImageBuffer[MAX_IMAGE_INFO];     // 图像数据，可能有多个
	void* pvImageInfo;                             // 和图像数据对应的信息
	
	unsigned int u32FreeBufferNumber;			   // 待free的TISPImageBuffer数目
	void* pvFreeTags[MAX_IMAGE_INFO];			   // 待free的TISPImageBuffer对应的pvbuffertag组成的数组	
}TISPImageInfo;


/***************************functions************************************/
/*=========================================================================
函 数 名： ISPVideoAlgInit
功    能： 设备平台初始化
算法实现： 无
参    数： 
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgInit();

/*=========================================================================
函 数 名： ISPVideoAlgRelease
功    能： 设备平台释放
算法实现： 无
参    数： 
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgRelease();

/*=========================================================================
函 数 名： ISPVideoAlgOpen
功    能： 算法初始化
算法实现： 无
参    数： pvHandle                    算法句柄[in]
           emIMGAlgType                算法类型[in]
		   pvOpen                      初始化结构体指针[in]
返 回 值： 返回函数调用信息
===========================================================================*/
EMISPRetCode ISPVideoAlgOpen(void** pvHandle,
							 EMISPAlgType emIMGAlgType,
						     void* pvOpen);

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
EMISPRetCode ISPVideoAlgProcess(void* pvHandle,
								EMISPAlgType emIMGAlgType,
							    TISPImageInfo* ptInputInfo,
							    TISPImageInfo* ptOutputInfo);

/*=========================================================================
函 数 名： ISPVideoAlgClose
功    能： 算法资源释放
算法实现： 无
参    数： pvHandle                    算法句柄[in]
           emIMGAlgType                算法类型[in]
返 回 值： 返回函数调用信息
===========================================================================*/							
EMISPRetCode ISPVideoAlgClose(void* pvHandle,
                              EMISPAlgType emIMGAlgType);

/*=========================================================================
函 数 名： GetISPVersionInfo
功    能： 获取版本等信息
算法实现： 无
参    数： emIMGAlgType               算法类型[in]
           ptVersionInfo              版本信息结构体[out]
返 回 值： 无返回信息
===========================================================================*/
void GetISPVersionInfo(EMISPAlgType emIMGAlgType,
					TISPVersionInfo* ptVersionInfo);


#ifdef __cplusplus
}
#endif
#endif




