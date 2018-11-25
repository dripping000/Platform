#pragma once
#include <QThread>
#include <QMutex>

#include <opencv2/core.hpp>

class CShowThread : public QThread
{

    Q_OBJECT

signals:
    void SendMat(cv::Mat mat);

public:
    ~CShowThread();

public:
    static CShowThread* Get()
    {
        static CShowThread cShowThread;
        return &cShowThread;
    }
    //线程入口函数
    void run();

    bool Open(const std::string strFileName);


private:
    QMutex c_qMutex;

protected:
    CShowThread();



};

