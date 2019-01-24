#include "c_transform.h"

#include <iostream>
#include <fstream>


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


void CTransform::ReadTxtFileToMat(const std::string strFilePath, cv::Mat& mat)
{
    std::ifstream infile;
    infile.open(strFilePath, std::ios::in);
    if (!infile.is_open())
    {
        std::cerr << "Open " << strFilePath << " failed!" << std::endl;
        return;
    }

    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            infile >> mat.at<float>(i, j);
        }
    }

    infile.close();
}


void CTransform::ReadMatToTxtFile(const cv::Mat& mat, std::string strFilePath)
{
    std::ofstream outfile;
    outfile.open(strFilePath, std::ios::out);
    if (!outfile.is_open())
    {
        std::cerr << "Open " << strFilePath << " failed!" << std::endl;
        return;
    }

    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            outfile << mat.at<float>(i, j) << " ";
        }
        outfile << "\n";
    }

}


void CTransform::ShuffleData(const cv::Mat& matFeature, const cv::Mat& matLabel, cv::Mat& matFeatureShuffled, cv::Mat& matLabelShuffled)
{
    if (matFeature.rows != matLabel.rows)
    {
        std::cerr << "WARNING: The feature length unequals the label length!" << std::endl;
        return;
    }

    std::vector<int> seeds;
    for (int i = 0; i < matFeature.rows; i++)
    {
        seeds.push_back(i);
    }
    cv::randShuffle(seeds);

    for (int i = 0; i < matFeature.rows; i++)
    {
        matFeatureShuffled.push_back(matFeature.row(seeds[i]));
        matLabelShuffled.push_back(matLabel.row(seeds[i]));
    }
}


void CTransform::SplitData(cv::Mat& matData, cv::Mat& matDataTrain, cv::Mat& matDataTest, float fRate)
{
    int train_num = int(fRate * matData.rows);

    for (int i = 0; i < train_num; i++)
    {
        matDataTrain.push_back(matData.row(i));
    }

    for (int i = train_num; i < matData.rows; i++)
    {
        matDataTest.push_back(matData.row(i));
    }
}
