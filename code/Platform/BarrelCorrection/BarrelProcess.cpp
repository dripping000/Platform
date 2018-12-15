#include <opencv2/core.hpp>

unsigned char clip(int indata);

/*====================================================================
函数名      : BarrelCorrectProcess
功能        : 镜头畸变校正处理主体函数
参数        :  i_pImg: 输入rgb图像指针[in]
               i_nWidth: 原图像宽[in]
               i_nHeight: 原图像高[in]
               i_nNewWidth: 输出图像宽[in]
               i_nNewHeight: 输出图像高[in]
               i_f32Ratio[2]: 缩放系数数组[in]
               o_pImg: 输出rgb图像指针[in] 
返回值说明  : 成功: 0, 失败: -1
====================================================================*/
int BarrelCorrectProcess(unsigned char * i_pImg, int i_nWidth, int i_nHeight, int i_nNewWidth, int i_nNewHeight, int nHalfWidth, int nHalfHeight,float i_f32Ratio[2],unsigned char * o_pImg, CvRect * pRect, int ReCalcFlag, float scaledown, float scaledownH)
{
    float vary, vary1, varall; 

    int nI, nJ;
	float deltx,delty;

    int o1[3],o2[3];

    int nNewHalfW, nNewHalfH, nNewWidthStep;
    int nOrgJ, nOrgI, nWidthStep, nDiff, nWeight1,nWeight2;
    int nOrgJLow,nOrgJHigh, nOrgILow,nOrgIHigh;
    unsigned char* pLineL1,* pLineH1, *pOut;
	int nLeftBound, nRightBound, nUpBound, nDownBound;


    nNewHalfW = i_nNewWidth/2;
    nNewHalfH = i_nNewHeight/2;
    nWidthStep = i_nWidth*3;
    nNewWidthStep = i_nNewWidth*3;

	if(ReCalcFlag==1)
	{
		nLeftBound = 0;
		nRightBound = i_nNewWidth;
		nUpBound = 0;
		nDownBound = i_nNewHeight;
	}

    for(nI=0;nI<i_nNewHeight;nI++)
    {
        delty =(nI-nNewHalfH)/scaledown;
		vary = delty*delty;

        for (nJ=0;nJ<i_nNewWidth;nJ++)
        {
            deltx = (nJ-nNewHalfW)/scaledownH;
			vary1 = deltx*deltx+vary;
			varall = (1.0+ vary1*vary1 * i_f32Ratio[1] +vary1*i_f32Ratio[0]);
            

            nOrgJ = (deltx*varall+nHalfWidth)*1024;
            nOrgI = (delty*varall+nHalfHeight)*1024;
            nOrgJLow = nOrgJ/1024;
            nOrgJHigh = nOrgJLow+1;
            nOrgILow = nOrgI/1024;
            nOrgIHigh = nOrgILow+1;

            if(nOrgJLow>0 && nOrgJHigh < i_nWidth-1 && nOrgILow>0 && nOrgIHigh < i_nHeight-1)
            {


                nDiff=nOrgJ-(nOrgJLow<<10);
                nWeight1 = 1024-nDiff;
				nWeight2 = nDiff;

                pLineL1 = (unsigned char*)(i_pImg+nWidthStep*(nOrgILow) + nOrgJLow*3);
                pLineH1 = (unsigned char*)(i_pImg+nWidthStep*(nOrgILow) + nOrgJHigh*3);
                o1[0] = (pLineL1[0]*nWeight1+pLineH1[0]*nWeight2)/1024;
                o1[1] = (pLineL1[1]*nWeight1+pLineH1[1]*nWeight2)/1024;
                o1[2] = (pLineL1[2]*nWeight1+pLineH1[2]*nWeight2)/1024;

                pLineL1 = (unsigned char*)(i_pImg+nWidthStep*(nOrgIHigh) + nOrgJLow*3);
                pLineH1 = (unsigned char*)(i_pImg+nWidthStep*(nOrgIHigh) + nOrgJHigh*3);
                o2[0] = (pLineL1[0]*nWeight1+pLineH1[0]*nWeight2)/1024;
                o2[1] = (pLineL1[1]*nWeight1+pLineH1[1]*nWeight2)/1024;
                o2[2] = (pLineL1[2]*nWeight1+pLineH1[2]*nWeight2)/1024;


                pOut = (unsigned char*)(o_pImg+nNewWidthStep*nI + nJ*3);
                nDiff=nOrgI-nOrgILow*1024;
                nWeight1 = 1024-nDiff;
				nWeight2 = nDiff;
                pOut[0] = clip((o1[0]*nWeight1+o2[0]*nWeight2)/1024);
                pOut[1] = clip((o1[1]*nWeight1+o2[1]*nWeight2)/1024);
                pOut[2] = clip((o1[2]*nWeight1+o2[2]*nWeight2)/1024);
            }
			else if(ReCalcFlag==1)
			{
				if(nI ==nNewHalfH && nJ < nNewHalfW && nJ > nLeftBound)
					nLeftBound = nJ;
				else if(nI ==nNewHalfH && nJ > nNewHalfW && nJ < nRightBound)
					nRightBound = nJ;

				if(nJ == nNewHalfW && nI < nNewHalfH && nI > nUpBound)
					nUpBound = nI;
				else if(nJ == nNewHalfW && nI > nNewHalfH && nI < nDownBound)
					nDownBound = nI;
			}

        }
    }
	if(ReCalcFlag==1)
	{
		pRect->x = nLeftBound;
		pRect->y = nUpBound;
		pRect->width = nRightBound-nLeftBound;
		pRect->height = nDownBound - nUpBound;
	}
    return 0;
}