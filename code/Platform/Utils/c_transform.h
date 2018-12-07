#pragma once
#include <opencv2/core.hpp>

typedef unsigned char BYTE;

class CTransform
{
public:
	CTransform();
    ~CTransform();

	//公有变量
	//公有函数
    int Mat2Array(cv::Mat matImage, BYTE *pArray);
	cv::Mat Array2Mat(BYTE* pbyImageRgb, int nHeight, int nWeight, int nChannels);
	//私有函数
	//私有变量
	cv::Mat c_matImage;

	//保护函数和变量
};

