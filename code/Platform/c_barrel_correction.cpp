#include "c_barrel_correction.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// BarrelCorrection_CPU
#include "CPUBCCommon.h"
#include "CPUCommon.h"
#include "CPUBC.h"
#include "CPUBarrelCorrection.h"

//Debug
#include "debug_log.h"


CBarrelCorrection::CBarrelCorrection()
{
    // BarrelCorrection
    this->m_nBaseK0 = 1600;
    this->m_nBaseK1 = 160;

    this->m_fK0 = 300.0;
    this->m_fK1 = 0.0;

    this->m_nBarrelCorrectionFixedSizeFlag = 1;

    // VirtualStitch
    this->m_nVirtualStitchFlag = 0;  

    this->m_nZoom = 600;
    this->m_nBarrelUp = 8;
    this->m_nUpDownCurveRatio = 13;
    this->m_nUpDownRatio = 120;

    this->m_nFinalWidth = 1920;
    this->m_nFinalHeight = 1080;
}


CBarrelCorrection::~CBarrelCorrection()
{
}

void CBarrelCorrection::SetK0(float K0)
{
    this->m_fK0 = K0;
}

float CBarrelCorrection::GetK0()
{
    return this->m_fK0;
}

void CBarrelCorrection::SetK1(float K1)
{
    this->m_fK1 = K1;
}

float CBarrelCorrection::GetK1()
{
    return this->m_fK1;
}

int BarrelCorrectProcess(unsigned char * i_pImg, int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight, int nHalfWidth, int nHalfHeight, float i_f32Ratio[2], unsigned char * o_pImg, CvRect * pRect, int ReCalcFlag, float scaledown, float scaledownH);
cv::Mat CBarrelCorrection::BarrelCorrection(cv::Mat matSrcImage)
{
    static float s_afK[2] = { -1.0, -1.0 };
    float fK[2];
    float fWratio, fHratio, fRatio;

    fK[0] = (float)(-(this->m_fK0 + this->m_nBaseK0) / 10000000000.0);
    fK[1] = (float)((this->m_fK1 + this->m_nBaseK1) / 10000000000000000.0);
    fWratio = fHratio = fRatio = 1.0;

    if (this->m_fK0 == 0 && this->m_fK1 == 0)
    {
        matSrcImage.copyTo(this->m_matBarrelCorrectionImage);
        this->m_nBarrelCorrectionWidth = this->m_matBarrelCorrectionImage.cols;
        this->m_nBarrelCorrectionHeight = this->m_matBarrelCorrectionImage.rows;
    }
    else
    {
        int nImageHalfWidth = matSrcImage.cols / 2;
        int nImageHalfHeight = matSrcImage.rows / 2;

        int nBarrelCorrectionValuesChangedFlag = 0;
        if (s_afK[0] != fK[0] || s_afK[1] != fK[0])
        {
            nBarrelCorrectionValuesChangedFlag = 1;

            int x, y;
            float x2_add_y2;
            int nNewImageHalfWidth, nNewImageHalfHeight;

            if (this->m_nBarrelCorrectionFixedSizeFlag)
            {
                this->m_nBarrelCorrectionWidth = this->m_nFinalWidth;
                this->m_nBarrelCorrectionHeight = this->m_nFinalHeight;

                x = nImageHalfWidth;
                y = nImageHalfHeight;

                // DebugMK
                cout << "(1.0 + fK[0] * x * x + fK[1] * x * x * x * x):" << (1.0 + fK[0] * x * x + fK[1] * x * x * x * x) << endl;
                cout << "(1.0 + fK[0] * y * y + fK[1] * y * y * y * y):" << (1.0 + fK[0] * y * y + fK[1] * y * y * y * y) << endl;

                nNewImageHalfWidth = x / (1.0 + fK[0] * x * x + fK[1] * x * x * x * x);
                nNewImageHalfHeight = y / (1.0 + fK[0] * y * y + fK[1] * y * y * y * y);

                fWratio = (float(this->m_nBarrelCorrectionWidth)) / (float(nNewImageHalfWidth * 2));
                fHratio = (float(this->m_nBarrelCorrectionHeight)) / (float(nNewImageHalfHeight * 2));

                // 选择需要需要缩放的最小倍率，另一个方向则裁剪（ratio越接近1，缩放倍率越小）
                if (fWratio < fHratio)
                {
                    fRatio = fHratio;
                }
                else
                {
                    fRatio = fWratio;
                }

                // DebugMK
                cout << "fK[0]:" << fK[0] << ", fK[1]:" << fK[1] << endl;
                cout << "fWratio:" << fWratio << ", fHratio:" << fHratio << ", fRatio:" << fRatio << endl;

                fWratio = fHratio = fRatio = 1;                
            }
            else
            {
                x = nImageHalfWidth;
                y = nImageHalfHeight;
                x2_add_y2 = x * x + y * y;

                // DebugMK
                cout << "(1.0 + fK[0] * x2_add_y2 + fK[1] * x2_add_y2 * x2_add_y2):" << (1.0 + fK[0] * x2_add_y2 + fK[1] * x2_add_y2 * x2_add_y2) << endl;

                nNewImageHalfWidth = fRatio * (x / (1.0 + fK[0] * x2_add_y2 + fK[1] * x2_add_y2 * x2_add_y2));
                nNewImageHalfHeight = fRatio * (y / (1.0 + fK[0] * x2_add_y2 + fK[1] * x2_add_y2 * x2_add_y2));

                this->m_nBarrelCorrectionWidth = nNewImageHalfWidth * 2;
                this->m_nBarrelCorrectionHeight = nNewImageHalfHeight * 2;
                
                fWratio = fHratio = fRatio;

                // DebugMK
                cout << "fK[0]:" << fK[0] << ", fK[1]:" << fK[1] << endl;
                cout << "fWratio:" << fWratio << ", fHratio:" << fHratio << ", fRatio:" << fRatio << endl;

            }

            // 更新变化值
            s_afK[0] = fK[0];
            s_afK[1] = fK[1];            
        }        

        // Execute BarrelCorrection algorithm
        if (nBarrelCorrectionValuesChangedFlag)
        {
            BYTE *pbySrcImage = new BYTE[matSrcImage.cols * matSrcImage.rows * 3];
            BYTE *pbyBarrelCorrectionImage = new BYTE[this->m_nBarrelCorrectionWidth * this->m_nBarrelCorrectionHeight * 3];

            this->m_cTransform.Mat2Array(matSrcImage, pbySrcImage);

            CvRect cvRect;
            BarrelCorrectProcess(pbySrcImage, matSrcImage.cols, matSrcImage.rows, this->m_nBarrelCorrectionWidth, this->m_nBarrelCorrectionHeight,
                nImageHalfWidth, nImageHalfHeight, fK, pbyBarrelCorrectionImage, &cvRect, nBarrelCorrectionValuesChangedFlag, fWratio, fHratio);

            this->m_matBarrelCorrectionImage = this->m_cTransform.Array2Mat(pbyBarrelCorrectionImage, this->m_nBarrelCorrectionWidth, this->m_nBarrelCorrectionHeight, 3);

            delete[] pbySrcImage;
            pbySrcImage = nullptr;
            delete[] pbyBarrelCorrectionImage;
            pbyBarrelCorrectionImage = nullptr;

            // DebugMK
            cout << "this->m_nBarrelCorrectionWidth:" << this->m_nBarrelCorrectionWidth << ", this->m_nBarrelCorrectionHeight:" << this->m_nBarrelCorrectionHeight << endl;
            cout << "cvRect.width:" << cvRect.width << ", cvRect.height:" << cvRect.height << endl;

            this->m_matBarrelCorrectionImage = this->m_matBarrelCorrectionImage(cvRect);
            this->m_nBarrelCorrectionWidth = this->m_matBarrelCorrectionImage.cols;
            this->m_nBarrelCorrectionHeight = this->m_matBarrelCorrectionImage.rows;

            nBarrelCorrectionValuesChangedFlag = 0;
        }
    }

    if (this->m_nVirtualStitchFlag)
    {
        return VirtualStitch(this->m_matBarrelCorrectionImage, fK);
    }

    return this->m_matBarrelCorrectionImage;
}

int VirtualStitchInit(int width, int height, float zoom, int endHeight, int startHeight, int *pOutWidth, int *pOutHeight, int level, int upMult, float max_ratio, int finalHeight);
int VirtualStitchProcess(unsigned char * i_pImg, int width, int height, unsigned char * o_pImg, int i_nOutWidth, int i_nOutHeight, CvRect * pRect, int ReCalcFlag, float i_f32Ratio[2], int i_nOrgWidth, int i_nOrgHeight);
int VirtualStitchRelease();
cv::Mat CBarrelCorrection::VirtualStitch(cv::Mat matSrcImage, float fK[])
{
    //m_nZoom = 600;
    //m_nBarrelUp = 8;
    //m_nUpDownCurveRatio = 13;
    //m_nUpDownRatio = 120;

    int nAdjustStartHeight, nAdjustEndHeight;
    nAdjustStartHeight = matSrcImage.rows / 4;
    nAdjustEndHeight = matSrcImage.rows - 1;

    float fZoomRatio;
    fZoomRatio = 1.0 + this->m_nZoom / 1000.0;

    float max_ratio = 0.9;

    static float s_fZoomRatio = -1.0;
    static int s_nAdjustStartHeight = 0, s_nAdjustEndHeight = 0, s_nBarrelUp = 0, s_nUpDownCurveRatio = 0, s_nUpDownRatio = 0;

    int nVirtualStitchValuesChangedFlag = 0;
    if (s_fZoomRatio != fZoomRatio || s_nAdjustStartHeight != nAdjustStartHeight || s_nAdjustEndHeight != nAdjustEndHeight 
        || s_nBarrelUp != this->m_nBarrelUp || s_nUpDownCurveRatio != this->m_nUpDownCurveRatio || s_nUpDownRatio != this->m_nUpDownRatio)
    {
        nVirtualStitchValuesChangedFlag = 1;

        max_ratio = (1000.0 - this->m_nUpDownRatio) / 1000.0;

        VirtualStitchInit(matSrcImage.cols, matSrcImage.rows, fZoomRatio, nAdjustEndHeight, nAdjustStartHeight, &(this->m_nVirtualStitchWidth), &(this->m_nVirtualStitchHeight), 
            this->m_nBarrelUp, this->m_nUpDownCurveRatio, max_ratio, this->m_nFinalHeight);

        // DebugMK
        cout << "this->m_nVirtualStitchWidth:" << this->m_nVirtualStitchWidth << ", this->m_nVirtualStitchHeight:" << this->m_nVirtualStitchHeight << ", this->m_nFinalHeight:" << this->m_nFinalHeight << endl;

        s_fZoomRatio = fZoomRatio;
        s_nAdjustStartHeight = nAdjustStartHeight;
        s_nAdjustEndHeight = nAdjustEndHeight;
        s_nBarrelUp = this->m_nBarrelUp;
        s_nUpDownCurveRatio = this->m_nUpDownCurveRatio;
        s_nUpDownRatio = this->m_nUpDownRatio;
    }

    // Execute VirtualStitch algorithm
    if (nVirtualStitchValuesChangedFlag)
    {
        BYTE *pbySrcImage = new BYTE[matSrcImage.cols * matSrcImage.rows * 3];
        BYTE *pbyVirtualStitchImage = new BYTE[this->m_nVirtualStitchWidth * this->m_nVirtualStitchHeight * 3];

        this->m_cTransform.Mat2Array(matSrcImage, pbySrcImage);

        CvRect cvRect;
        VirtualStitchProcess(pbySrcImage, matSrcImage.cols, matSrcImage.rows, pbyVirtualStitchImage, this->m_nVirtualStitchWidth, this->m_nVirtualStitchHeight, &cvRect, nVirtualStitchValuesChangedFlag, fK, this->m_nFinalWidth, this->m_nFinalHeight);

        this->m_matVirtualStitchImage = this->m_cTransform.Array2Mat(pbyVirtualStitchImage, this->m_nVirtualStitchWidth, this->m_nVirtualStitchHeight, 3);

        delete[] pbySrcImage;
        pbySrcImage = nullptr;
        delete[] pbyVirtualStitchImage;
        pbyVirtualStitchImage = nullptr;

        this->m_matVirtualStitchImage = this->m_matVirtualStitchImage(cvRect);

        VirtualStitchRelease();

        nVirtualStitchValuesChangedFlag = 0;
    }

    return this->m_matVirtualStitchImage;
}


cv::Mat CBarrelCorrection::BarrelCorrection_CPU(cv::Mat& matSrcImage)
{
    int SrcWidth = matSrcImage.cols;
    int SrcHeight = matSrcImage.rows;

    unsigned char* pYUVBuf = NULL;
    pYUVBuf = (unsigned char*)malloc(SrcWidth * SrcHeight * 3 / 2 * sizeof(unsigned char));

    unsigned char* pYBuf = NULL;
    unsigned char* pUVBuf = NULL;
    pYBuf = (unsigned char*)malloc(SrcWidth * SrcHeight * sizeof(unsigned char));
    pUVBuf = (unsigned char*)malloc(SrcWidth * SrcHeight / 2 * sizeof(unsigned char));

    cv::Mat yuvImg;
    cvtColor(matSrcImage, yuvImg, CV_BGR2YUV_I420);
    memcpy(pYUVBuf, yuvImg.data, SrcWidth * SrcHeight * 3 / 2 * sizeof(unsigned char));

    // YUV--->NV12
    unsigned char* pYUVBufTmp = NULL;
    unsigned char* pYBufTmp = NULL;
    unsigned char* pUVBufTmp = NULL;

    pYUVBufTmp = pYUVBuf;
    pYBufTmp = pYBuf;
    pUVBufTmp = pUVBuf;

    for (int i = 0; i < SrcWidth * SrcHeight * 5 / 4; i++)
    {
        if (i < SrcWidth * SrcHeight)
        {
            *pYBufTmp = *pYUVBufTmp;
            pYBufTmp++;
            pYUVBufTmp++;
        }
        else
        {
            *pUVBufTmp = *pYUVBufTmp;
            pUVBufTmp++;
            *pUVBufTmp = *(pYUVBufTmp + SrcWidth * SrcHeight / 4);
            pUVBufTmp++;

            pYUVBufTmp++;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    int ResultWidth = 1920;
    int ResultHeight = 1080;    

    unsigned char* pYBufResult = NULL;
    unsigned char* pUVBufResult = NULL;
    pYBufResult = (unsigned char*)malloc(ResultWidth * ResultHeight * sizeof(unsigned char));
    pUVBufResult = (unsigned char*)malloc(ResultWidth * ResultHeight / 2 * sizeof(unsigned char));

    unsigned char* pYUVBufResult = NULL;
    pYUVBufResult = (unsigned char*)malloc(ResultWidth * ResultHeight * 3 / 2 * sizeof(unsigned char));

    // 参数初始化
    int ret;

    TPlatformObject tPlatformObject;
    TBarrelCorrectionObject tBarrelCorrectionObject;
    TBarrelCorrectionOpen tBarrelCorrectionOpen;

    tBarrelCorrectionOpen.flag = 0;   // 只进行畸变校正
    tBarrelCorrectionOpen.tBCOpen.u32InputWidth = SrcWidth;
    tBarrelCorrectionOpen.tBCOpen.u32InputHeight = SrcHeight;
    tBarrelCorrectionOpen.tBCOpen.u32OutputWidth = ResultWidth;
    tBarrelCorrectionOpen.tBCOpen.u32OutputHeight = ResultHeight;

    tBarrelCorrectionOpen.tBC_VSOpen.u32InputWidth = SrcWidth;
    tBarrelCorrectionOpen.tBC_VSOpen.u32InputHeight = SrcHeight;
    tBarrelCorrectionOpen.tBC_VSOpen.u32OutputWidth = ResultWidth;
    tBarrelCorrectionOpen.tBC_VSOpen.u32OutputHeight = ResultHeight;

    tBarrelCorrectionOpen.tBCOpen.flagROIMode = 0;
    tBarrelCorrectionOpen.tBCOpen.f32w = 0.8;
    tBarrelCorrectionOpen.tBCOpen.f32h = 0.6;
    tBarrelCorrectionOpen.tBCOpen.f32x0 = 0;
    tBarrelCorrectionOpen.tBCOpen.f32y0 = 0;

    tBarrelCorrectionOpen.tBCOpen.u32InputPitchY = 0; //
    tBarrelCorrectionOpen.tBCOpen.u32InputPitchUV = 0; // 

    tBarrelCorrectionOpen.tBCOpen.u32Ratio1 = 500; //
    tBarrelCorrectionOpen.tBCOpen.u32Ratio2 = 0; // 

    TISPImageInfo tISPImageInfoInput;
    TISPImageInfo tISPImageInfoOutput;

    tISPImageInfoInput.tImageBuffer[0].pu8ImageDataY = pYBuf;
    tISPImageInfoInput.tImageBuffer[0].pu8ImageDataU = pUVBuf;

    tISPImageInfoOutput.tImageBuffer[0].pu8ImageDataY = pYBufResult;
    tISPImageInfoOutput.tImageBuffer[0].pu8ImageDataU = pUVBufResult;


    ret = BarrelCorrectionOpen(&tPlatformObject, &tBarrelCorrectionObject, &tBarrelCorrectionOpen);

    ret = BarrelCorrectionProcess(&tPlatformObject, &tBarrelCorrectionObject,
        &tISPImageInfoInput, &tISPImageInfoOutput);

    ret = BarrelCorrectionClose(&tPlatformObject, &tBarrelCorrectionObject);

    // NV12--->YUV
    pYUVBufTmp = pYUVBufResult;
    pYBufTmp = tISPImageInfoOutput.tImageBuffer[0].pu8ImageDataY;
    pUVBufTmp = tISPImageInfoOutput.tImageBuffer[0].pu8ImageDataU;

    for (int i = 0; i < ResultWidth * ResultHeight * 5 / 4; i++)
    {
        if (i < ResultWidth * ResultHeight)
        {
            *pYUVBufTmp = *pYBufTmp;
            pYBufTmp++;
            pYUVBufTmp++;
        }
        else
        {
            *pYUVBufTmp = *pUVBufTmp;
            pUVBufTmp++;
            *(pYUVBufTmp + ResultWidth * ResultHeight / 4) = *pUVBufTmp;
            pUVBufTmp++;

            pYUVBufTmp++;
        }
    }

    cv::Mat yuvImgResult;
    yuvImgResult.create(ResultHeight * 3 / 2, ResultWidth, CV_8UC1);
    memcpy(yuvImgResult.data, pYUVBufResult, ResultWidth * ResultHeight * 3 / 2 * sizeof(unsigned char));

    cv::Mat rgbImgResult;
    cv::cvtColor(yuvImgResult, rgbImgResult, CV_YUV2BGR_I420);

    free(pYUVBuf);
    free(pYBuf);
    free(pUVBuf);
    pYUVBuf = NULL;
    pYBuf = NULL;
    pUVBuf = NULL;

    free(pYUVBufResult);
    free(pYBufResult);
    free(pUVBufResult);
    pYUVBufResult = NULL;
    pYBufResult = NULL;
    pUVBufResult = NULL;

    return rgbImgResult;    
}
