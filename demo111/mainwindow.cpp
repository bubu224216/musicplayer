#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QIcon>
#include<QSize>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置标题
    setWindowTitle("音乐播放器");

    //设置按钮大小
    ui->preBtn->setFixedSize(50,50);
    ui->preBtn->setIcon(QIcon(":res/previous.png"));
    ui->preBtn->setIconSize(QSize(ui->preBtn->width(),ui->preBtn->height()));
    ui->preBtn->setStyleSheet("background-color:transparent");

}

MainWindow::~MainWindow()
{
    delete ui;
}
