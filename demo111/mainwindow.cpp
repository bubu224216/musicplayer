#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QSize>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置标题
    setWindowTitle("音乐播放器");

    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 创建进度条布局
    QHBoxLayout *progressLayout = new QHBoxLayout();
    QLabel *currentTimeLabel = new QLabel("00:00", this);
    QSlider *progressSlider = new QSlider(Qt::Horizontal, this);
    QLabel *totalTimeLabel = new QLabel("00:00", this);

    // 设置进度条样式
    progressSlider->setStyleSheet("QSlider::groove:horizontal { border: 1px solid #999999; height: 8px; background: #CCCCCC; margin: 2px 0; }"
                                  "QSlider::handle:horizontal { background: #0099FF; border: 1px solid #5C5C5C; width: 16px; margin: -2px 0; border-radius: 4px; }");

    // 添加进度条组件到布局
    progressLayout->addWidget(currentTimeLabel);
    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(totalTimeLabel);

    // 创建按钮容器部件
    QWidget *buttonContainer = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(20, 10, 20, 20);

    // 将按钮添加到布局，并设置伸缩性
    buttonLayout->addStretch();
    buttonLayout->addWidget(ui->preBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(ui->playBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(ui->nextBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(ui->modeBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(ui->listBtn);
    buttonLayout->addStretch();

    // 添加进度条和按钮到主布局
    mainLayout->addLayout(progressLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonContainer);

    // 初始化按钮样式
    initButtons();
}

void MainWindow::setButtonStyle(QPushButton* button, const QString& filename) {
    button->setFixedSize(50, 50);
    button->setIcon(QIcon(filename));
    button->setIconSize(QSize(50, 50));
    button->setStyleSheet("background-color: transparent; border: none;");
}

void MainWindow::initButtons() {
    setButtonStyle(ui->preBtn, ":/res/previous.png");
    setButtonStyle(ui->playBtn, ":/res/pause.png");
    setButtonStyle(ui->nextBtn, ":/res/next.png");
    setButtonStyle(ui->modeBtn, ":/res/list.png");
    setButtonStyle(ui->listBtn, ":/res/pure.png");
}

MainWindow::~MainWindow() {
    delete ui;
}
