#include "q_image_show.h"

#include <QPainter>

#include <opencv2/imgproc.hpp>

QImageShow::QImageShow(QWidget *pqWidget) : QOpenGLWidget(pqWidget)
{
}


QImageShow::~QImageShow()
{
}

void QImageShow::ReceiveMat(cv::Mat mat)
{
    if (c_qImage.isNull())
    {
        uchar *pbyBuffer = new uchar[width() * height() * 3];
        c_qImage = QImage(pbyBuffer, width(), height(), QImage::Format_RGB888);
    }

    cv::Mat matDes;
    cv::resize(mat, matDes, cv::Size(c_qImage.size().width(), c_qImage.size().height()));
    cv::cvtColor(matDes, matDes, cv::COLOR_BGR2RGB);

    memcpy(c_qImage.bits(), matDes.data, matDes.cols*matDes.rows*matDes.elemSize());
    update();
}

void QImageShow::paintEvent(QPaintEvent *pqPaintEvent)
{
    QPainter qPainter;
    qPainter.begin(this);
    qPainter.drawImage(QPoint(0, 0), c_qImage);
    qPainter.end();
}
