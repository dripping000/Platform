#pragma once

#include <opencv2/core.hpp>

#include "c_transform.h"

class CBarrelCorrection
{
public:
    CBarrelCorrection();
    ~CBarrelCorrection();

private:
    // BarrelCorrection
    float m_fK0;
    float m_fK1;

    int m_nBarrelCorrectionFixedSizeFlag;    

    // VirtualStitch
    int m_nVirtualStitchFlag;
    int m_nZoom;
    int m_nBarrelUp;
    int m_nUpDownCurveRatio;
    int m_nUpDownRatio;   

    int m_nFinalWidth;
    int m_nFinalHeight;
    

public:
    void SetK0(float K0);
    float GetK0();

    void SetK1(float K1);
    float GetK1();

    cv::Mat BarrelCorrection(cv::Mat matSrcImage);
    cv::Mat VirtualStitch(cv::Mat matSrcImage, float fK[]);


private:
    CTransform m_cTransform;

    int m_nBaseK0;
    int m_nBaseK1;

    cv::Mat m_matBarrelCorrectionImage;
    int m_nBarrelCorrectionWidth;
    int m_nBarrelCorrectionHeight;

    cv::Mat m_matVirtualStitchImage;
    int m_nVirtualStitchWidth;
    int m_nVirtualStitchHeight;

};

