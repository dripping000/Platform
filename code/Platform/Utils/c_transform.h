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
	cv::Mat Array2Mat(BYTE* pbyImageRgb, int nWeight, int nHeight, int nChannels);

    void ReadTxtFileToMat(const std::string strFilePath, cv::Mat& mat);
    void ReadMatToTxtFile(const cv::Mat& mat, std::string strFilePath);

    void ShuffleData(const cv::Mat& matFeature, const cv::Mat& matLabel, cv::Mat& matFeatureShuffled, cv::Mat& matLabelShuffled);

    void SplitData(cv::Mat& matData, cv::Mat& matDataTrain, cv::Mat& matDataTest, float fRate);

	//私有函数
	//私有变量
	cv::Mat c_matImage;

	//保护函数和变量
};

