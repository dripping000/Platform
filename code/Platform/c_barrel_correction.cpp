#include "c_barrel_correction.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

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
    this->m_nVirtualStitchFlag = 1;  

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

                fWratio = fHratio = fRatio;                
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
