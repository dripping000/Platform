#include "c_transform.h"



CTransform::CTransform()
{
}


CTransform::~CTransform()
{
}


int CTransform::Mat2Array(cv::Mat matImage, BYTE *pArray)
{
	if (matImage.empty() || pArray == nullptr)
	{
		return -1;
	}

	// 没有填充像素，把行设为1，列设为H*W，从而去除外层的循环	
	int nr = matImage.rows;
	int nc = matImage.cols * matImage.channels();
	if (matImage.isContinuous())
	{		
		nc = nr * nc;
		nr = 1;
	}

	BYTE* pbyImgeRgb = pArray;
	for (int i = 0; i < nr; i++)
	{
        //cout << "matImage.type(): " << matImage.type() << endl;
        // 输入图像类型是CV_32F
        if (CV_32F == matImage.type())
        {
            float *data = matImage.ptr<float>(i);

            for (int j = 0; j < nc; j++)
            {
                *pbyImgeRgb = (uchar)data[j];
                pbyImgeRgb++;
            }
        }
        // 输入图像类型是CV_8U
        else if (CV_8UC3 == matImage.type())
        {
            BYTE* data = matImage.ptr<uchar>(i);

            for (int j = 0; j < nc; j++)
            {
                *pbyImgeRgb = (uchar)data[j];
                pbyImgeRgb++;
            }
        }		
	}

	return 0;
}


cv::Mat CTransform::Array2Mat(BYTE* pbyImageArray, int nWeight, int nHeight, int nChannels)
{
	if (pbyImageArray == nullptr)
	{
		// DebugMK
		//return;
	}

	if (!c_matImage.empty())
	{
		c_matImage.release();
	}
	c_matImage = cv::Mat(nHeight, nWeight, CV_8UC3);

	int nr = nHeight;
	int nc = nWeight * nChannels;

	BYTE* pbyImgeRgb = pbyImageArray;
	for (int i = 0; i < nr; i++)
	{
		BYTE* data = c_matImage.ptr<uchar>(i);

		for (int j = 0; j < nc; j++)
		{
			data[j] = *pbyImgeRgb;
			pbyImgeRgb++;
		}
	}

	return c_matImage;
}
