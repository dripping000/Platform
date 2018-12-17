#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

// DebugMK
#include "c_barrel_correction.h"


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
    // 畸变矫正算法类
    CBarrelCorrection m_cBarrelCorrection;

private:
    void Init();

    void Test();    

    void BarrelCorrectionTest();

    void HistogramEnhancemen();
    void Mat2Array(cv::Mat matImage, int *pArray);

};
