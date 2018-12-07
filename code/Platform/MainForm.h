#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::MainWindowClass ui;

////////////////////////////////////////////////////////////////////////////////

public slots:
    void Open();
    void ValueChanged();

private:
    void Init();

    void Test();

    void Mat2Array(cv::Mat matImage, int *pArray);

    void HistogramEnhancemen();
    void DistortionCorrection(int nCut_, int nBarrel_ParaL_, int nBarrel_ParaS_);


};
