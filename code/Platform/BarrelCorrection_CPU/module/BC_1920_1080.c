#define NULL 0


#define _GNU_SOURCE
//#include <sched.h>
//#include <stdio.h>
//#include <malloc.h>
//#include <stdlib.h>
//#include <math.h>
//#include <pthread.h>
//#include <string.h>
//#include <arm_neon.h>
//#include <sys/prctl.h>

#include "CPUBC.h"
#include "ISP_Alglog.h"
/////////////////////////////////////////////////////////////////////////////////////////
#define UUVSETBL12	//define to run in 12 threads. if not defined, run in 8 threads

#define CROP16_9
/////////////////////////////////////////////////////////////////////////////////////////
#define CLIP(x) ((x)<0?0:(x)>255?255:(x))


#define RESOLUTION_1920_1080

#ifdef RESOLUTION_2560_1440
#define PITCHY 2560
#define PITCHYADD1 2561
#endif	
#ifdef RESOLUTION_2688_1512
#define PITCHY 2688
#define PITCHYADD1 2689
#endif	
#ifdef RESOLUTION_1920_1080
#define PITCHY 1920
#define PITCHYADD1 1921
#endif	
#ifdef RESOLUTION_2048_1536
#define PITCHY 2048
#define PITCHYADD1 2049
#endif


#define WIDTH 1920
#define HEIGHT 1080

unsigned int* BCcoordYYB = NULL;
unsigned int* BCcoordYYFXB = NULL;
unsigned int* BCcoordYYFYB = NULL;
unsigned char* BCcoordYYFXYCHARB = NULL;
unsigned int* BCcoordUUVB = NULL;
unsigned char* BCYDataB = NULL;
unsigned char* BCUVDataB = NULL;
unsigned char* pSrcYBCB = NULL;
unsigned char* pSrcUBCB = NULL;

float* coordY_m1B = NULL;
float* coordUV_m1B = NULL;
float* coordY_m2B = NULL;
float* coordUV_m2B = NULL;

static int g1B_exit_flag = 1;
static int g2B_exit_flag = 1;
static int g3B_exit_flag = 1;
static int g4B_exit_flag = 1;
static int g5B_exit_flag = 1;
static int g6B_exit_flag = 1;
static int g7B_exit_flag = 1;
static int g8B_exit_flag = 1;
static int g9B_exit_flag = 1;
static int g10B_exit_flag = 1;
static int g11B_exit_flag = 1;

int BCDataThread1BBool = 0;
int BCDataThread2BBool = 0;
int BCDataThread3BBool = 0;
int BCDataThread4BBool = 0;
int BCDataThread5BBool = 0;
int BCDataThread6BBool = 0;
int BCDataThread7BBool = 0;
int BCDataThread8BBool = 0;
int BCDataThread9BBool = 0;
int BCDataThread10BBool = 0;
int BCDataThread11BBool = 0;

int BCDataThread1BOUTBool = 0;
int BCDataThread2BOUTBool = 0;
int BCDataThread3BOUTBool = 0;
int BCDataThread4BOUTBool = 0;
int BCDataThread5BOUTBool = 0;
int BCDataThread6BOUTBool = 0;
int BCDataThread7BOUTBool = 0;
int BCDataThread8BOUTBool = 0;
int BCDataThread9BOUTBool = 0;
int BCDataThread10BOUTBool = 0;
int BCDataThread11BOUTBool = 0;


void bc_convertB()
{
    int j;
    unsigned short *uv1;

    for (j = 0; j < 1920 * 1080; j++)
    {
#if 0
        uint8x8x4_t	fxy = vld4_u8(BCcoordYYFXYCHARB + j * 4);

        uint8x8_t x0y0;
        uint8x8_t x1y0;
        uint8x8_t x0y1;
        uint8x8_t x1y1;

        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j]], x0y0, 0);
        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 1]], x0y0, 1);
        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 2]], x0y0, 2);
        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 3]], x0y0, 3);
        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 4]], x0y0, 4);
        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 5]], x0y0, 5);
        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 6]], x0y0, 6);
        x0y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 7]], x0y0, 7);

        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j] + 1], x1y0, 0);
        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 1] + 1], x1y0, 1);
        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 2] + 1], x1y0, 2);
        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 3] + 1], x1y0, 3);
        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 4] + 1], x1y0, 4);
        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 5] + 1], x1y0, 5);
        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 6] + 1], x1y0, 6);
        x1y0 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 7] + 1], x1y0, 7);

        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j] + PITCHY], x0y1, 0);
        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 1] + PITCHY], x0y1, 1);
        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 2] + PITCHY], x0y1, 2);
        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 3] + PITCHY], x0y1, 3);
        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 4] + PITCHY], x0y1, 4);
        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 5] + PITCHY], x0y1, 5);
        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 6] + PITCHY], x0y1, 6);
        x0y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 7] + PITCHY], x0y1, 7);

        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j] + PITCHYADD1], x1y1, 0);
        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 1] + PITCHYADD1], x1y1, 1);
        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 2] + PITCHYADD1], x1y1, 2);
        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 3] + PITCHYADD1], x1y1, 3);
        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 4] + PITCHYADD1], x1y1, 4);
        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 5] + PITCHYADD1], x1y1, 5);
        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 6] + PITCHYADD1], x1y1, 6);
        x1y1 = vset_lane_u8(pSrcYBCB[BCcoordYYB[j + 7] + PITCHYADD1], x1y1, 7);

        uint16x8_t temp;
        temp = vmull_u8(x0y0, fxy.val[0]);
        temp = vmlal_u8(temp, x1y0, fxy.val[1]);
        temp = vmlal_u8(temp, x0y1, fxy.val[2]);
        temp = vmlal_u8(temp, x1y1, fxy.val[3]);
        uint8x8_t result = vshrn_n_u16(temp, 8);
        vst1_u8(BCYDataB + j, result);
#endif
        unsigned char fxy[4];
        fxy[0] = *(BCcoordYYFXYCHARB + j * 4);
        fxy[1] = *(BCcoordYYFXYCHARB + j * 4 + 1);
        fxy[2] = *(BCcoordYYFXYCHARB + j * 4 + 2);
        fxy[3] = *(BCcoordYYFXYCHARB + j * 4 + 3);

        unsigned char x0y0;
        unsigned char x1y0;
        unsigned char x0y1;
        unsigned char x1y1;

        x0y0 = (pSrcYBCB[BCcoordYYB[j]]);
        x1y0 = (pSrcYBCB[BCcoordYYB[j] + 1]);
        x0y1 = (pSrcYBCB[BCcoordYYB[j] + PITCHY]);
        x1y1 = (pSrcYBCB[BCcoordYYB[j] + PITCHYADD1]);    

        unsigned int temp;
        temp = fxy[0] * x0y0 + fxy[1] * x1y0 + fxy[2] * x0y1 + fxy[3] * x1y1;
        //unsigned char result = temp / 16;
        unsigned char result = temp >> 8;
        *(BCYDataB + j) = result;
    }

    for (j = 0; j < 1920 * 1080 / 4; j++)
    {
        uv1 = (unsigned short *)&pSrcUBCB[BCcoordUUVB[j]];
        (BCUVDataB[j << 1]) = ((*uv1) & 0xff);
        BCUVDataB[(j << 1) + 1] = ((*uv1 >> 8) & 0xff);
    }
}

int BarrelCorrectInitB(int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight,
    float i_f32Ratio1, float i_f32Ratio2,
    float* coordY, float* coordUV, int flagROIMode, float f32x0, float f32y0, float f32w, float f32h)
{
    float vary, vary1, varall;

    int nI, nJ;

    float deltx, delty;

    float scaleDownV = 1.0f;
    float scaleDownH = 1.0f;

    int nNewHalfW, nNewHalfH, nNewWidthStep;

    int nOrgJ, nOrgI, nHalfWidth, nHalfHeight, nWidthStep, ndiffB;
    int nWeight1j, nWeight2j, nWeight1i, nWeight2i;

    int nOrgJLow, nOrgJHigh, nOrgILow, nOrgIHigh;

    int nLeftBound, nRightBound, nUpBound, nDownBound;
    float Hratio, Vratio;
    int nBHalfW, nBHalfH;


    int x0, y0;
    int ROIWidth, ROIHeight;
    float w;
    float pX, pY;
    float fOrgJ, fOrgI, fDiff;
    int pXLow, pXHigh, pYLow, pYHigh;
    float u, v;
    int cropWidth_half, cropHeight_half;
    int ZoomWidth, ZoomHeight;
    int nI_min, nJ_min;
    int nI_max, nJ_max;
    int array_width, array_height;

    coordY_m1B = (float*)malloc(i_nWidth*i_nHeight*sizeof(float) * 2);
    coordUV_m1B = (float*)malloc(i_nWidth*i_nHeight*sizeof(float) * 2 / 4);

    coordY_m2B = (float*)malloc(i_nWidth*i_nHeight*sizeof(float) * 2);
    coordUV_m2B = (float*)malloc(i_nWidth*i_nHeight*sizeof(float) * 2 / 4);

    if ((coordY_m1B == NULL) || (coordUV_m1B == NULL) || (coordY_m2B == NULL) || (coordUV_m2B == NULL))
    {
        return ERR_CREATE_HOST_BUFFER;
    }

    nHalfWidth = i_nWidth / 2;
    nHalfHeight = i_nHeight / 2;
    nNewHalfW = i_nNewWidth / 2;
    nNewHalfH = i_nNewHeight / 2;
    nWidthStep = i_nWidth;
    nNewWidthStep = i_nNewWidth;

    vary = nHalfWidth*nHalfWidth;
    nBHalfW = (nHalfWidth / (1.0 + i_f32Ratio2*(vary*vary) - vary*i_f32Ratio1));
    vary = nHalfHeight*nHalfHeight;
    nBHalfH = (nHalfHeight / (1.0 + i_f32Ratio2*(vary*vary) - vary*i_f32Ratio1));

    // Hratio = (float)(i_nWidth)/((float)((nBHalfW/4)*8));
    // Vratio = (float)(i_nHeight)/((float)(nBHalfH*2));
    Hratio = ((float)(i_nNewWidth)) / ((float)((nBHalfW / 4) * 8));
    Vratio = ((float)(i_nNewHeight)) / ((float)(nBHalfH * 2));
    ISPprintLog("i_nNewWidth:%d, i_nNewHeight:%d\n", i_nNewWidth, i_nNewHeight);
    ISPprintLog("i_nWidth:%d, nBHalfW:%d\n", i_nWidth, nBHalfW);
    ISPprintLog("i_nHeight:%d, nBHalfH:%d\n", i_nHeight, nBHalfH);
    ISPprintLog("Hratio:%f, Vratio:%f\n", Hratio, Vratio);

#ifndef CROP16_9
    if (Hratio < Vratio)
    {
        scaleDownV = Vratio;
        scaleDownH = Vratio;
    }
    else
    {
        scaleDownV = Hratio;
        scaleDownH = Hratio;
    }
    ISPprintLog(" NOT DEFINE CROP16_9 scaleDownV:%f, scaleDownH:%f\n", scaleDownV, scaleDownH);

#else
    scaleDownV = Vratio;
    scaleDownH = Hratio;
    ISPprintLog("DEFINE CROP16_9 scaleDownV:%f, scaleDownH:%f\n", scaleDownV, scaleDownH);

#endif

    /*
    flagROIMode = 1;
    f32w = 0.8;
    f32h = 0.6;
    f32x0 = 0;
    f32y0 = 0;
    */

    if (((f32x0 + f32w) > 1) || ((f32y0 + f32h) > 1))
    {
        ISPprintLog("roi region set wrong!\n");
        return ERR_ALGORITHM_TYPE;
    }

    ISPprintLog("INPUT PARAM........\n");
    ISPprintLog("w0  is %f\n", f32w);
    ISPprintLog("h0  is %f\n", f32h);
    ISPprintLog("x0  is %f\n", f32x0);
    ISPprintLog("y0  is %f\n", f32y0);

    if (0 == flagROIMode)
    {
        ISPprintLog("choose not to zoom!\n");

        for (nI = 0; nI < i_nNewHeight; nI++)
        {
            delty = (nI - nNewHalfH) / scaleDownV;
            vary = delty*delty;

            for (nJ = 0; nJ < i_nNewWidth; nJ++)
            {
                deltx = (nJ - nNewHalfW) / scaleDownH;
                vary1 = deltx*deltx + vary;
                varall = (1.0 + vary1*vary1 * i_f32Ratio2 - vary1*i_f32Ratio1);

                //nOrgJ = (deltx*varall+nHalfWidth)*1024;
                //nOrgI = (delty*varall+nHalfHeight)*1024;

                fOrgJ = (deltx*varall + nHalfWidth);
                fOrgI = (delty*varall + nHalfHeight);

                nOrgJLow = fOrgJ;
                nOrgILow = fOrgI;

                //nOrgJLow = nOrgJ/1024;
                nOrgJHigh = nOrgJLow + 1;
                //nOrgILow = nOrgI/1024;
                nOrgIHigh = nOrgILow + 1;

                if (nOrgJLow > 0 && nOrgJHigh < i_nWidth - 1 && nOrgILow>0 && nOrgIHigh < i_nHeight - 1)
                {
                    //ndiffB=nOrgJ-(nOrgJLow<<10);
                    //coordY[(nI*i_nNewWidth+nJ)*2+0] = ((float)nOrgJLow + (float)ndiffB/(float)1024);
                    //ndiffB=nOrgI-nOrgILow*1024;
                    //coordY[(nI*i_nNewWidth+nJ)*2+1] = ((float)nOrgILow + (float)ndiffB/(float)1024);

                    coordY[(nI*i_nNewWidth + nJ) * 2 + 0] = fOrgJ;
                    coordY[(nI*i_nNewWidth + nJ) * 2 + 1] = fOrgI;
                    /*
                                    if((960 == nI)&&(640 == nJ))
                                    {
                                        printf("==============>coordY[(nI*i_nNewWidth+nJ)*2+0]:%f\n",fOrgJ);
                                        printf("==============>coordY[(nI*i_nNewWidth+nJ)*2+1]:%f\n",fOrgI);

                                    }
                    */

                    if (nJ % 2 == 0 && nI % 2 == 0)
                    {
                        coordUV[((nI / 2)*nNewHalfW + (nJ / 2)) * 2 + 0] = coordY[(nI*i_nNewWidth + nJ) * 2 + 0];
                        coordUV[((nI / 2)*nNewHalfW + (nJ / 2)) * 2 + 1] = coordY[(nI*i_nNewWidth + nJ) * 2 + 1];
                    }
                }
            }
        }

    }
    else if (1 == flagROIMode) //1  //zoom1 : need to crop, crop height, then zoom
    {
        ROIWidth = f32w * i_nNewWidth;
        ROIHeight = f32h* i_nNewHeight;
        ZoomWidth = ROIHeight *((float)i_nNewWidth / (float)i_nNewHeight);
        ZoomHeight = ROIWidth* ((float)i_nNewHeight / (float)i_nNewWidth);   // adjust

        if (ROIHeight >= ZoomHeight)
        {
            w = f32w;
            cropHeight_half = (f32h * i_nNewHeight - ZoomHeight) / 2;
            cropWidth_half = 0;
            nI_min = cropHeight_half;
            nJ_min = 0;
            nI_max = ZoomHeight + cropHeight_half;
            nJ_max = ROIWidth;
            array_width = ROIWidth;
            array_height = ZoomHeight;
        }
        else
        {
            w = f32h;
            cropWidth_half = (f32w * i_nNewWidth - ZoomWidth) / 2;
            cropHeight_half = 0;
            nI_min = 0;
            nJ_min = cropWidth_half;
            nI_max = ROIHeight;
            nJ_max = ZoomWidth + cropWidth_half;
            array_width = ROIWidth - cropWidth_half * 2;
            array_height = ROIHeight;
        }
        x0 = f32x0 * i_nNewWidth;
        y0 = f32y0 * i_nNewHeight;

        ISPprintLog("choose to zoom in ROIMode 1!\n");

        ISPprintLog(" ROIWidth:%d,ROIHeight:%d,cropWidth_half:%d,cropHeight_half:%d,W is %f\n", ROIWidth, ROIHeight, cropWidth_half, cropHeight_half, w);
        ISPprintLog(" x0:%d,y0:%d\n", x0, y0);
        ISPprintLog(" nI_min:%d,nJ_min:%d,nI_max:%d,nJ_max:%d\n", nI_min, nJ_min, nI_max, nJ_max);

        for (nI = nI_min; nI < nI_max; nI++)
        {
            delty = (nI - nNewHalfH + y0) / scaleDownV;
            vary = delty*delty;

            for (nJ = nJ_min; nJ < nJ_max; nJ++)
            {
                deltx = (nJ - nNewHalfW + x0) / scaleDownH;
                vary1 = deltx*deltx + vary;
                varall = (1.0 + vary1*vary1 * i_f32Ratio2 - vary1*i_f32Ratio1);

                fOrgJ = (deltx*varall + nHalfWidth);
                fOrgI = (delty*varall + nHalfHeight);
                nOrgJLow = fOrgJ;
                nOrgJHigh = nOrgJLow + 1;
                nOrgILow = fOrgI;
                nOrgIHigh = nOrgILow + 1;

                if (nOrgJLow >= 0 && nOrgJHigh <= i_nWidth - 1 && nOrgILow >= 0 && nOrgIHigh <= i_nHeight - 1)
                {
                    coordY_m1B[((nI - nI_min)*array_width + nJ - nJ_min) * 2 + 0] = fOrgJ;
                    coordY_m1B[((nI - nI_min)*array_width + nJ - nJ_min) * 2 + 1] = fOrgI;
                }
            }
        }

        for (nI = 0; nI < i_nNewHeight; nI++)
        {
            for (nJ = 0; nJ < i_nNewWidth; nJ++)
            {
                pX = w*(float)nJ;
                pY = w*(float)nI;
                pXLow = pX;
                pXHigh = pX + 1;
                pYLow = pY;
                pYHigh = pY + 1;
                u = pX - pXLow;
                v = pY - pYLow;

                if (pXHigh >= array_width)
                {
                    pXHigh = array_width - 1;
                }
                if (pYHigh >= array_height)
                {
                    pYHigh = array_height - 1;
                }

                coordY[(nI*i_nNewWidth + nJ) * 2 + 0] = (1 - u)*(1 - v)*coordY_m1B[(pYLow*array_width + pXLow) * 2 + 0] + (1 - u)*v*coordY_m1B[(pYHigh*array_width + pXLow) * 2 + 0] + u*(1 - v)*coordY_m1B[(pYLow*array_width + pXHigh) * 2 + 0] + u*v*coordY_m1B[(pYHigh*array_width + pXHigh) * 2 + 0];
                coordY[(nI*i_nNewWidth + nJ) * 2 + 1] = (1 - u)*(1 - v)*coordY_m1B[(pYLow*array_width + pXLow) * 2 + 1] + (1 - u)*v*coordY_m1B[(pYHigh*array_width + pXLow) * 2 + 1] + u*(1 - v)*coordY_m1B[(pYLow*array_width + pXHigh) * 2 + 1] + u*v*coordY_m1B[(pYHigh*array_width + pXHigh) * 2 + 1];

                if (nJ % 2 == 0 && nI % 2 == 0)
                {
                    coordUV[((nI / 2)*nNewHalfW + (nJ / 2)) * 2 + 0] = coordY[(nI*i_nNewWidth + nJ) * 2 + 0];
                    coordUV[((nI / 2)*nNewHalfW + (nJ / 2)) * 2 + 1] = coordY[(nI*i_nNewWidth + nJ) * 2 + 1];
                }
            }
        }

    }

    else if (2 == flagROIMode)  //zoom2: just zoom
    {
        x0 = f32x0 * i_nNewWidth;
        y0 = f32y0 * i_nNewHeight;
        ROIWidth = f32w * i_nNewWidth;
        ROIHeight = f32h * i_nNewHeight;

        ISPprintLog("choose to zoom in ROIMode 2 !\n");

        ISPprintLog(" ROIWidth:%d,i_nNewWidth:%d,W is %f\n", ROIWidth, i_nNewWidth, w);

        for (nI = 0; nI < ROIHeight; nI++)
        {
            delty = (nI - nNewHalfH + y0) / scaleDownV;
            vary = delty*delty;

            for (nJ = 0; nJ < ROIWidth; nJ++)
            {
                deltx = (nJ - nNewHalfW + x0) / scaleDownH;
                vary1 = deltx*deltx + vary;
                varall = (1.0 + vary1*vary1 * i_f32Ratio2 - vary1*i_f32Ratio1);

                fOrgJ = (deltx*varall + nHalfWidth);
                fOrgI = (delty*varall + nHalfHeight);
                nOrgJLow = fOrgJ;
                nOrgJHigh = nOrgJLow + 1;
                nOrgILow = fOrgI;
                nOrgIHigh = nOrgILow + 1;

                if (nOrgJLow >= 0 && nOrgJHigh <= i_nWidth - 1 && nOrgILow >= 0 && nOrgIHigh <= i_nHeight - 1)
                {
                    coordY_m2B[(nI*ROIWidth + nJ) * 2 + 0] = fOrgJ;
                    coordY_m2B[(nI*ROIWidth + nJ) * 2 + 1] = fOrgI;
                }
            }
        }

        for (nI = 0; nI < i_nNewHeight; nI++)
        {
            for (nJ = 0; nJ < i_nNewWidth; nJ++)
            {
                pX = f32w*(float)nJ;
                pY = f32h*(float)nI;
                pXLow = pX;
                pXHigh = pX + 1;
                pYLow = pY;
                pYHigh = pY + 1;
                u = pX - (float)pXLow;
                v = pY - (float)pYLow;

                if (pXHigh >= ROIWidth)
                {
                    pXHigh = ROIWidth - 1;
                }
                if (pYHigh >= ROIHeight)
                {
                    pYHigh = ROIHeight - 1;
                }

                coordY[(nI*i_nNewWidth + nJ) * 2 + 0] = (1 - u)*(1 - v)*coordY_m2B[(pYLow*ROIWidth + pXLow) * 2 + 0] + (1 - u)*v*coordY_m2B[(pYHigh*ROIWidth + pXLow) * 2 + 0] + u*(1 - v)*coordY_m2B[(pYLow*ROIWidth + pXHigh) * 2 + 0] + u*v*coordY_m2B[(pYHigh*ROIWidth + pXHigh) * 2 + 0];
                coordY[(nI*i_nNewWidth + nJ) * 2 + 1] = (1 - u)*(1 - v)*coordY_m2B[(pYLow*ROIWidth + pXLow) * 2 + 1] + (1 - u)*v*coordY_m2B[(pYHigh*ROIWidth + pXLow) * 2 + 1] + u*(1 - v)*coordY_m2B[(pYLow*ROIWidth + pXHigh) * 2 + 1] + u*v*coordY_m2B[(pYHigh*ROIWidth + pXHigh) * 2 + 1];

                if (nJ % 2 == 0 && nI % 2 == 0)
                {
                    coordUV[((nI / 2)*nNewHalfW + (nJ / 2)) * 2 + 0] = coordY[(nI*i_nNewWidth + nJ) * 2 + 0];
                    coordUV[((nI / 2)*nNewHalfW + (nJ / 2)) * 2 + 1] = coordY[(nI*i_nNewWidth + nJ) * 2 + 1];
                }
            }

        }


    }
    else
    {
        ISPprintLog("roi_flag not support!\n");
        return ERR_ALGORITHM_TYPE;
    }

    return 0;
}

int BarrelCorrectOpenB(TPlatformObject* ptPlatformObject, TBarrelCorrectionObject* ptBarrelCorrectionObject, TBCObject* ptBCObject, TBCOpen* ptBCOpen)
{
    int width = ptBCOpen->u32InputWidth;
    int height = ptBCOpen->u32InputHeight;

    int newWidth = ptBCOpen->u32OutputWidth;
    int newHeight = ptBCOpen->u32OutputHeight;

    int ratio1;
    int ratio2;
    float f32Ratio1;
    float f32Ratio2;
    float* coordY = NULL;
    float* coordUV = NULL;

    int flagROIMode = ptBCOpen->flagROIMode;
    float f32x0 = ptBCOpen->f32x0;
    float f32y0 = ptBCOpen->f32y0;
    float f32w = ptBCOpen->f32w;
    float f32h = ptBCOpen->f32h;

    coordY = (float*)malloc(newWidth*newHeight*sizeof(float) * 2);
    if (coordY == NULL)
    {
        return ERR_CREATE_HOST_BUFFER;
    }
    coordUV = (float*)malloc(newWidth*newHeight*sizeof(float) * 2 / 4);
    if (coordUV == NULL)
    {
        return ERR_CREATE_HOST_BUFFER;
    }

    ptBCObject->srcWidth = width;
    ptBCObject->srcHeight = height;
    ptBCObject->srcPitchY = ptBCOpen->u32InputPitchY;
    ptBCObject->srcPitchUV = ptBCOpen->u32InputPitchUV;
    ptBCObject->dstWidth = newWidth;
    ptBCObject->dstHeight = newHeight;
    ptBCObject->pCoordY = coordY;
    ptBCObject->pCoordUV = coordUV;

    ratio1 = ptBCOpen->u32Ratio1;
    ratio2 = ptBCOpen->u32Ratio2;

    ISPprintLog("ratio1 = %d\n", ratio1);
    ISPprintLog("ratio2 = %d\n", ratio2);

    printf("ratio1 = %d\n", ratio1);
    printf("ratio2 = %d\n", ratio2);

    f32Ratio1 = (float)(ratio1) / 10000000000.0;
    f32Ratio2 = (float)(ratio2) / 10000000000000000.0;

    BarrelCorrectInitB(width, height, newWidth, newHeight, f32Ratio1, f32Ratio2, coordY, coordUV, flagROIMode, f32x0, f32y0, f32w, f32h);
    ISPprintLog("BarrelCorrectInitC succeed!\n");

    BCcoordYYB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
    BCcoordYYFXB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
    BCcoordYYFYB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int));
    BCcoordYYFXYCHARB = (unsigned char*)malloc(newWidth*newHeight * 4 * sizeof(unsigned char));
    BCcoordUUVB = (unsigned int*)malloc(newWidth*newHeight*sizeof(unsigned int) / 4);


    if ((NULL == BCcoordYYB) || (NULL == BCcoordYYFXB) || (NULL == BCcoordYYFYB) || (NULL == BCcoordYYFXYCHARB) || (NULL == BCcoordUUVB))
    {
        return ERR_CREATE_HOST_BUFFER;
    }

    //trans 2dim to 1dim
    int a, i, j, x, y;
    for (j = 0; j < newHeight; j++)
    {
        a = j*newWidth;
        for (i = 0; i < newWidth; i++)
        {
            x = coordY[(a + i) * 2];
            y = coordY[(a + i) * 2 + 1];

            BCcoordYYFXB[(a + i)] = (coordY[(a + i) * 2] - x) * 16;
            BCcoordYYFYB[(a + i)] = (coordY[(a + i) * 2 + 1] - y) * 16;
            BCcoordYYB[(a + i)] = y*width + x;

            BCcoordYYFXYCHARB[4 * (a + i)] = CLIP((16 - BCcoordYYFXB[(a + i)])*(16 - BCcoordYYFYB[(a + i)]));
            BCcoordYYFXYCHARB[4 * (a + i) + 1] = BCcoordYYFXB[(a + i)] * (16 - BCcoordYYFYB[(a + i)]);
            BCcoordYYFXYCHARB[4 * (a + i) + 2] = (16 - BCcoordYYFXB[(a + i)])*BCcoordYYFYB[(a + i)];
            BCcoordYYFXYCHARB[4 * (a + i) + 3] = BCcoordYYFXB[(a + i)] * BCcoordYYFYB[(a + i)];
        }
    }
    for (j = 0; j < newHeight / 2; j++)
    {
        a = j*newWidth / 2;
        for (i = 0; i < newWidth / 2; i++)
        {
            x = coordUV[(a + i) * 2];
            y = coordUV[(a + i) * 2 + 1];
            BCcoordUUVB[a + i] = y / 2 * width + x / 2 * 2;
        }
    }

#ifdef CPUCOMBINE
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(5, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
    {
        ISPprintLog("sched_setaffinity\n");
    }
#endif

    g1B_exit_flag = 0;
    g2B_exit_flag = 0;
    g3B_exit_flag = 0;
    g4B_exit_flag = 0;
    g5B_exit_flag = 0;
    g6B_exit_flag = 0;
    g7B_exit_flag = 0;

#ifdef UUVSETBL12
    g8B_exit_flag = 0;
    g9B_exit_flag = 0;
    g10B_exit_flag = 0;
    g11B_exit_flag = 0;
#endif

    return SUCCESS_GPUALG;
}

int BarrelCorrectProcessB(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject,
    TISPImageInfo* ptBarrelInput, TISPImageInfo* ptBarrelOutput)
{
    double total_time = 0;
    double total_time_single = 0;
    double total_time_single_max = 0;
    double total_time_single_min = 10000;

    pSrcYBCB = ptBarrelInput->tImageBuffer[0].pu8ImageDataY;
    pSrcUBCB = ptBarrelInput->tImageBuffer[0].pu8ImageDataU;

    BCYDataB = ptBarrelOutput->tImageBuffer[0].pu8ImageDataY;
    BCUVDataB = ptBarrelOutput->tImageBuffer[0].pu8ImageDataU;


    BCDataThread1BOUTBool = 0;
    BCDataThread2BOUTBool = 0;
    BCDataThread3BOUTBool = 0;
    BCDataThread4BOUTBool = 0;
    BCDataThread5BOUTBool = 0;
    BCDataThread6BOUTBool = 0;
    BCDataThread7BOUTBool = 0;
    BCDataThread1BBool = 1;
    BCDataThread2BBool = 1;
    BCDataThread3BBool = 1;
    BCDataThread4BBool = 1;
    BCDataThread5BBool = 1;
    BCDataThread6BBool = 1;
    BCDataThread7BBool = 1;


#ifdef UUVSETBL12
    BCDataThread8BOUTBool = 0;
    BCDataThread9BOUTBool = 0;
    BCDataThread10BOUTBool = 0;
    BCDataThread11BOUTBool = 0;

    BCDataThread8BBool = 1;
    BCDataThread9BBool = 1;
    BCDataThread10BBool = 1;
    BCDataThread11BBool = 1;
#endif

    ISPprintLog("BarrelCorrectProcessB\n");

#if 0  
    bc_convert0B();

    ISPprintLog("[[[[[MAIN ALL start]]]]]\n");
    pthread_mutex_lock(&mutexBC_mainB);

#ifdef UUVSETBL12
    while ((BCDataThread1BOUTBool == 0) || (BCDataThread2BOUTBool == 0) || (BCDataThread3BOUTBool == 0) || (BCDataThread4BOUTBool == 0) || (BCDataThread5BOUTBool == 0) || (BCDataThread6BOUTBool == 0)
        || (BCDataThread7BOUTBool == 0) || (BCDataThread8BOUTBool == 0) || (BCDataThread9BOUTBool == 0) || (BCDataThread10BOUTBool == 0) || (BCDataThread11BOUTBool == 0))
#else
    while ((BCDataThread1BOUTBool == 0) || (BCDataThread2BOUTBool == 0) || (BCDataThread3BOUTBool == 0) || (BCDataThread4BOUTBool == 0) || (BCDataThread5BOUTBool == 0) || (BCDataThread6BOUTBool == 0) || (BCDataThread7BOUTBool == 0))
#endif
    {
        pthread_cond_wait(&BCCond_mainB, &mutexBC_mainB);
    }
    pthread_mutex_unlock(&mutexBC_mainB);
    ISPprintLog("[[[[[MAIN ALL END]]]]]\n");
#endif

    bc_convertB();

    ptBarrelOutput->tImageBuffer[0].u32Width = ptBCObject->dstWidth;
    ptBarrelOutput->tImageBuffer[0].u32Height = ptBCObject->dstHeight;
    ptBarrelOutput->tImageBuffer[0].u32PitchY = ptBCObject->dstWidth;
    ptBarrelOutput->tImageBuffer[0].u32PitchUV = ptBCObject->dstWidth;
    //ptBarrelOutput->tImageBuffer[0].u32PitchY= 1664;
    //ptBarrelOutput->tImageBuffer[0].u32PitchUV= 1664;

    ISPprintLog("======B=======>single time is %fms\n", total_time_single);

    return SUCCESS_GPUALG;
}

int BarrelCorrectCloseB(TPlatformObject* ptPlatformObject, TBCObject* ptBCObject)
{
    int ret = 0;

    g1B_exit_flag = 1;
    g2B_exit_flag = 1;
    g3B_exit_flag = 1;
    g4B_exit_flag = 1;
    g5B_exit_flag = 1;
    g6B_exit_flag = 1;
    g7B_exit_flag = 1;
    g8B_exit_flag = 1;
    g9B_exit_flag = 1;
    g10B_exit_flag = 1;
    g11B_exit_flag = 1;

    BCDataThread1BOUTBool = 0;
    BCDataThread2BOUTBool = 0;
    BCDataThread3BOUTBool = 0;
    BCDataThread4BOUTBool = 0;
    BCDataThread5BOUTBool = 0;
    BCDataThread6BOUTBool = 0;
    BCDataThread7BOUTBool = 0;
    BCDataThread1BBool = 1;
    BCDataThread2BBool = 1;
    BCDataThread3BBool = 1;
    BCDataThread4BBool = 1;
    BCDataThread5BBool = 1;
    BCDataThread6BBool = 1;
    BCDataThread7BBool = 1;

#ifdef UUVSETBL12
    BCDataThread8BOUTBool = 0;
    BCDataThread9BOUTBool = 0;
    BCDataThread10BOUTBool = 0;
    BCDataThread11BOUTBool = 0;

    BCDataThread8BBool = 1;
    BCDataThread9BBool = 1;
    BCDataThread10BBool = 1;
    BCDataThread11BBool = 1;
#endif




#ifdef UUVSETBL12

#endif

    free(BCcoordYYB);
    free(BCcoordYYFXB);
    free(BCcoordYYFYB);
    free(BCcoordYYFXYCHARB);
    free(BCcoordUUVB);
    free(ptBCObject->pCoordY);
    free(ptBCObject->pCoordUV);
    free(coordY_m1B);
    free(coordUV_m1B);
    free(coordY_m2B);
    free(coordUV_m2B);

    BCcoordYYB = NULL;
    BCcoordYYFXB = NULL;
    BCcoordYYFYB = NULL;
    BCcoordYYFXYCHARB = NULL;
    BCcoordUUVB = NULL;
    ptBCObject->pCoordY = NULL;
    ptBCObject->pCoordUV = NULL;
    coordY_m1B = NULL;
    coordUV_m1B = NULL;
    coordY_m2B = NULL;
    coordUV_m2B = NULL;

    return 0;
}
