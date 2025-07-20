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
    m_mode(LIST_MODE),
    ui(new Ui::MainWindow),
    m_shuffleIndex(-1)
{
    ui->setupUi(this);
    setWindowTitle("音乐播放器");

    m_player = new QMediaPlayer(this);

    // 中央部件布局（保持原结构）
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);
    splitter->setChildrenCollapsible(false);

    QWidget *listWidget = new QWidget(this);
    QVBoxLayout *listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);
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

    QWidget *playerWidget = new QWidget(this);
    QVBoxLayout *playerLayout = new QVBoxLayout(playerWidget);
    playerLayout->setContentsMargins(10, 10, 10, 10);
    playerLayout->setSpacing(10);

    // 进度条占位（不实现功能）
    QHBoxLayout *progressLayout = new QHBoxLayout();
    QLabel *currentTimeLabel = new QLabel("00:00", this);
    QSlider *progressSlider = new QSlider(Qt::Horizontal, this);
    QLabel *totalTimeLabel = new QLabel("00:00", this);
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
    progressLayout->addWidget(currentTimeLabel);
    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(totalTimeLabel);

    QWidget *buttonContainer = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(20, 10, 20, 20);
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

    playerLayout->addLayout(progressLayout);
    playerLayout->addStretch();
    playerLayout->addWidget(buttonContainer);
    splitter->addWidget(listWidget);
    splitter->addWidget(playerWidget);
    splitter->setSizes(QList<int>() << 200 << 400);
    mainLayout->addWidget(splitter);

    initButtons();

    // 音乐文件夹路径（确保末尾有分隔符）
    musicDir = "D:\\实训\\musicplayer\\";
    loadAppointMusicDir(musicDir);

    srand(time(NULL));

    // 播放结束自动切歌
    connect(m_player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState && ui->musicList->count() > 0) {
            handleNextSlot();
        }
    });
}

void MainWindow::setButtonStyle(QPushButton* button, const QString& filename) {
    button->setFixedSize(80, 80);
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

void MainWindow::shufflePlayList() {
    m_shuffleList.clear();
    int count = ui->musicList->count();
    if (count <= 0) return;

    for (int i = 0; i < count; ++i) {
        m_shuffleList.push_back(i);
    }

    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(m_shuffleList[i], m_shuffleList[j]);
    }

    m_shuffleIndex = 0;
    qDebug() << "随机序列生成，共" << count << "首";
}

void MainWindow::initButtons() {
    setButtonStyle(ui->preBtn, ":/res/previous.png");
    setButtonStyle(ui->playBtn, ":/res/play.png");
    setButtonStyle(ui->nextBtn, ":/res/next.png");
    setButtonStyle(ui->modeBtn, ":/res/list.png");
    setButtonStyle(ui->listBtn, ":/res/pure.png");

    connect(ui->playBtn, &QPushButton::clicked, this, &MainWindow::handlePlaySlot);
    connect(ui->modeBtn, &QPushButton::clicked, this, &MainWindow::handleModeSlot);
    connect(ui->nextBtn,&QPushButton::clicked,this,&MainWindow::handleNextSlot);
    connect(ui->preBtn,&QPushButton::clicked,this,&MainWindow::handlePrevSlot);

    // 列表项点击播放
    connect(ui->musicList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int row = ui->musicList->row(item);
        if (m_mode == RANDOM_MODE) {
            for (int i = 0; i < m_shuffleList.size(); ++i) {
                if (m_shuffleList[i] == row) {
                    m_shuffleIndex = i;
                    break;
                }
            }
        }
        QString musicFile = musicDir + item->text() + ".mp3";
        m_player->setMedia(QUrl::fromLocalFile(musicFile));
        m_player->play();
        ui->playBtn->setIcon(QIcon(":/res/pause.png"));
    });
}

void MainWindow::handlePlaySlot(){
    if (ui->musicList->count() == 0) return;

    // 未加载媒体时自动加载第一首
    if (m_player->mediaStatus() == QMediaPlayer::NoMedia) {
        ui->musicList->setCurrentRow(0);
        startPlayMusic();
        return;
    }

    if (m_player->state() == QMediaPlayer::PlayingState) {
        m_player->pause();
        ui->playBtn->setIcon(QIcon(":/res/play.png"));
    } else {
        m_player->play();
        ui->playBtn->setIcon(QIcon(":/res/pause.png"));
    }
}

void MainWindow::handleModeSlot(){
    m_mode = static_cast<PLAYMODE>((m_mode + 1) % 3);

    if (m_mode == RANDOM_MODE) {
        shufflePlayList();
    }

    switch(m_mode) {
        case LIST_MODE:
            ui->modeBtn->setIcon(QIcon(":/res/list.png"));
            qDebug() << "列表循环模式";
            break;
        case RANDOM_MODE:
            ui->modeBtn->setIcon(QIcon(":/res/random.png"));
            qDebug() << "随机播放模式";
            break;
        case SINGLE_MODE:
            ui->modeBtn->setIcon(QIcon(":/res/single.png"));
            qDebug() << "单曲循环模式";
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
            nextRow = (currentRow + 1) % count;
            break;
        }
        case SINGLE_MODE: {
            nextRow = ui->musicList->currentRow();
            break;
        }
        case RANDOM_MODE: {
            if (m_shuffleList.empty()) {
                shufflePlayList();
            } else {
                m_shuffleIndex = (m_shuffleIndex + 1) % m_shuffleList.size();
            }
            nextRow = m_shuffleList[m_shuffleIndex];
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
            preRow = (currentRow - 1 + count) % count;
            break;
        }
        case SINGLE_MODE: {
            preRow = ui->musicList->currentRow();
            break;
        }
        case RANDOM_MODE: {
            if (m_shuffleList.empty()) {
                shufflePlayList();
                m_shuffleIndex = m_shuffleList.size() - 1;
            } else {
                m_shuffleIndex = (m_shuffleIndex - 1 + m_shuffleList.size()) % m_shuffleList.size();
            }
            preRow = m_shuffleList[m_shuffleIndex];
            break;
        }
    }
    ui->musicList->setCurrentRow(preRow);
    startPlayMusic();
}

void MainWindow::startPlayMusic(){
    if (!ui->musicList->currentItem()) return;

    QString musicName = ui->musicList->currentItem()->text();
    QString Abs = musicDir + musicName + ".mp3";

    m_player->setMedia(QUrl::fromLocalFile(Abs));
    m_player->play();
    ui->playBtn->setIcon(QIcon(":/res/pause.png"));
}

void MainWindow::loadAppointMusicDir(const QString& filepath){
    QDir dir(filepath);
    if(!dir.exists()){
        QMessageBox::warning(this,"文件夹","文件夹打开失败: " + filepath);
        return;
    }

    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    for(auto e : fileList){
        if(e.suffix().toLower() == "mp3"){
            ui->musicList->addItem(e.baseName());
        }
    }

    if(ui->musicList->count() > 0){
        ui->musicList->setCurrentRow(0);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

MainWindow::~MainWindow() {
    delete ui;
}
