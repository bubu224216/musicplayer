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
#include <QToolBar>


//构造函数以及ui外观的设计
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_mode(LIST_MODE),
    ui(new Ui::MainWindow),
    m_shuffleIndex(-1),
    m_isManualSwitch(false) // 初始化为 false  防止自动播放和手动播放之间的冲突
{
    ui->setupUi(this);
    resize(1680, 1000);
    setWindowTitle("音乐播放器");

    QWidget *controlPanel = new QWidget(this);
    controlPanel->setStyleSheet("background-color: rgba(255, 255, 255, 180); border-radius: 10px;");


    m_player = new QMediaPlayer(this);
    QToolBar *toolBar = addToolBar("工具栏");
    toolBar->setMovable(false); // 禁止拖动
    toolBar->setStyleSheet("QToolBar { background: transparent; border: none; }");

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);

    QAction *addMusicAction = new QAction(QIcon(":/res/add_to.png"), "添加音乐", this);
    QAction *changeThemeAction = new QAction(QIcon(":/res/theme.png"), "更换主题", this);

    toolBar->addAction(addMusicAction);
    toolBar->addAction(changeThemeAction);

    connect(addMusicAction, &QAction::triggered, this, &MainWindow::handleAddMusicSlot);
    connect(changeThemeAction, &QAction::triggered, this, &MainWindow::handleChangeBackgroundSlot);


    // 中央部件布局
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

    splitter->setHandleWidth(0);

    ui->musicList->setStyleSheet("QListWidget {"
                                 "    background-color: transparent;"
                                 "    border: 1px solid #ddd;"
                                 "}"
                                 "QListWidget::item {"
                                 "    padding: 5px 10px;"
                                 "    border-bottom: 1px solid #eee;"
                                 "    font-size: 14px;"
                                 "}"
                                 "QListWidget::item:selected {"
                                 "    background-color: rgba(204, 229, 255, 0.5);"
                                 "    color: #004085;"
                                 "}");
    ui->musicList->setStyleSheet("QListWidget { background: transparent; border: none; }");

    listLayout->addWidget(ui->musicList);

    QWidget *playerWidget = new QWidget(this);
    QVBoxLayout *playerLayout = new QVBoxLayout(playerWidget);
    playerLayout->setContentsMargins(10, 10, 10, 10);
    playerLayout->setSpacing(10);

    // 进度条
    QHBoxLayout *progressLayout = new QHBoxLayout();
    ui->progressSlider->setStyleSheet("QSlider::groove:horizontal {"
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
    progressLayout->addWidget(ui->currentTimeLabel);
    progressLayout->addWidget(ui->progressSlider);
    progressLayout->addWidget(ui->totalTimeLabel);

    // 在构造函数中保存UI设计器中控件的指针
    m_progressSlider = ui->progressSlider;
    m_currentTimeLabel = ui->currentTimeLabel;
    m_totalTimeLabel = ui->totalTimeLabel;

    // 连接播放器信号到进度更新函数
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::updateProgress);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);

    // 连接进度条信号到位置设置函数
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(m_progressSlider, &QSlider::sliderPressed, this, &MainWindow::handleProgressSliderPressed);
    connect(m_progressSlider, &QSlider::sliderReleased, this, &MainWindow::handleProgressSliderReleased);



    //放在widget当中用于调整布局
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
    buttonLayout->addWidget(ui->soundBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(ui->togglelistBtn);
    buttonLayout->addStretch();




    m_listWidget = listWidget;



    playerLayout->addStretch();
    playerLayout->addLayout(progressLayout);
    playerLayout->addWidget(buttonContainer);
    splitter->addWidget(listWidget);
    splitter->addWidget(playerWidget);
    splitter->setSizes(QList<int>() << 200 << 400);
    mainLayout->addWidget(splitter);
    setBackGround(":/res/bcakground.jpg");

    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setContentsMargins(15, 10, 15, 10);
    controlLayout->setSpacing(10);

    // 添加进度条区域（已有的 progressLayout）
    controlLayout->addLayout(progressLayout);

    // 添加按钮区域（已有的 buttonLayout）
    controlLayout->addWidget(buttonContainer);  // 包含播放、暂停等按钮
    playerLayout->addWidget(controlPanel);
    controlPanel->setStyleSheet(R"(
        QWidget {
            background-color: rgba(255, 255, 255, 200);
            border-radius: 15px;
            border: 1px solid #ccc;
        }
    )");


    initButtons();

    //后面添加了图形界面添加歌曲
    //这两行代码是否还需要？考虑
    // 音乐文件夹路径（确保末尾有分隔符）
//    musicDir = "D:\\实训\\musicplayer\\";
//    loadAppointMusicDir(musicDir);

    srand(time(NULL));

    //自动切歌和手动切歌之间有些问题的存在，存在冲突的现象



    // 播放结束自动切歌
    connect(m_player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state) {
        // 只有当不是手动切歌，且播放器确实停止（自然结束），才自动切下一首
        if (state == QMediaPlayer::StoppedState
            && ui->musicList->count() > 0
            && !m_isManualSwitch) {
            handleNextSlot();
        }
    });
}

//按钮的外观设计代码
void MainWindow::setButtonStyle(QAbstractButton* button, const QString& filename) {
    button->setFixedSize(80, 80);
    button->setIconSize(QSize(80, 80));
    button->setIcon(QIcon(filename));
    button->setStyleSheet("QAbstractButton {"
                          "    background-color: transparent;"
                          "    border: none;"
                          "    padding: 5px;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: rgba(0, 153, 255, 0.2);"
                          "    border-radius: 5px;"
                          "}");
}


//实现真随机的洗牌代码
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

//按钮的初始化，信息和槽的实现
void MainWindow::initButtons() {
    setButtonStyle(ui->preBtn, ":/res/previous.png");
    setButtonStyle(ui->playBtn, ":/res/play.png");
    setButtonStyle(ui->nextBtn, ":/res/next.png");
    setButtonStyle(ui->modeBtn, ":/res/list.png");
    setButtonStyle(ui->addBtn, ":/res/add_to.png");
    setButtonStyle(ui->themeBtn, ":/res/theme.png");
    setButtonStyle(ui->soundBtn, ":/res/sound.png");
    setButtonStyle(ui->togglelistBtn, ":/res/playlist.png");


    connect(ui->playBtn, &QPushButton::clicked, this, &MainWindow::handlePlaySlot);
    connect(ui->modeBtn, &QPushButton::clicked, this, &MainWindow::handleModeSlot);
    connect(ui->nextBtn,&QPushButton::clicked,this,&MainWindow::handleNextSlot);
    connect(ui->preBtn,&QPushButton::clicked,this,&MainWindow::handlePrevSlot);
    connect(ui->addBtn, &QPushButton::clicked, this, &MainWindow::handleAddMusicSlot);
    connect(ui->themeBtn, &QPushButton::clicked, this, &MainWindow::handleChangeBackgroundSlot);
    connect(ui->togglelistBtn, &QPushButton::clicked, this, &MainWindow::handleToggleListSlot);


    // 列表项点击播放
    // 在initButtons()函数中
    connect(ui->musicList, &QListWidget::itemClicked, this, &MainWindow::handleMusicListItemClicked);
}

//播放按钮的槽函数
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

//播放模式设计的槽函数
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

//下一首的槽函数
void MainWindow::handleNextSlot(){
    int count = ui->musicList->count();
    if (count <= 0) return;

    m_isManualSwitch = true;
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

    m_isManualSwitch = false;
}

//上一首的槽函数
void MainWindow::handlePrevSlot(){
    int count = ui->musicList->count();
    if (count <= 0) return;

    m_isManualSwitch = true;

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

    m_isManualSwitch = false;
}

//列表点击播放音乐的槽函数
void MainWindow::handleMusicListItemClicked(QListWidgetItem *item)
{
    m_isManualSwitch = true;
    int row = ui->musicList->row(item);

    // 更新随机播放模式下的索引
    if (m_mode == RANDOM_MODE) {
        for (int i = 0; i < m_shuffleList.size(); ++i) {
            if (m_shuffleList[i] == row) {
                m_shuffleIndex = i;
                break;
            }
        }
    }

    // 使用m_fullFileNameMap获取完整文件名（含后缀）
    QString musicName = item->text();
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到音乐文件：" + musicName);
        m_isManualSwitch = false;
        return;
    }

    QString Abs = musicDir + fullFileName;
    qDebug() << "播放路径：" << Abs;
    m_player->setMedia(QUrl::fromLocalFile(Abs));
    m_player->play();
    ui->playBtn->setIcon(QIcon(":/res/pause.png"));

    m_isManualSwitch = false;
}
// 添加音乐文件的槽函数
void MainWindow::handleAddMusicSlot() {
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this, "选择音乐文件", QDir::homePath(),
        "音频文件 (*.mp3 *.wav *.flac);;所有文件 (*)"
    );

    if (!filePaths.isEmpty()) {
        if (musicDir.isEmpty()) {
            QFileInfo firstFile(filePaths.first());
            musicDir = firstFile.absolutePath() + "/";
        }

        for (const QString& filePath : filePaths) {
            QFileInfo fileInfo(filePath);
            QString suffix = fileInfo.suffix().toLower();
            // 只处理支持的格式
            if (suffix == "mp3" || suffix == "wav" || suffix == "flac") {
                QString musicName = fileInfo.baseName(); // 不含后缀的文件名
                QString fullName = fileInfo.fileName();   // 含后缀的完整文件名

                // 避免重复添加
                bool exists = false;
                for (int i = 0; i < ui->musicList->count(); ++i) {
                    if (ui->musicList->item(i)->text() == musicName) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    ui->musicList->addItem(musicName);
                    // 关键：添加映射关系（不含后缀 -> 含后缀）
                    m_fullFileNameMap[musicName] = fullName;
                }
            }
        }

        // 首次添加时自动播放
        if (ui->musicList->count() > 0 && m_player->mediaStatus() == QMediaPlayer::NoMedia) {
            ui->musicList->setCurrentRow(0);
            startPlayMusic();
        }
    }
}
void MainWindow::handleChangeBackgroundSlot(){
    // 打开文件对话框，限制只显示图片文件
        QString filePath = QFileDialog::getOpenFileName(
            this,                        // 父窗口指针
            "选择背景图片",               // 对话框标题
            QDir::homePath(),            // 默认打开路径（用户主目录）
            "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif);;所有文件 (*)"  // 文件过滤器
        );

        // 如果用户取消了选择，filePath 会是空字符串
        if (filePath.isEmpty()) {
            qDebug() << "用户取消了背景图片选择";
            return;
        }

        // 获取文件信息（可选，用于验证文件有效性）
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            QMessageBox::warning(this, "错误", "选择的文件不存在或无效");
            return;
        }

        // 可选：立即应用背景图片
        setBackGround(filePath);

}

void MainWindow::handleToggleListSlot(){
    if (m_listWidget->isVisible()) {
           m_listWidget->hide();
       } else {
           m_listWidget->show();
       }

}


//开始播放音乐函数
void MainWindow::startPlayMusic() {
    if (!ui->musicList->currentItem()) return;

    QString musicName = ui->musicList->currentItem()->text();
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到音乐文件：" + musicName);
        return;
    }

    QString Abs = musicDir + fullFileName;
    qDebug() << "播放路径：" << Abs; // 打印路径，检查是否正确
    m_player->setMedia(QUrl::fromLocalFile(Abs));
    m_player->play();
    ui->playBtn->setIcon(QIcon(":/res/pause.png"));


    // 重置进度条和时间显示
    m_progressSlider->setValue(0);
    m_currentTimeLabel->setText("00:00");
    m_totalTimeLabel->setText("00:00");
}

//加载音乐文件的函数
void MainWindow::loadAppointMusicDir(const QString& filepath){
    QDir dir(filepath);
    if(!dir.exists()){
        QMessageBox::warning(this,"文件夹","文件夹打开失败: " + filepath);
        return;
    }

    // 定义支持的音频格式（转为小写，避免大小写问题）
    QStringList supportedFormats = {"mp3", "wav", "flac"};

    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    for(auto e : fileList){
        // 检查文件后缀是否在支持的格式列表中
        if(supportedFormats.contains(e.suffix().toLower())){
            ui->musicList->addItem(e.baseName());  // 添加文件名（不含后缀）
            // 记录完整文件名（含后缀），用于后续播放
            m_fullFileNameMap[e.baseName()] = e.fileName();//使用了一个map的数据结构
        }
    }

    if(ui->musicList->count() > 0){
        ui->musicList->setCurrentRow(0);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

//设置背景的函数
void MainWindow::setBackGround(const QString & filename){
    setStyleSheet(QString("MainWindow { border-image: url(%1) 0 0 0 0 stretch stretch; }")
                      .arg(filename));

}


void MainWindow::updateProgress(qint64 position)
{
    // 仅当用户未主动拖动进度条时更新UI
    if (!m_sliderPressed && m_player->duration() > 0) {
        m_progressSlider->setValue(position);            // 更新进度条位置
        m_currentTimeLabel->setText(formatTime(position));  // 更新当前时间
    }
}

void MainWindow::updateDuration(qint64 duration)
{
    m_progressSlider->setRange(0, duration);       // 设置进度条范围
    m_totalTimeLabel->setText(formatTime(duration));  // 更新总时长
}

void MainWindow::handleProgressSliderPressed()
{
    m_sliderPressed = true;  // 标记用户正在拖动进度条
}

void MainWindow::handleProgressSliderReleased()
{
    m_sliderPressed = false;  // 标记拖动结束
    setPosition(m_progressSlider->value());  // 跳转到释放位置
}

void MainWindow::setPosition(int position)
{
    if (m_sliderPressed) {  // 仅在拖动释放后跳转，避免拖动过程中频繁跳转
        m_player->setPosition(position);
        m_currentTimeLabel->setText(formatTime(position));
    }
}

QString MainWindow::formatTime(qint64 timeMilliSeconds)
{
    qint64 seconds = timeMilliSeconds / 1000;    // 转换为秒
    qint64 minutes = seconds / 60;               // 提取分钟
    seconds = seconds % 60;                      // 剩余秒数
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}


MainWindow::~MainWindow() {
    delete ui;
}
