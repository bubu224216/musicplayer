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
#include<QMediaMetaData>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QAction>


// 构造函数：按"基础初始化→组件创建→布局组装→信号连接"分步实现
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_mode(LIST_MODE),
    m_nightmode(true),
    ui(new Ui::MainWindow),
    m_shuffleIndex(-1),
    m_isManualSwitch(false),
    m_sliderPressed(false),
    m_currentLyricIndex(-1)
{
    // 第一步：基础初始化（窗口属性、核心组件）
    ui->setupUi(this);
    resize(1344, 800);
    setWindowTitle("音乐播放器");
    m_player = new QMediaPlayer(this);  // 播放器核心组件

    setWindowIcon(QIcon(":/pause.png"));

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
    SwitchModeAction = new QAction(QIcon(":/night.png"), "夜间模式", this);
    deleteMusicAction = new QAction(QIcon(":/delete.png"), "删除音乐", this);
    addMusicAction = new QAction(QIcon(":/add_to.png"), "添加音乐", this);
    changeThemeAction = new QAction(QIcon(":/theme.png"), "更换主题", this);
    toolBar->addAction(SwitchModeAction);
    toolBar->addAction(deleteMusicAction);
    toolBar->addAction(addMusicAction);
    toolBar->addAction(changeThemeAction);
    // 工具栏信号连接（提前连接，避免分散）
    connect(SwitchModeAction, &QAction::triggered, this, &MainWindow::handleSwitchModeSlot);
    connect(deleteMusicAction, &QAction::triggered, this, &MainWindow::handleDeleteMusicSlot);
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
                                        "    border: none;"
                                        "    outline: none;"
                                        "    color: #6495ed;"
                                        "}"
                                     "QListWidget::item:selected {"
                                     "    background: transparent;"
                                     "    color: #f8f8ff;"
                                     "    outline: none;"   // 去掉焦点框
                                     "}"
                                     "QListWidget::item:focus {"
                                         "    outline: none;"   // 去掉焦点框
                                         "}"
                                     );
    ui->musicList->setFocusPolicy(Qt::NoFocus);
    QFont font("微软雅黑", 15);
    font.setWeight(QFont::Normal);
    ui->musicList->setFont(font);
    listLayout->addWidget(ui->musicList);
    ui->musicList->setGridSize(QSize(0, 60));  // 60像素行高，可根据需要调整

    // 允许列表多选（按住Ctrl或Shift键选择多个项）
    ui->musicList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_listWidget = listWidget;  // 保存列表容器引用



    // 下方播放控制区（水平布局）
    QWidget *playerWidget = new QWidget(this);
    QHBoxLayout *playerLayout = new QHBoxLayout(playerWidget);
    playerLayout->setContentsMargins(10, 10, 10, 10);
    playerLayout->setSpacing(10);

    //上方音乐区
    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    topLayout->addWidget(listWidget);  // 左侧音乐列表

    mainLayout->addWidget(topWidget); // 添加上半部分（列表）

    mainLayout->addWidget(playerWidget);   // 播放区统一置底


    // 5.1 歌词显示部件
    m_lyricWidget = new QWidget(this);

    m_lyricWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_lyricWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0); border-radius: 5px;");
    m_lyricLayout = new QVBoxLayout(m_lyricWidget);
    m_lyricLayout->setAlignment(Qt::AlignCenter);
    m_lyricLayout->setSpacing(1);

    m_coverWidget = new QWidget(this);  // 显式创建实例
    m_coverLayout = new QVBoxLayout(m_coverWidget);  // 为其设置唯一布局
    m_coverLayout->setAlignment(Qt::AlignCenter);
    m_mainlyricWidget = new QWidget(this);
    m_mainlyricLayout = new QVBoxLayout(m_mainlyricWidget);
    m_mainlyricLayout->setAlignment(Qt::AlignCenter);


    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, &MainWindow::onMediaStatusChanged);


    topLayout->addWidget(m_mainlyricWidget);  // 添加到右侧布局
    //让列表和歌曲按比例分配
    topLayout->setStretch(0, 1);  // 第0个控件（musicList）
    topLayout->setStretch(1, 2);  // 第1个控件（lyrics）



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
    //随黑暗模式改变时间标签的颜色
    m_currentTimeLabel->setStyleSheet("font-family: 微软雅黑; font-size: 12pt; font-weight: bold; color: #ffffff;");
    m_totalTimeLabel->setStyleSheet("font-family: 微软雅黑; font-size: 12pt; font-weight: bold; color: #ffffff;");
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
            background-color: rgba(255, 255, 255, 0);
            border-radius: 15px;
            border: none;
        }
    )");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setContentsMargins(15, 10, 15, 10);
    controlLayout->setSpacing(10);
    controlLayout->addLayout(progressLayout);  // 进度条
    controlLayout->addWidget(buttonContainer);  // 按钮区
    // 添加到右侧布局（歌词区下方）
    playerLayout->addWidget(controlPanel);



    setBackGround(":/background_day.png");  // 设置背景


    // 第七步：初始化与信号连接
    initButtons();  // 按钮样式与信号
    initCoverDisplay();
    initSystemTray();
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


    // 最后加载保存的播放列表
    //获取上一次的歌单信息
    loadPlayList();
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

    if(m_nightmode==true){
        if (m_player->state() == QMediaPlayer::PlayingState) {
                m_player->pause();
                ui->playBtn->setIcon(QIcon(":/bofang.png"));
          } else {
                m_player->play();
                loadAlbumCover();
                ui->playBtn->setIcon(QIcon(":/pause.png"));
            }
      }
    else  if(m_nightmode==false){
        if (m_player->state() == QMediaPlayer::PlayingState) {
                m_player->pause();
                ui->playBtn->setIcon(QIcon(":/play_daytime.png"));
          } else {
                m_player->play();
                loadAlbumCover();
                ui->playBtn->setIcon(QIcon(":/pause_day.png"));
            }
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
    loadAlbumCover();
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
    loadAlbumCover();
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
    loadAlbumCover();
    if(m_nightmode == true){
        ui->playBtn->setIcon(QIcon(":/pause.png"));
    }
    else if(m_nightmode == false){
        ui->playBtn->setIcon(QIcon(":/pause_day.png"));
    }
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

    if(m_nightmode == true){
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
    else if(m_nightmode == false){
        switch(m_mode) {
            case LIST_MODE:
                ui->modeBtn->setIcon(QIcon(":/shunxvbofang_day.png"));
                qDebug() << "列表循环模式";
                break;
            case RANDOM_MODE:
                ui->modeBtn->setIcon(QIcon(":/24gl-shuffle.png"));
                qDebug() << "随机播放模式";
                break;
            case SINGLE_MODE:
                ui->modeBtn->setIcon(QIcon(":/24gl-repeatOnce2.png"));
                qDebug() << "单曲循环模式";
                break;
        }
    }
}
//黑暗模式切换
void MainWindow::handleSwitchModeSlot(){
    m_nightmode = !m_nightmode;
    if(m_nightmode == false){
        SwitchModeAction->setIcon(QIcon(":/daytime.png"));  // 改变为另一个图标资源
        SwitchModeAction->setText("日间模式");
        deleteMusicAction->setIcon(QIcon(":/delete_day.png"));  // 改变为另一个图标资源
        addMusicAction->setIcon(QIcon(":/add_day.png"));  // 改变为另一个图标资源
        changeThemeAction->setIcon(QIcon(":/theme_day.png"));  // 改变为另一个图标资源
        setBackGround(":/background_night.png");
        setBtnStyle(ui->preBtn, ":/previous_day.png");
        setBtnStyle(ui->nextBtn, ":/next_day.png");
        setBtnStyle(ui->modeBtn, ":/shunxvbofang_day.png");
        setBtnStyle(ui->soundBtn, ":/sound_day.png");
        setBtnStyle(ui->togglelistBtn, ":/playerlist_day.png");
        //判断是否在播放
        if(m_player->state() == QMediaPlayer::PlayingState){
            setBtnStyle(ui->playBtn,":pause_day.png");
        }else{
            setBtnStyle(ui->playBtn,":/play_daytime.png");
        }



    }
    else if(m_nightmode == true){
        SwitchModeAction->setIcon(QIcon(":/night.png"));  // 改变为另一个图标资源
        SwitchModeAction->setText("夜间模式");
        setBackGround(":/background_day.png");
        deleteMusicAction->setIcon(QIcon(":/delete.png"));  // 改变为另一个图标资源
        addMusicAction->setIcon(QIcon(":/add_to.png"));  // 改变为另一个图标资源
        changeThemeAction->setIcon(QIcon(":/theme.png"));  // 改变为另一个图标资源
        setBtnStyle(ui->preBtn, ":/previous.png");
        setBtnStyle(ui->nextBtn, ":/next.png");
        setBtnStyle(ui->modeBtn, ":/shunxubofang.png");
        setBtnStyle(ui->soundBtn, ":/sound.png");
        setBtnStyle(ui->togglelistBtn, ":/playerlist.png");
        //判断是否在播放
        if(m_player->state() == QMediaPlayer::PlayingState){
            setBtnStyle(ui->playBtn,":pause.png");
        }else{
            setBtnStyle(ui->playBtn,":/bofang.png");
        }

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

// 批量删除选中的音乐
void MainWindow::handleDeleteMusicSlot() {
    // 获取所有选中的项
    QList<QListWidgetItem*> selectedItems = ui->musicList->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选中要删除的音乐（可按住Ctrl/Shift多选）");
        return;
    }

    // 确认删除（显示选中的数量）
    int ret = QMessageBox::question(
        this, "确认删除",
        QString("确定要从列表中删除选中的 %1 首音乐吗？\n（此操作不会删除本地文件）")
            .arg(selectedItems.size()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No
    );
    if (ret != QMessageBox::Yes) return;

    // 记录正在播放的音乐（用于后续处理）
    QString playingMusic;
    bool isPlaying = (m_player->state() == QMediaPlayer::PlayingState ||
                     m_player->state() == QMediaPlayer::PausedState);
    if (isPlaying && ui->musicList->currentItem()) {
        playingMusic = ui->musicList->currentItem()->text();
    }

    // 批量删除选中项
    for (QListWidgetItem* item : selectedItems) {
        // 从映射表中删除
        m_fullFileNameMap.remove(item->text());
        // 从列表中删除（会自动释放内存）
        delete item;
    }

    // 处理正在播放的音乐被删除的情况
    if (isPlaying && !m_fullFileNameMap.contains(playingMusic)) {
        m_player->stop();
        ui->playBtn->setIcon(QIcon(":/bofang.png"));

        // 如果还有音乐，自动播放第一项
        if (ui->musicList->count() > 0) {
            ui->musicList->setCurrentRow(0);
            startPlayMusic();
        }
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
void MainWindow::setBtnStyle (QAbstractButton* btn, const QString& icon) {
    // 按钮样式设置（lambda复用）
    btn->setFixedSize(80, 80);
    btn->setIconSize(QSize(80, 80));
    btn->setIcon(QIcon(icon));
    btn->setStyleSheet("QAbstractButton {"
                      "    background-color: transparent;"
                      "    border: none;"
                      "    padding: 5px;"
                      "}"
                      "QAbstractButton:hover {"
                      "    background-color: rgba(0, 153, 255, 0.2);"
                      "    border-radius: 5px;"
                      "}");
};
void MainWindow::initButtons(){
    // 逐个设置按钮
    setBtnStyle(ui->preBtn, ":/previous.png");
    setBtnStyle(ui->playBtn, ":/bofang.png");
    setBtnStyle(ui->nextBtn, ":/next.png");
    setBtnStyle(ui->modeBtn, ":/shunxubofang.png");
    setBtnStyle(ui->soundBtn, ":/sound.png");
    setBtnStyle(ui->togglelistBtn, ":/playerlist.png");


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
    loadAlbumCover();
    if(m_nightmode == true){
        ui->playBtn->setIcon(QIcon(":/pause.png"));
    }
    else if(m_nightmode == false){
        ui->playBtn->setIcon(QIcon(":/pause_day.png"));
    }

    // 重置进度与歌词
    m_progressSlider->setValue(0);
    m_currentTimeLabel->setText("00:00");
    m_totalTimeLabel->setText("00:00");
    loadLyricForCurrentSong();

    QString musicName1 = ui->musicList->currentItem()->text();
    showDesktopNotification("正在播放", musicName1);
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
    in.setCodec("GBK");
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
void MainWindow::loadLyricForCurrentSong() {

    QFont baseFont("楷体", 14); // 设置默认字体和大小
    baseFont.setItalic(false);
    baseFont.setBold(false);

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
//    if (!QFile::exists(lyricFilePath))


    // 解析并显示歌词
    m_currentLyricFile = lyricFilePath;
    if (parseLyricFile(lyricFilePath)) {
        for (const Lyric& lyric : m_lyrics) {
            QLabel* label = new QLabel(lyric.content, m_mainlyricWidget);
            label->setFont(baseFont);
            // 设置透明背景（关键修改）
            label->setStyleSheet("color: #CCCCCC; background-color: transparent; border: none;");
            label->setAlignment(Qt::AlignCenter);
            m_lyricLabels.append(label);
            m_lyricLayout->addWidget(label);
            label->hide(); // 初始隐藏
        }
        updateLyricDisplay();  // 初始显示
    }
}

// 更新歌词显示（高亮当前句+滚动）
void MainWindow::updateLyricDisplay(){
    if (m_lyricLabels.isEmpty() || m_currentLyricIndex < 0) return;

    // 隐藏所有歌词标签
        for (QLabel* label : m_lyricLabels) {
            label->hide();
        }

        // 显示当前歌词及其前后各一到两句歌词
        int startIndex = qMax(0, m_currentLyricIndex - 1);
        int endIndex = qMin(m_lyricLabels.size() - 1, m_currentLyricIndex + 1);


        for (int i = startIndex; i <= endIndex; ++i) {
            if (i < m_lyricLabels.size()) {
                m_lyricLabels[i]->setStyleSheet(
                    "color: #FFFFFF; font-size: 50px; font-weight: bold;"
                    "background-color: transparent; border: none;"
                );
                m_lyricLabels[i]->show();
                if (i == m_currentLyricIndex) {
                    // 当前歌词：白色加粗+放大+阴影
                    m_lyricLabels[i]->setStyleSheet(
                        "color: #90ee90; font-size: 50px; font-weight: bold;"
                        "text-shadow: 0 0 10px rgba(0, 255, 0, 0.8);"  // 绿色阴影
                        "background-color: transparent; /* 透明背景 */"
                        "border: none;"
                    );
                } else {
                    // 其他歌词：浅灰+稍大字体
                    m_lyricLabels[i]->setStyleSheet("color: #dcdcdc; font-size: 30px;""background-color: transparent; /* 透明背景 */"
                                                    "border: none;");
                }
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
    if(m_nightmode == true){
        if (value == 0) {
            ui->soundBtn->setIcon(QIcon(":/jingyin.png"));  // 设置为静音图标
        } else {
            ui->soundBtn->setIcon(QIcon(":/sound.png"));  // 设置为音量图标
        }
     }
    else{
        if (value == 0) {
            ui->soundBtn->setIcon(QIcon(":/silence_day.png"));  // 设置为静音图标
        } else {
            ui->soundBtn->setIcon(QIcon(":/sound_day.png"));  // 设置为音量图标
        }
    }
}


// 初始化封面显示组件
void MainWindow::initCoverDisplay() {
    // 创建封面标签
    m_coverLabel = new QLabel(this);
    m_coverLabel->setFixedSize(500,500);  // 设置封面大小
    m_coverLabel->setStyleSheet("border: 2px solid rgba(255, 255, 255, 0.3); "
                               "border-radius: 8px; "
                               "border: none;"
                               "background-color: rgba(0, 0, 0, 0);");
    m_coverLabel->setAlignment(Qt::AlignCenter);

    // 加载默认封面（没有找到封面时显示）
    m_defaultCover = QPixmap(":/bk6.jpg");
    if (m_defaultCover.isNull()) {
        // 如果默认封面加载失败，创建一个简单的占位图
        m_defaultCover = QPixmap(300, 300);
        m_defaultCover.fill(QColor(50, 50, 50));
    }

    // 设置初始显示默认封面
    setCoverImage(m_defaultCover);

    // 将封面标签添加到歌词布局（替换原来的静态封面）
    // 找到之前添加封面的位置，替换为：
    m_coverLayout->addWidget(m_coverLabel);
    m_mainlyricLayout->addWidget(m_coverWidget);
    m_mainlyricLayout->addWidget(m_lyricWidget);
}

// 媒体状态变化时处理
void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    // 当媒体加载完成后，提取封面
    if (status == QMediaPlayer::LoadedMedia) {
        loadAlbumCover();
    }
}

// 获取内嵌的专辑封面
// 修改 getEmbeddedCover() 函数，移除对 CoverArt 的使用
QPixmap MainWindow::getEmbeddedCover() {
    // 检查元数据是否可用
    if (!m_player->isMetaDataAvailable()) {
        return QPixmap();
    }

    // 尝试获取封面图片（使用 Qt 明确支持的元数据键）
    QVariant coverData = m_player->metaData(QMediaMetaData::CoverArtImage);
    if (coverData.isValid() && coverData.canConvert<QPixmap>()) {
        return coverData.value<QPixmap>();
    }

    // 尝试获取缩略图作为备选
    coverData = m_player->metaData(QMediaMetaData::ThumbnailImage);
    if (coverData.isValid() && coverData.canConvert<QPixmap>()) {
        return coverData.value<QPixmap>();
    }

    // 没有找到内嵌封面
    return QPixmap();
}

QPixmap MainWindow::findLocalCoverFile() {
    // 检查是否有选中的音乐项
    if (!ui->musicList->currentItem()) return QPixmap();

    // 获取当前歌曲信息
    QString musicName = ui->musicList->currentItem()->text();
    qDebug()<<musicName;
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) return QPixmap();

    // 拼接完整音乐文件路径
    QString musicFilePath = musicDir + fullFileName;
    QFileInfo fileInfo(musicFilePath);

    // 提取关键信息：歌曲所在目录 + 歌曲名（不含扩展名）
    QString dirPath = fileInfo.absolutePath(); // 例如：D:/Music/
    QString songBaseName = fileInfo.baseName(); // 例如：周杰伦-还在流浪（不含.mp3）

    // 拼接封面文件路径：目录 + 歌曲名_cov.jpg
    QString coverPath = dirPath + "/" + songBaseName + "_cov.jpg";
    qDebug()<<coverPath;
    // 最终路径示例：D:/Music/周杰伦-还在流浪_cov.jpg

    // 检查文件是否存在并加载
    if (QFile::exists(coverPath)) {


        QPixmap cover(coverPath);
        if (!cover.isNull()) {
            qDebug()<<"图片不是空";
            return cover; // 找到并返回封面
        }
    }

    // 未找到返回空
    return QPixmap();
}

// 设置封面图片（保持比例缩放）
void MainWindow::setCoverImage(const QPixmap &pixmap) {
     qDebug()<<"进来了函数！！！！";
    if (!pixmap.isNull()) {
        // 保持比例缩放图片以适应标签大小
        qDebug()<<"进来了if！！！！";
        QPixmap scaledPixmap = pixmap.scaled(
            m_coverLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        m_coverLabel->setPixmap(scaledPixmap);
        m_coverLabel->setAlignment(Qt::AlignCenter);
        qDebug()<<"实现了这一步(设置封面图片)";
    }
}

// 加载专辑封面的主函数
void MainWindow::loadAlbumCover() {
    // 1. 先尝试从音频文件内嵌元数据中获取封面
    QPixmap cover = getEmbeddedCover();

    // 2. 如果没有内嵌封面，尝试在歌曲所在目录查找
    if (cover.isNull()) {
        qDebug()<<"使用了本地值";
        cover = findLocalCoverFile();

    }

    //3. 如果都没有找到，使用默认封面
    else  {
        qDebug()<<"使用了默认值";
        cover = m_defaultCover;
    }

    // 显示封面
    setCoverImage(cover);
    qDebug()<<"在setCoverImage之后的代码";
}

void MainWindow::initSystemTray() {
    // 创建系统托盘图标
    m_systemTray = new QSystemTrayIcon(this);
    m_systemTray->setIcon(QIcon(":/bofang.png")); // 设置托盘图标
    m_systemTray->setToolTip("音乐播放器"); // 鼠标悬停提示

    // 创建右键菜单
    m_trayMenu = new QMenu(this);

    // 添加菜单项
    QAction *playAction = new QAction("播放/暂停", this);
    QAction *nextAction = new QAction("下一首", this);
    QAction *prevAction = new QAction("上一首", this);
    QAction *showAction = new QAction("显示窗口", this);
    QAction *quitAction = new QAction("退出", this);

    // 关联信号槽
    connect(playAction, &QAction::triggered, this, &MainWindow::handlePlaySlot);
    connect(nextAction, &QAction::triggered, this, &MainWindow::handleNextSlot);
    connect(prevAction, &QAction::triggered, this, &MainWindow::handlePrevSlot);
    connect(showAction, &QAction::triggered, this, &MainWindow::showNormal);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    // 添加到菜单并设置给托盘
    m_trayMenu->addAction(playAction);
    m_trayMenu->addAction(nextAction);
    m_trayMenu->addAction(prevAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(showAction);
    m_trayMenu->addAction(quitAction);

    m_systemTray->setContextMenu(m_trayMenu);

    // 点击托盘图标显示/隐藏窗口
    connect(m_systemTray, &QSystemTrayIcon::activated,
            this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            if (isVisible()) hide();
            else showNormal();
        }
    });

    // 显示托盘图标
    m_systemTray->show();
}

// 添加发送通知的函数
void MainWindow::showDesktopNotification(const QString &title, const QString &message) {
    // 简单通知（Qt原生方法，在不同平台表现可能不同）
    m_systemTray->showMessage(
        title,
        message,
        QIcon(":/icon.png"),
        1000 // 显示2秒
    );
}

// 保存播放列表时，同时保存 musicDir
void MainWindow::savePlayList() {
    if (ui->musicList->count() == 0) return;

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    QString savePath = appDataPath + "/playlist.json";

    // 构建JSON对象（包含musicDir和音乐列表）
    QJsonObject rootObj;
    rootObj["musicDir"] = musicDir; // 保存当前固定的musicDir

    QJsonArray musicArray;
    for (int i = 0; i < ui->musicList->count(); ++i) {
        QString musicName = ui->musicList->item(i)->text();
        QString fullFileName = m_fullFileNameMap.value(musicName);
        QJsonObject musicObj;
        musicObj["name"] = musicName;
        musicObj["fileName"] = fullFileName; // 只存文件名（不含路径）
        musicArray.append(musicObj);
    }
    rootObj["musicList"] = musicArray;

    // 保存到文件
    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(rootObj);
        file.write(doc.toJson());
        file.close();
    }
}

// 加载播放列表时，恢复保存的musicDir
void MainWindow::loadPlayList() {
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString savePath = appDataPath + "/playlist.json";

    QFile file(savePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) return;
    QJsonObject rootObj = doc.object();

    // 关键：恢复保存的musicDir（固定不变）
    musicDir = rootObj["musicDir"].toString();
    if (musicDir.isEmpty()) return; // 如果没有保存的路径，直接返回

    QJsonArray musicArray = rootObj["musicList"].toArray();

    ui->musicList->clear();
    m_fullFileNameMap.clear();

    // 加载列表数据（使用保存的musicDir）
    for (int i = 0; i < musicArray.size(); ++i) {
        QJsonObject musicObj = musicArray[i].toObject();
        QString musicName = musicObj["name"].toString();
        QString fullFileName = musicObj["fileName"].toString(); // 仅文件名

        if (musicName.isEmpty() || fullFileName.isEmpty()) continue;

        // 检查文件是否存在（确保在固定的musicDir下）
        QString fullPath = musicDir + fullFileName;
        if (!QFile::exists(fullPath)) {
            qDebug() << "文件不存在：" << fullPath;
            continue;
        }

        ui->musicList->addItem(musicName);
        m_fullFileNameMap[musicName] = fullFileName; // 只存文件名
    }

    if (ui->musicList->count() > 0) {
        ui->musicList->setCurrentRow(0);
    }
}


// ------------------------------
// 八、事件处理函数
// ------------------------------

// 窗口大小改变事件
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

}

// 重写closeEvent，实现关闭时最小化到托盘
void MainWindow::closeEvent(QCloseEvent *event) {

    // 关闭前保存播放列表
    //保存上一次的歌单内容
    savePlayList();

    if (m_systemTray->isVisible()) {
        hide(); // 隐藏窗口而不是关闭
        event->ignore(); // 忽略关闭事件
    }
}
