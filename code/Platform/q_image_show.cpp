#include "q_image_show.h"

#include <QPainter>

#include <opencv2/imgproc.hpp>

QImageShow::QImageShow(QWidget *pqWidget) : QOpenGLWidget(pqWidget)
{
    this->c_ImgFormat = QImage::Format_RGB888;
    this->c_nPixelSize = 3;
}


QImageShow::~QImageShow()
{
}

void QImageShow::ReceiveMat(cv::Mat mat)
{
    if (mat.type() == CV_8UC1)
    {
        this->c_ImgFormat = QImage::Format_Grayscale8;
        this->c_nPixelSize = 1;
    }

    if (c_qImage.isNull() || c_qImage.format() != this->c_ImgFormat)
    {
        delete c_qImage.bits();

        uchar *pbyBuffer = new uchar[width() * height() * this->c_nPixelSize];
        c_qImage = QImage(pbyBuffer, width(), height(), this->c_ImgFormat);
    }

    cv::Mat matDes;
    cv::resize(mat, matDes, cv::Size(c_qImage.size().width(), c_qImage.size().height()));
    if (this->c_nPixelSize > 1)
    {
        cv::cvtColor(matDes, matDes, cv::COLOR_BGR2RGB);
    }    

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
