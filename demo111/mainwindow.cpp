#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QSize>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSplitter>
#include <QStyle>
#include <QResizeEvent>
#include <QDebug>
#include <time.h>
#include <vector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_mode(LIST_MODE),  // 初始化为顺序播放
    ui(new Ui::MainWindow),
    m_shuffleIndex(-1)  // 洗牌序列当前索引（初始为-1表示未初始化）
{
    ui->setupUi(this);

    // 设置标题
    setWindowTitle("音乐播放器");

    // 音乐播放器
    m_player = new QMediaPlayer(this);

    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建分割器
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1); // 设置分割线宽度
    splitter->setChildrenCollapsible(false); // 防止子部件折叠

    // 创建左侧音乐列表区域
    QWidget *listWidget = new QWidget(this);
    QVBoxLayout *listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);

    // 美化音乐列表
    ui->musicList->setStyleSheet("QListWidget {"
                                 "    background-color: #f5f5f5;"
                                 "    border: 1px solid #ddd;"
                                 "}"
                                 "QListWidget::item {"
                                 "    padding: 5px 10px;"
                                 "    border-bottom: 1px solid #eee;"
                                 "    font-size: 14px;"
                                 "}"
                                 "QListWidget::item:selected {"
                                 "    background-color: #cce5ff;"
                                 "    color: #004085;"
                                 "}");
    listLayout->addWidget(ui->musicList);

    // 创建右侧播放器控制区域
    QWidget *playerWidget = new QWidget(this);
    QVBoxLayout *playerLayout = new QVBoxLayout(playerWidget);
    playerLayout->setContentsMargins(10, 10, 10, 10);
    playerLayout->setSpacing(10);

    // 创建进度条布局
    QHBoxLayout *progressLayout = new QHBoxLayout();
    QLabel *currentTimeLabel = new QLabel("00:00", this);
    QSlider *progressSlider = new QSlider(Qt::Horizontal, this);
    QLabel *totalTimeLabel = new QLabel("00:00", this);

    // 设置进度条样式
    progressSlider->setStyleSheet("QSlider::groove:horizontal {"
                                  "    border: 1px solid #999999;"
                                  "    height: 8px;"
                                  "    background: #CCCCCC;"
                                  "    margin: 2px 0;"
                                  "}"
                                  "QSlider::handle:horizontal {"
                                  "    background: #0099FF;"
                                  "    border: 1px solid #5C5C5C;"
                                  "    width: 16px;"
                                  "    margin: -2px 0;"
                                  "    border-radius: 4px;"
                                  "}");

    // 添加进度条组件到布局
    progressLayout->addWidget(currentTimeLabel);
    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(totalTimeLabel);

    // 创建按钮容器部件
    QWidget *buttonContainer = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(20);
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

    // 添加进度条和按钮到播放器布局
    playerLayout->addLayout(progressLayout);
    playerLayout->addStretch();
    playerLayout->addWidget(buttonContainer);

    // 将列表区域和播放器区域添加到分割器
    splitter->addWidget(listWidget);
    splitter->addWidget(playerWidget);

    // 设置初始大小比例
    splitter->setSizes(QList<int>() << 200 << 400);

    // 添加分割器到主布局
    mainLayout->addWidget(splitter);

    // 初始化按钮样式
    initButtons();

    // 加载音乐文件夹
    musicDir = "D:\\实训\\musicplayer";
    loadAppointMusicDir(musicDir);

    // 初始化随机数种子
    srand(time(NULL));
}

void MainWindow::setButtonStyle(QPushButton* button, const QString& filename) {
    // 增大按钮尺寸
    button->setFixedSize(80, 80);
    // 增大图标尺寸
    button->setIconSize(QSize(80, 80));
    button->setIcon(QIcon(filename));
    button->setStyleSheet("QPushButton {"
                          "    background-color: transparent;"
                          "    border: none;"
                          "    padding: 5px;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: rgba(0, 153, 255, 0.2);"
                          "    border-radius: 5px;"
                          "}");
}



// 洗牌算法（Fisher-Yates）：生成不重复的随机索引序列
void MainWindow::shufflePlayList() {
    m_shuffleList.clear();  // 清空原有序列
    int count = ui->musicList->count();
    if (count <= 0) return;

    // 初始化索引序列（0,1,2,...,n-1）
    for (int i = 0; i < count; ++i) {
        m_shuffleList.push_back(i);
    }

    // Fisher-Yates 洗牌：从后往前随机交换
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);  // 生成0到i的随机索引
        std::swap(m_shuffleList[i], m_shuffleList[j]);  // 交换元素
    }

    m_shuffleIndex = 0;  // 重置当前播放位置为序列开头
    qDebug() << "随机播放序列已生成，共" << count << "首歌曲";
}



void MainWindow::initButtons() {
    setButtonStyle(ui->preBtn, ":/res/previous.png");
    setButtonStyle(ui->playBtn, ":/res/play.png");
    setButtonStyle(ui->nextBtn, ":/res/next.png");
    setButtonStyle(ui->modeBtn, ":/res/list.png");
    setButtonStyle(ui->listBtn, ":/res/pure.png");

    // 播放和暂停的信号和槽
    connect(ui->playBtn, &QPushButton::clicked, this, &MainWindow::handlePlaySlot);

    // 模式更改的信号和槽
    connect(ui->modeBtn, &QPushButton::clicked, this, &MainWindow::handleModeSlot);

    // 下一首的信号和槽
    connect(ui->nextBtn,&QPushButton::clicked,this,&MainWindow::handleNextSlot);

    // 上一首的信号和槽
    connect(ui->preBtn,&QPushButton::clicked,this,&MainWindow::handlePrevSlot);

    // 列表项点击切换歌曲（需要关联播放逻辑，这里补充）
    connect(ui->musicList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int row = ui->musicList->row(item);
        // 切换歌曲时如果是随机模式，需要重新定位洗牌序列
        if (m_mode == RANDOM_MODE) {
            // 查找当前歌曲在洗牌序列中的位置
            for (int i = 0; i < m_shuffleList.size(); ++i) {
                if (m_shuffleList[i] == row) {
                    m_shuffleIndex = i;
                    break;
                }
            }
        }
        // 这里添加设置播放器媒体的代码（根据实际路径设置）
        QString musicDir = "D:\\实训\\musicplayer";
        QString musicFile = musicDir + "\\" + item->text() + ".mp3";
        m_player->setMedia(QUrl::fromLocalFile(musicFile));
        m_player->play();
    });
}

void MainWindow::handlePlaySlot(){
    if (m_player->state() == QMediaPlayer::PlayingState) {
        m_player->pause();
        ui->playBtn->setIcon(QIcon(":/res/pause.png"));
    } else {
        m_player->play();
        ui->playBtn->setIcon(QIcon(":/res/play.png"));
    }
}

void MainWindow::handleModeSlot(){
    // 循环切换播放模式
    m_mode = static_cast<PLAYMODE>((m_mode + 1) % 3);

    // 切换到随机模式时立即生成洗牌序列
    if (m_mode == RANDOM_MODE) {
        shufflePlayList();
    }

    // 根据当前模式更新按钮图标
    switch(m_mode) {
        case LIST_MODE:
            ui->modeBtn->setIcon(QIcon(":/res/list.png"));
            qDebug() << "切换到列表循环模式";
            break;
        case RANDOM_MODE:
            ui->modeBtn->setIcon(QIcon(":/res/random.png"));
            qDebug() << "切换到随机播放模式";
            break;
        case SINGLE_MODE:
            ui->modeBtn->setIcon(QIcon(":/res/single.png"));
            qDebug() << "切换到单曲循环模式";
            break;
    }
}

void MainWindow::handleNextSlot(){
    int count = ui->musicList->count();
    if (count <= 0) return;

    int nextRow = 0;
    switch(m_mode) {
        case LIST_MODE: {
            int currentRow = ui->musicList->currentRow();
            nextRow = (currentRow + 1) % count;  // 顺序循环
            break;
        }
        case SINGLE_MODE: {
            nextRow = ui->musicList->currentRow();  // 保持当前行
            break;
        }
        case RANDOM_MODE: {
            // 若序列未初始化或已播放完毕，重新洗牌
            if (m_shuffleList.empty() || m_shuffleIndex >= m_shuffleList.size() - 1) {
                shufflePlayList();
            } else {
                m_shuffleIndex++;  // 播放序列中的下一首
            }
            nextRow = m_shuffleList[m_shuffleIndex];  // 从洗牌序列中取索引
            break;
        }
    }
    ui->musicList->setCurrentRow(nextRow);

    startPlayMusic();

}

void MainWindow::handlePrevSlot(){
    int count = ui->musicList->count();
    if (count <= 0) return;

    int preRow = 0;
    switch(m_mode) {
        case LIST_MODE: {
            int currentRow = ui->musicList->currentRow();
            preRow = (currentRow - 1 + count) % count;  // 顺序循环（避免负数）
            break;
        }
        case SINGLE_MODE: {
            preRow = ui->musicList->currentRow();  // 保持当前行
            break;
        }
        case RANDOM_MODE: {
            // 若序列未初始化或已到开头，重新洗牌
            if (m_shuffleList.empty() || m_shuffleIndex <= 0) {
                shufflePlayList();
                // 重新洗牌后默认取最后一首作为上一首
                m_shuffleIndex = m_shuffleList.size() - 1;
            } else {
                m_shuffleIndex--;  // 播放序列中的上一首
            }
            preRow = m_shuffleList[m_shuffleIndex];  // 从洗牌序列中取索引
            break;
        }
    }
    ui->musicList->setCurrentRow(preRow);
    startPlayMusic();
}

//播放音乐
void MainWindow::startPlayMusic(){
    QString musicName=ui->musicList->currentItem()->text();
    //绝对路径
    QString Abs=musicDir+musicName+".mp3";

    //media加载音乐
    m_player->setMedia(QUrl::fromLocalFile(Abs));

   handleModeSlot();

}

// 加载指定的文件夹
void MainWindow::loadAppointMusicDir(const QString& filepath){
    QDir dir(filepath);
    if(dir.exists()==false){
        QMessageBox::warning(this,"文件夹","文件夹打开失败");
        return;
    }
    // 获取文件列表
    QFileInfoList fileList=dir.entryInfoList(QDir::Files);

    for(auto e:fileList){
        // 筛选MP3文件
        if(e.suffix().toLower()=="mp3"){
            ui->musicList->addItem(e.baseName());
        }
    }
    // 打开后第一首高亮处理
    if(ui->musicList->count() > 0){
        ui->musicList->setCurrentRow(0);
    }
}

// 重写resizeEvent方法，确保界面元素随窗口大小变化
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    // 布局管理器会自动调整，无需额外处理
}

MainWindow::~MainWindow() {
    delete ui;
}
