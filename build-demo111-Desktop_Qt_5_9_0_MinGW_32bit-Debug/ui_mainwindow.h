/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *preBtn;
    QPushButton *playBtn;
    QPushButton *nextBtn;
    QPushButton *listBtn;
    QPushButton *modeBtn;
    QListWidget *musicList;
    QSlider *progressSlider;
    QLabel *totalTimeLabel;
    QLabel *currentTimeLabel;
    QToolBar *mainToolBar;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(540, 389);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 220, 491, 191));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        preBtn = new QPushButton(widget);
        preBtn->setObjectName(QStringLiteral("preBtn"));

        horizontalLayout->addWidget(preBtn);

        playBtn = new QPushButton(widget);
        playBtn->setObjectName(QStringLiteral("playBtn"));

        horizontalLayout->addWidget(playBtn);

        nextBtn = new QPushButton(widget);
        nextBtn->setObjectName(QStringLiteral("nextBtn"));

        horizontalLayout->addWidget(nextBtn);

        listBtn = new QPushButton(widget);
        listBtn->setObjectName(QStringLiteral("listBtn"));

        horizontalLayout->addWidget(listBtn);

        modeBtn = new QPushButton(widget);
        modeBtn->setObjectName(QStringLiteral("modeBtn"));

        horizontalLayout->addWidget(modeBtn);

        musicList = new QListWidget(centralWidget);
        musicList->setObjectName(QStringLiteral("musicList"));
        musicList->setGeometry(QRect(280, -10, 256, 311));
        progressSlider = new QSlider(centralWidget);
        progressSlider->setObjectName(QStringLiteral("progressSlider"));
        progressSlider->setGeometry(QRect(60, 80, 160, 16));
        progressSlider->setOrientation(Qt::Horizontal);
        totalTimeLabel = new QLabel(centralWidget);
        totalTimeLabel->setObjectName(QStringLiteral("totalTimeLabel"));
        totalTimeLabel->setGeometry(QRect(80, 120, 54, 12));
        currentTimeLabel = new QLabel(centralWidget);
        currentTimeLabel->setObjectName(QStringLiteral("currentTimeLabel"));
        currentTimeLabel->setGeometry(QRect(80, 140, 54, 12));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 540, 21));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        preBtn->setText(QString());
        playBtn->setText(QString());
        nextBtn->setText(QString());
        listBtn->setText(QString());
        modeBtn->setText(QString());
        totalTimeLabel->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
        currentTimeLabel->setText(QApplication::translate("MainWindow", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
