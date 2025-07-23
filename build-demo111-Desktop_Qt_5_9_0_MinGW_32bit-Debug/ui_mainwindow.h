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
#include <QtWidgets/QToolButton>
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
    QPushButton *addBtn;
    QPushButton *modeBtn;
    QListWidget *musicList;
    QSlider *progressSlider;
    QLabel *totalTimeLabel;
    QLabel *currentTimeLabel;
    QPushButton *themeBtn;
    QToolButton *soundBtn;
    QPushButton *togglelistBtn;
    QWidget *widget_2;
    QPushButton *nightBtn;
    QPushButton *daytimeBtn;
    QMenuBar *menuBar;

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

        addBtn = new QPushButton(widget);
        addBtn->setObjectName(QStringLiteral("addBtn"));

        horizontalLayout->addWidget(addBtn);

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
        themeBtn = new QPushButton(centralWidget);
        themeBtn->setObjectName(QStringLiteral("themeBtn"));
        themeBtn->setGeometry(QRect(150, 160, 80, 15));
        soundBtn = new QToolButton(centralWidget);
        soundBtn->setObjectName(QStringLiteral("soundBtn"));
        soundBtn->setGeometry(QRect(220, 180, 32, 16));
        togglelistBtn = new QPushButton(centralWidget);
        togglelistBtn->setObjectName(QStringLiteral("togglelistBtn"));
        togglelistBtn->setGeometry(QRect(90, 30, 80, 15));
        widget_2 = new QWidget(centralWidget);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setGeometry(QRect(160, 20, 120, 80));
        nightBtn = new QPushButton(centralWidget);
        nightBtn->setObjectName(QStringLiteral("nightBtn"));
        nightBtn->setGeometry(QRect(30, 180, 80, 15));
        daytimeBtn = new QPushButton(centralWidget);
        daytimeBtn->setObjectName(QStringLiteral("daytimeBtn"));
        daytimeBtn->setGeometry(QRect(20, 160, 80, 15));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 540, 17));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        preBtn->setText(QString());
        playBtn->setText(QString());
        nextBtn->setText(QString());
        addBtn->setText(QString());
        modeBtn->setText(QString());
        totalTimeLabel->setText(QApplication::translate("MainWindow", "00:00", Q_NULLPTR));
        currentTimeLabel->setText(QApplication::translate("MainWindow", "00:00", Q_NULLPTR));
        themeBtn->setText(QString());
        soundBtn->setText(QApplication::translate("MainWindow", "...", Q_NULLPTR));
        togglelistBtn->setText(QString());
        nightBtn->setText(QString());
        daytimeBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
