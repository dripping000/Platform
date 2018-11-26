#pragma once

#include <QOpenGLWidget>

#include <opencv2/core.hpp>

class QImageShow : public QOpenGLWidget
{

    Q_OBJECT

public slots:
    void ReceiveMat(cv::Mat mat);

public:
    QImageShow(QWidget *pqWidget);
    ~QImageShow();

private:
    QImage::Format c_ImgFormat;
    int c_nPixelSize;
    QImage c_qImage;


//Ë½ÓÐº¯Êý
void paintEvent(QPaintEvent *pqPaintEvent);
};

