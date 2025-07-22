#include "mainwindow.h"
#include "ui_mainwindow.h"
// 头文件集中放在最上方，按Qt库→标准库→自定义逻辑排序
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
#include <QToolBar>
#include <QPropertyAnimation>
#include <QFileDialog>
#include <time.h>
#include <vector>
#include <algorithm>


// 构造函数：按"基础初始化→组件创建→布局组装→信号连接"分步实现
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_mode(LIST_MODE),
    ui(new Ui::MainWindow),
    m_shuffleIndex(-1),
    m_isManualSwitch(false),
    m_sliderPressed(false),
    m_currentLyricIndex(-1)
{
    // 第一步：基础初始化（窗口属性、核心组件）
    ui->setupUi(this);
    resize(1680, 1000);
    setWindowTitle("音乐播放器");
    m_player = new QMediaPlayer(this);  // 播放器核心组件

    // 主布局容器初始化
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    // 第二步：创建工具栏（快捷操作区）
    QToolBar *toolBar = addToolBar("工具栏");
    toolBar->setMovable(false);
    toolBar->setStyleSheet("QToolBar { background: transparent; border: none; }");
    // 工具栏右侧对齐（通过拉伸部件实现）
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);
    // 工具栏按钮
    QAction *addMusicAction = new QAction(QIcon(":/add_to.png"), "添加音乐", this);
    QAction *changeThemeAction = new QAction(QIcon(":/theme.png"), "更换主题", this);
    toolBar->addAction(addMusicAction);
    toolBar->addAction(changeThemeAction);
    // 工具栏信号连接（提前连接，避免分散）
    connect(addMusicAction, &QAction::triggered, this, &MainWindow::handleAddMusicSlot);
    connect(changeThemeAction, &QAction::triggered, this, &MainWindow::handleChangeBackgroundSlot);




    // 第四步：左侧音乐列表区
    QWidget *listWidget = new QWidget(this);
    QVBoxLayout *listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);
    // 列表样式
    ui->musicList->setStyleSheet("QListWidget {"
                                 "    background: transparent;"
                                 "    border: none;"
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
    listLayout->addWidget(ui->musicList);
    m_listWidget = listWidget;  // 保存列表容器引用



    // 第五步：右侧播放控制区（垂直布局）
    QWidget *playerWidget = new QWidget(this);
    QHBoxLayout *playerLayout = new QHBoxLayout(playerWidget);
    playerLayout->setContentsMargins(10, 10, 10, 10);
    playerLayout->setSpacing(10);

    //调整结构为上下结构
    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    topLayout->addWidget(listWidget);  // 左侧音乐列表
//    topLayout->addStretch();           // 右侧空白或封面等

    // 设置 topWidget 的尺寸策略，避免无限扩展
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    mainLayout->addWidget(topWidget); // 添加上半部分（列表）
//    mainLayout->addStretch();  // 移除这行，避免布局无限扩展
    mainLayout->addWidget(playerWidget);   // 播放区统一置底


    // 5.1 歌词显示部件
    m_lyricWidget = new QWidget(this);
    m_lyricWidget->setMinimumHeight(500);
    m_lyricWidget->setMinimumWidth(900);
    m_lyricWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_lyricWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0.3); border-radius: 5px;");
    m_lyricLayout = new QVBoxLayout(m_lyricWidget);
    m_lyricLayout->setAlignment(Qt::AlignCenter);
    m_lyricLayout->setSpacing(10);

    // 歌曲封面
    QWidget *cover = new QWidget;
    cover->setFixedSize(300, 300);  // 设置固定大小，根据需要调整

    // 创建布局
    QVBoxLayout *coverLayout = new QVBoxLayout(cover);
    coverLayout->setContentsMargins(0, 0, 0, 0);  // 移除边距
    coverLayout->addStretch();

    // 使用 QPalette 设置背景图片
    QPixmap background(":/bk6.jpg");  // 替换为你的图片路径
    QPalette palette;
    palette.setBrush(cover->backgroundRole(), QBrush(background.scaled(
        cover->size(),                // 缩放尺寸
        Qt::KeepAspectRatioByExpanding,  // 保持比例并填充
        Qt::SmoothTransformation       // 平滑缩放
    )));
    cover->setPalette(palette);
    cover->setAutoFillBackground(true);  // 启用自动填充背景
    m_lyricLayout->addWidget(cover);

    // 添加歌词区域的拉伸项，确保封面在顶部
    m_lyricLayout->addStretch();

    topLayout->addWidget(m_lyricWidget);  // 添加到右侧布局
    // 设置歌词组件的拉伸因子，避免无限扩展
    topLayout->setStretchFactor(m_lyricWidget, 1);


    // 5.2 进度条布局（水平）
    QHBoxLayout *progressLayout = new QHBoxLayout();
    m_progressSlider = ui->progressSlider;
    m_currentTimeLabel = ui->currentTimeLabel;
    m_totalTimeLabel = ui->totalTimeLabel;
    // 进度条样式
    m_progressSlider->setStyleSheet("QSlider::groove:horizontal {"
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
    progressLayout->addWidget(m_currentTimeLabel);
    progressLayout->addWidget(m_progressSlider);
    progressLayout->addWidget(m_totalTimeLabel);

    // 5.3 按钮控制区（水平）
    QWidget *buttonContainer = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(20, 10, 20, 20);
    // 按钮布局（添加拉伸使按钮居中）
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

    // 5.4 控制面板（包含进度条+按钮区）
    QWidget *controlPanel = new QWidget(this);
    controlPanel->setFixedHeight(200);
    controlPanel->setStyleSheet(R"(
        QWidget {
            background-color: rgba(255, 255, 255, 200);
            border-radius: 15px;
            border: 1px solid #ccc;
        }
    )");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setContentsMargins(15, 10, 15, 10);
    controlLayout->setSpacing(10);
    controlLayout->addLayout(progressLayout);  // 进度条
    controlLayout->addWidget(buttonContainer);  // 按钮区
    // 添加到右侧布局（歌词区下方）
//    playerLayout->addStretch();  // 歌词与控制区间的空白
    playerLayout->addWidget(controlPanel);

    // 设置 playerWidget 的尺寸策略，确保高度固定
    playerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setBackGround(":/bcakground.jpg");  // 设置背景


    // 第七步：初始化与信号连接
    initButtons();  // 按钮样式与信号
    srand(time(NULL));  // 随机数种子（用于随机播放）


    // 创建音量滑块弹出窗口
     m_volumePopup = new QWidget(this, Qt::Popup);  // 设置为弹出式窗口
     m_volumePopup->setStyleSheet("QWidget { background: rgba(255, 255, 255, 230); border: 1px solid #ccc; border-radius: 8px; }");

     QVBoxLayout* volumeLayout = new QVBoxLayout(m_volumePopup);
     volumeLayout->setContentsMargins(10, 10, 10, 10);

     m_volumeSlider = new QSlider(Qt::Vertical, m_volumePopup);
     m_volumeSlider->setRange(0, 100);
     m_volumeSlider->setValue(m_player->volume());  // 初始设置为当前音量

     m_volumeSlider->setStyleSheet(R"(
         QSlider::groove:vertical {
             background: #ccc; /* 轨道背景色 */
             width: 6px;
             border-radius: 3px;
         }
         QSlider::handle:vertical {
             background: #0099FF; /* 滑块按钮背景色 */
             height: 16px;
             margin: 0 -6px; /* 使滑块超出轨道宽度 */
             border-radius: 8px;
         }
     )");
     volumeLayout->addWidget(m_volumeSlider);

     // 控制播放器音量
     connect(m_volumeSlider, &QSlider::valueChanged, m_player, &QMediaPlayer::setVolume);

     //音量为0
        connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeSliderValueChanged);
    // 播放结束自动切歌信号
    connect(m_player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState && ui->musicList->count() > 0 && !m_isManualSwitch) {
            handleNextSlot();
        }
    });
    // 进度与时长更新信号
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::updateProgress);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    // 进度条交互信号
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(m_progressSlider, &QSlider::sliderPressed, this, &MainWindow::handleProgressSliderPressed);
    connect(m_progressSlider, &QSlider::sliderReleased, this, &MainWindow::handleProgressSliderReleased);
}


// 析构函数（放在构造函数之后）
MainWindow::~MainWindow() {
    delete ui;
}


// ------------------------------
// 一、播放控制核心槽函数（最常用功能优先）
// ------------------------------

// 播放/暂停切换
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
        ui->playBtn->setIcon(QIcon(":/bofang.png"));
    } else {
        m_player->play();
        ui->playBtn->setIcon(QIcon(":/pause.png"));
    }
}

// 下一首
void MainWindow::handleNextSlot(){
    int count = ui->musicList->count();
    if (count <= 0) return;

    m_isManualSwitch = true;
    int nextRow = 0;

    switch(m_mode) {
        case LIST_MODE:
            nextRow = (ui->musicList->currentRow() + 1) % count;
            break;
        case SINGLE_MODE:
            nextRow = ui->musicList->currentRow();
            break;
        case RANDOM_MODE:
            if (m_shuffleList.empty()) shufflePlayList();
            else m_shuffleIndex = (m_shuffleIndex + 1) % m_shuffleList.size();
            nextRow = m_shuffleList[m_shuffleIndex];
            break;
    }
    ui->musicList->setCurrentRow(nextRow);
    startPlayMusic();
    m_isManualSwitch = false;
}

// 上一首
void MainWindow::handlePrevSlot(){
    int count = ui->musicList->count();
    if (count <= 0) return;

    m_isManualSwitch = true;
    int preRow = 0;

    switch(m_mode) {
        case LIST_MODE:
            preRow = (ui->musicList->currentRow() - 1 + count) % count;
            break;
        case SINGLE_MODE:
            preRow = ui->musicList->currentRow();
            break;
        case RANDOM_MODE:
            if (m_shuffleList.empty()) {
                shufflePlayList();
                m_shuffleIndex = m_shuffleList.size() - 1;
            } else {
                m_shuffleIndex = (m_shuffleIndex - 1 + m_shuffleList.size()) % m_shuffleList.size();
            }
            preRow = m_shuffleList[m_shuffleIndex];
            break;
    }
    ui->musicList->setCurrentRow(preRow);
    startPlayMusic();
    m_isManualSwitch = false;
}

// 列表项点击播放
void MainWindow::handleMusicListItemClicked(QListWidgetItem *item){
    m_isManualSwitch = true;
    int row = ui->musicList->row(item);

    // 随机模式下更新索引
    if (m_mode == RANDOM_MODE) {
        for (int i = 0; i < m_shuffleList.size(); ++i) {
            if (m_shuffleList[i] == row) {
                m_shuffleIndex = i;
                break;
            }
        }
    }

    QString musicName = item->text();
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到音乐文件：" + musicName);
        m_isManualSwitch = false;
        return;
    }

    m_player->setMedia(QUrl::fromLocalFile(musicDir + fullFileName));
    m_player->play();
    ui->playBtn->setIcon(QIcon(":/pause.png"));
    m_isManualSwitch = false;

    // 加载歌词
    loadLyricForCurrentSong();
}


// ------------------------------
// 二、模式与界面控制槽函数
// ------------------------------

// 播放模式切换（列表/随机/单曲）
void MainWindow::handleModeSlot(){
    m_mode = static_cast<PLAYMODE>((m_mode + 1) % 3);
    if (m_mode == RANDOM_MODE) shufflePlayList();

    switch(m_mode) {
        case LIST_MODE:
            ui->modeBtn->setIcon(QIcon(":/shunxubofang.png"));
            qDebug() << "列表循环模式";
            break;
        case RANDOM_MODE:
            ui->modeBtn->setIcon(QIcon(":/suijibofang.png"));
            qDebug() << "随机播放模式";
            break;
        case SINGLE_MODE:
            ui->modeBtn->setIcon(QIcon(":/single.png"));
            qDebug() << "单曲循环模式";
            break;
    }
}

// 切换左侧列表显示/隐藏
void MainWindow::handleToggleListSlot(){
    m_listWidget->setVisible(!m_listWidget->isVisible());
}

// 更换背景图片
void MainWindow::handleChangeBackgroundSlot(){
    QString filePath = QFileDialog::getOpenFileName(
        this, "选择背景图片", QDir::homePath(),
        "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif);;所有文件 (*)"
    );
    if (!filePath.isEmpty() && QFile::exists(filePath)) {
        setBackGround(filePath);
    }
}


// ------------------------------
// 三、音乐管理槽函数
// ------------------------------

// 添加音乐文件
void MainWindow::handleAddMusicSlot(){
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this, "选择音乐文件", QDir::homePath(),
        "音频文件 (*.mp3 *.wav *.flac);;所有文件 (*)"
    );

    if (filePaths.isEmpty()) return;

    // 初始化音乐目录（首次添加时）
    if (musicDir.isEmpty()) {
        QFileInfo firstFile(filePaths.first());
        musicDir = firstFile.absolutePath() + "/";
    }

    // 去重添加到列表
    for (const QString& filePath : filePaths) {
        QFileInfo fileInfo(filePath);
        QString suffix = fileInfo.suffix().toLower();
        if (suffix != "mp3" && suffix != "wav" && suffix != "flac") continue;

        QString musicName = fileInfo.baseName();
        QString fullFileName = fileInfo.fileName();
        bool exists = false;
        for (int i = 0; i < ui->musicList->count(); ++i) {
            if (ui->musicList->item(i)->text() == musicName) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            ui->musicList->addItem(musicName);
            m_fullFileNameMap[musicName] = fullFileName;
        }
    }

    // 首次添加自动播放
    if (ui->musicList->count() > 0 && m_player->mediaStatus() == QMediaPlayer::NoMedia) {
        ui->musicList->setCurrentRow(0);
        startPlayMusic();
    }
}


// ------------------------------
// 四、进度条与歌词槽函数
// ------------------------------

// 进度条拖动开始
void MainWindow::handleProgressSliderPressed(){
    m_sliderPressed = true;
}

// 进度条拖动结束（更新播放位置）
void MainWindow::handleProgressSliderReleased(){
    m_sliderPressed = false;
    setPosition(m_progressSlider->value());
}

// 设置播放位置
void MainWindow::setPosition(int position){
    if (!m_sliderPressed) {
        m_player->setPosition(position);
        m_currentTimeLabel->setText(formatTime(position));
    }
}

// 更新进度（随播放实时更新）
void MainWindow::updateProgress(qint64 position){
    if (!m_sliderPressed && m_player->duration() > 0) {
        m_progressSlider->setValue(position);
        m_currentTimeLabel->setText(formatTime(position));
    }

    // 更新歌词显示
    if (!m_lyrics.isEmpty()) {
        int newIndex = -1;
        for (int i = 0; i < m_lyrics.size(); ++i) {
            if (m_lyrics[i].time <= position) newIndex = i;
            else break;
        }
        if (newIndex != -1 && newIndex != m_currentLyricIndex) {
            m_currentLyricIndex = newIndex;
            updateLyricDisplay();
        }
    }
}

// 更新总时长（音乐加载后）
void MainWindow::updateDuration(qint64 duration){
    m_progressSlider->setRange(0, duration);
    m_totalTimeLabel->setText(formatTime(duration));
}


// ------------------------------
// 五、基础辅助函数
// ------------------------------

// 初始化按钮样式与信号
void MainWindow::initButtons(){
    // 按钮样式设置（lambda复用）
    auto setBtnStyle = [&](QAbstractButton* btn, const QString& icon) {
        btn->setFixedSize(80, 80);
        btn->setIconSize(QSize(80, 80));
        btn->setIcon(QIcon(icon));
        btn->setStyleSheet("QAbstractButton {"
                          "    background-color: transparent;"
                          "    border: none;"
                          "    padding: 5px;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: rgba(0, 153, 255, 0.2);"
                          "    border-radius: 5px;"
                          "}");
    };
    // 逐个设置按钮
    setBtnStyle(ui->preBtn, ":/previous.png");
    setBtnStyle(ui->playBtn, ":/bofang.png");
    setBtnStyle(ui->nextBtn, ":/next.png");
    setBtnStyle(ui->modeBtn, ":/shunxubofang.png");
    setBtnStyle(ui->soundBtn, ":/sound.png");
    setBtnStyle(ui->togglelistBtn, ":/playlist.png");

    // 按钮信号连接
    connect(ui->playBtn, &QPushButton::clicked, this, &MainWindow::handlePlaySlot);
    connect(ui->modeBtn, &QPushButton::clicked, this, &MainWindow::handleModeSlot);
    connect(ui->nextBtn, &QPushButton::clicked, this, &MainWindow::handleNextSlot);
    connect(ui->preBtn, &QPushButton::clicked, this, &MainWindow::handlePrevSlot);
    connect(ui->togglelistBtn, &QPushButton::clicked, this, &MainWindow::handleToggleListSlot);
    connect(ui->musicList, &QListWidget::itemClicked, this, &MainWindow::handleMusicListItemClicked);

    connect(ui->soundBtn, &QPushButton::clicked, this, [=]() {
            // 设置弹出滑块位置（在按钮右上角）
            QPoint btnPos = ui->soundBtn->mapToGlobal(QPoint(0, 0));
            m_volumePopup->move(btnPos.x() + ui->soundBtn->width() + 5, btnPos.y());
            m_volumePopup->show();
        });
}

// 设置背景图片
void MainWindow::setBackGround(const QString &filename){
    setStyleSheet(QString("MainWindow { border-image: url(%1) 0 0 0 0 stretch stretch; }").arg(filename));
}

// 时间格式化（毫秒→分:秒）
QString MainWindow::formatTime(qint64 timeMilliSeconds){
    qint64 seconds = timeMilliSeconds / 1000;
    return QString("%1:%2").arg(seconds/60, 2, 10, QChar('0')).arg(seconds%60, 2, 10, QChar('0'));
}

// 加载指定目录音乐
void MainWindow::loadAppointMusicDir(const QString& filepath){
    QDir dir(filepath);
    if(!dir.exists()){
        QMessageBox::warning(this,"文件夹","文件夹打开失败: " + filepath);
        return;
    }

    QStringList supportedFormats = {"mp3", "wav", "flac"};
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);
    for(auto e : fileList){
        if(supportedFormats.contains(e.suffix().toLower())){
            ui->musicList->addItem(e.baseName());
            m_fullFileNameMap[e.baseName()] = e.fileName();
        }
    }

    if(ui->musicList->count() > 0) ui->musicList->setCurrentRow(0);
}


// ------------------------------
// 六、播放相关辅助函数
// ------------------------------

// 开始播放音乐
void MainWindow::startPlayMusic(){
    if (!ui->musicList->currentItem()) return;

    QString musicName = ui->musicList->currentItem()->text();
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到音乐文件：" + musicName);
        return;
    }

    QString Abs = musicDir + fullFileName;
    m_player->setMedia(QUrl::fromLocalFile(Abs));
    m_player->play();
    ui->playBtn->setIcon(QIcon(":/pause.png"));

    // 重置进度与歌词
    m_progressSlider->setValue(0);
    m_currentTimeLabel->setText("00:00");
    m_totalTimeLabel->setText("00:00");
    loadLyricForCurrentSong();
}

// 生成随机播放列表
void MainWindow::shufflePlayList(){
    m_shuffleList.clear();
    int count = ui->musicList->count();
    if (count <= 0) return;

    // 填充索引并打乱
    for (int i = 0; i < count; ++i) m_shuffleList.push_back(i);
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(m_shuffleList[i], m_shuffleList[j]);
    }
    m_shuffleIndex = 0;
}


// ------------------------------
// 七、歌词相关辅助函数
// ------------------------------

// 解析歌词文件（.lrc）
bool MainWindow::parseLyricFile(const QString& filePath){
    m_lyrics.clear();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QRegExp regExp("\\[(\\d+):(\\d+).(\\d+)\\](.*)");  // 匹配[分:秒.毫秒]歌词

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (regExp.indexIn(line) != -1) {
            // 解析时间（转换为毫秒）
            int minute = regExp.cap(1).toInt();
            int second = regExp.cap(2).toInt();
            int msec = regExp.cap(3).toInt() * 10;
            qint64 totalMsec = minute * 60 * 1000 + second * 1000 + msec;

            // 保存歌词
            Lyric lyric;
            lyric.time = totalMsec;
            lyric.content = regExp.cap(4).trimmed();
            if (!lyric.content.isEmpty()) m_lyrics.append(lyric);
        }
    }

    // 按时间排序
    std::sort(m_lyrics.begin(), m_lyrics.end(), [](const Lyric& a, const Lyric& b) { return a.time < b.time; });
    file.close();
    return true;
}

// 加载当前歌曲的歌词
void MainWindow::loadLyricForCurrentSong(){
    qDeleteAll(m_lyricLabels);  // 清除旧歌词
    m_lyricLabels.clear();
    m_lyrics.clear();
    m_currentLyricIndex = -1;

    if (!ui->musicList->currentItem()) return;

    // 获取歌词文件路径（与音乐同目录，同名.lrc）
    QString musicName = ui->musicList->currentItem()->text();
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) return;

    QString musicFilePath = musicDir + fullFileName;
    QString lyricFilePath = musicFilePath.left(musicFilePath.lastIndexOf('.')) + ".lrc";

    // 无歌词文件时显示提示
    if (!QFile::exists(lyricFilePath)) {
        QLabel* noLyricLabel = new QLabel("未找到歌词文件", m_lyricWidget);
        noLyricLabel->setStyleSheet("color: #CCCCCC; font-size: 14px;");
        noLyricLabel->setAlignment(Qt::AlignCenter);
        m_lyricLabels.append(noLyricLabel);
        m_lyricLayout->addWidget(noLyricLabel);
        m_currentLyricFile = "";
        return;
    }

    // 解析并显示歌词
    m_currentLyricFile = lyricFilePath;
    if (parseLyricFile(lyricFilePath)) {
        for (const Lyric& lyric : m_lyrics) {
            QLabel* label = new QLabel(lyric.content, m_lyricWidget);
            label->setStyleSheet("color: #CCCCCC; font-size: 14px;");
            label->setAlignment(Qt::AlignCenter);
            m_lyricLabels.append(label);
            m_lyricLayout->addWidget(label);
        }
        updateLyricDisplay();  // 初始显示
    } else {
        QLabel* errorLabel = new QLabel("歌词文件解析失败", m_lyricWidget);
        errorLabel->setStyleSheet("color: #CCCCCC; font-size: 14px;");
        errorLabel->setAlignment(Qt::AlignCenter);
        m_lyricLabels.append(errorLabel);
        m_lyricLayout->addWidget(errorLabel);
    }
}

// 更新歌词显示（高亮当前句+滚动）
void MainWindow::updateLyricDisplay(){
    if (m_lyricLabels.isEmpty() || m_currentLyricIndex < 0) return;

    // 逐个设置歌词样式（当前句高亮）
    for (int i = 0; i < m_lyricLabels.size(); ++i) {
        if (i == m_currentLyricIndex) {
            // 当前歌词：白色加粗+放大+阴影
            m_lyricLabels[i]->setStyleSheet(
                "color: #FFFFFF; font-size: 20px; font-weight: bold;"
                "text-shadow: 0 0 10px rgba(255, 255, 255, 0.8);"
            );
            // 缩放动画
            QPropertyAnimation* animation = new QPropertyAnimation(m_lyricLabels[i], "geometry");
            animation->setDuration(300);
            animation->setStartValue(m_lyricLabels[i]->geometry());
            QRect endRect = m_lyricLabels[i]->geometry();
            endRect.setWidth(endRect.width() * 1.1);
            endRect.setHeight(endRect.height() * 1.1);
            endRect.moveLeft(endRect.left() - endRect.width() * 0.05);
            endRect.moveTop(endRect.top() - endRect.height() * 0.05);
            animation->setEndValue(endRect);
            animation->setEasingCurve(QEasingCurve::OutQuad);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        } else if (qAbs(i - m_currentLyricIndex) <= 1) {
            // 相邻歌词：浅灰+稍大字体
            m_lyricLabels[i]->setStyleSheet("color: #DDDDDD; font-size: 16px;");
        } else {
            // 其他歌词：深灰+小字体
            m_lyricLabels[i]->setStyleSheet("color: #888888; font-size: 14px;");
        }
    }

    // 滚动到当前歌词（居中显示）
    if (m_currentLyricIndex < m_lyricLabels.size()) {
        m_lyricWidget->scroll(0, m_lyricLabels[m_currentLyricIndex]->y() -
                            m_lyricWidget->height()/2 +
                            m_lyricLabels[m_currentLyricIndex]->height()/2);
    }
}


// 添加新的槽函数
void MainWindow::onVolumeSliderValueChanged(int value) {
    m_player->setVolume(value);  // 设置播放器音量
    // 检查音量是否为0
    if (value == 0) {
        ui->soundBtn->setIcon(QIcon(":/jingyin.png"));  // 设置为静音图标
    } else {
        ui->soundBtn->setIcon(QIcon(":/sound.png"));  // 设置为音量图标
    }
}

// 添加辅助函数更新歌词组件高度
void MainWindow::updateLyricHeight() {
    int availableHeight = this->contentsRect().height();
    int controlHeight = 200; // 控制区固定高度

    // 计算歌词区域高度（窗口高度 - 控制区高度 - 工具栏高度 - 其他组件高度）
    // 这里简化为窗口高度减去控制区高度
    int lyricHeight = availableHeight - controlHeight;

    // 添加安全限制，避免高度过小或过大
    lyricHeight = qMax(lyricHeight, 300); // 最小高度
    lyricHeight = qMin(lyricHeight, 1500); // 最大高度

    m_lyricWidget->setFixedHeight(lyricHeight);
}


// ------------------------------
// 八、事件处理函数
// ------------------------------

// 窗口大小改变事件
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateLyricHeight();
}
