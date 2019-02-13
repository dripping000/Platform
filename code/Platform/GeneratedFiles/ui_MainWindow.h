/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "q_image_show.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QImageShow *ImageShow;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *Open;
    QSpacerItem *horizontalSpacer_2;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QSlider *BC_nK0_hs;
    QLabel *label_3;
    QSlider *BC_nK1_hs;
    QLineEdit *BC_nK1_le;
    QLineEdit *BC_nK0_le;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(992, 752);
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        ImageShow = new QImageShow(centralWidget);
        ImageShow->setObjectName(QStringLiteral("ImageShow"));
        ImageShow->setMinimumSize(QSize(960, 540));
        ImageShow->setMaximumSize(QSize(960, 540));

        horizontalLayout_2->addWidget(ImageShow);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        Open = new QPushButton(centralWidget);
        Open->setObjectName(QStringLiteral("Open"));

        horizontalLayout->addWidget(Open);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        BC_nK0_hs = new QSlider(centralWidget);
        BC_nK0_hs->setObjectName(QStringLiteral("BC_nK0_hs"));
        BC_nK0_hs->setMaximum(800);
        BC_nK0_hs->setPageStep(2);
        BC_nK0_hs->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(BC_nK0_hs, 0, 1, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        BC_nK1_hs = new QSlider(centralWidget);
        BC_nK1_hs->setObjectName(QStringLiteral("BC_nK1_hs"));
        BC_nK1_hs->setMaximum(800);
        BC_nK1_hs->setPageStep(2);
        BC_nK1_hs->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(BC_nK1_hs, 1, 1, 1, 1);

        BC_nK1_le = new QLineEdit(centralWidget);
        BC_nK1_le->setObjectName(QStringLiteral("BC_nK1_le"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(BC_nK1_le->sizePolicy().hasHeightForWidth());
        BC_nK1_le->setSizePolicy(sizePolicy);
        BC_nK1_le->setReadOnly(false);

        gridLayout->addWidget(BC_nK1_le, 1, 2, 1, 1);

        BC_nK0_le = new QLineEdit(centralWidget);
        BC_nK0_le->setObjectName(QStringLiteral("BC_nK0_le"));
        sizePolicy.setHeightForWidth(BC_nK0_le->sizePolicy().hasHeightForWidth());
        BC_nK0_le->setSizePolicy(sizePolicy);
        BC_nK0_le->setReadOnly(false);

        gridLayout->addWidget(BC_nK0_le, 0, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 992, 23));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        retranslateUi(MainWindowClass);
        QObject::connect(Open, SIGNAL(clicked()), MainWindowClass, SLOT(Open()));
        QObject::connect(BC_nK0_hs, SIGNAL(sliderReleased()), MainWindowClass, SLOT(ValueChanged()));
        QObject::connect(BC_nK1_hs, SIGNAL(sliderReleased()), MainWindowClass, SLOT(ValueChanged()));

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "MainWindow", Q_NULLPTR));
        Open->setText(QApplication::translate("MainWindowClass", "Open", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindowClass", "BC_nK0:", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindowClass", "BC_nK1:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
