#include "c_show_thread.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

// DebugMK
#include "c_barrel_correction.h"

// DebugMK
#include "debug_log.h"


static VideoCapture videoCapture;
static bool bIsExit = false;

CShowThread::CShowThread()
{
    start();
}


CShowThread::~CShowThread()
{
    c_qMutex.lock();
    bIsExit = true;
    c_qMutex.unlock();

    wait();
}

void CShowThread::run()
{
    Mat mat;

    for (;;)
    {
        c_qMutex.lock();
        if (bIsExit)
        {
            c_qMutex.unlock();
            break;
        }
        // 是否打开
        if (!videoCapture.isOpened())
        {
            msleep(5);
            c_qMutex.unlock();
            continue;
        }
        // 是否读取
        if (!videoCapture.read(mat) || mat.empty())
        {
            msleep(5);
            c_qMutex.unlock();
            continue;
        }

        // 图像处理算法
        #if 0
        CBarrelCorrection cBarrelCorrection;
        mat = cBarrelCorrection.BarrelCorrection(mat);
        #endif

        SendMat(mat);
        msleep(40);

        c_qMutex.unlock();
    }
}

bool CShowThread::Open(const std::string strFileName)
{
    c_qMutex.lock();
    bool ret = videoCapture.open(strFileName);
    c_qMutex.unlock();

    return ret;
}
