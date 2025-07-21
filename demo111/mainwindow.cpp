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
#include <algorithm>
#include <QPropertyAnimation>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_mode(LIST_MODE),
    ui(new Ui::MainWindow),
    m_shuffleIndex(-1),
    m_isManualSwitch(false),
    m_sliderPressed(false),
    m_currentLyricIndex(-1)
{
    ui->setupUi(this);
    setWindowTitle("音乐播放器");

    m_player = new QMediaPlayer(this);

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
    listLayout->addWidget(ui->musicList);

    QWidget *playerWidget = new QWidget(this);
    QVBoxLayout *playerLayout = new QVBoxLayout(playerWidget);
    playerLayout->setContentsMargins(10, 10, 10, 10);
    playerLayout->setSpacing(10);

    // 歌词显示部件
    m_lyricWidget = new QWidget(this);
    m_lyricWidget->setMinimumHeight(200);
    m_lyricWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0.3); border-radius: 5px;");
    m_lyricLayout = new QVBoxLayout(m_lyricWidget);
    m_lyricLayout->setAlignment(Qt::AlignCenter);
    m_lyricLayout->setSpacing(10);
    playerLayout->addWidget(m_lyricWidget);

    // 进度条布局
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

    // 保存UI控件指针
    m_progressSlider = ui->progressSlider;
    m_currentTimeLabel = ui->currentTimeLabel;
    m_totalTimeLabel = ui->totalTimeLabel;

    // 连接播放器信号
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::updateProgress);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(m_progressSlider, &QSlider::sliderPressed, this, &MainWindow::handleProgressSliderPressed);
    connect(m_progressSlider, &QSlider::sliderReleased, this, &MainWindow::handleProgressSliderReleased);

    // 按钮容器布局
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

    QWidget *buttonContainer2 = new QWidget(this);
    QVBoxLayout *buttonLayout2 = new QVBoxLayout(buttonContainer2);
    buttonLayout2->addWidget(ui->addBtn);
    buttonLayout2->addStretch();
    buttonLayout2->addWidget(ui->themeBtn);
    buttonLayout2->addStretch();
    playerLayout->addWidget(buttonContainer2);

    m_listWidget = listWidget;

    playerLayout->addStretch();
    playerLayout->addLayout(progressLayout);
    playerLayout->addWidget(buttonContainer);
    splitter->addWidget(listWidget);
    splitter->addWidget(playerWidget);
    splitter->setSizes(QList<int>() << 200 << 400);
    mainLayout->addWidget(splitter);
    setBackGround(":/res/bcakground.jpg");
    initButtons();

    srand(time(NULL));

    // 播放结束自动切歌
    connect(m_player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState
            && ui->musicList->count() > 0
            && !m_isManualSwitch) {
            handleNextSlot();
        }
    });
}

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
    connect(ui->musicList, &QListWidget::itemClicked, this, &MainWindow::handleMusicListItemClicked);
}

void MainWindow::handlePlaySlot(){
    if (ui->musicList->count() == 0) return;

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

void MainWindow::handleMusicListItemClicked(QListWidgetItem *item)
{
    m_isManualSwitch = true;
    int row = ui->musicList->row(item);

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

    QString Abs = musicDir + fullFileName;
    qDebug() << "播放路径：" << Abs;
    m_player->setMedia(QUrl::fromLocalFile(Abs));
    m_player->play();
    ui->playBtn->setIcon(QIcon(":/res/pause.png"));
    loadLyricForCurrentSong();

    m_isManualSwitch = false;
}

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
            if (suffix == "mp3" || suffix == "wav" || suffix == "flac") {
                QString musicName = fileInfo.baseName();
                QString fullName = fileInfo.fileName();

                bool exists = false;
                for (int i = 0; i < ui->musicList->count(); ++i) {
                    if (ui->musicList->item(i)->text() == musicName) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    ui->musicList->addItem(musicName);
                    m_fullFileNameMap[musicName] = fullName;
                }
            }
        }

        if (ui->musicList->count() > 0 && m_player->mediaStatus() == QMediaPlayer::NoMedia) {
            ui->musicList->setCurrentRow(0);
            startPlayMusic();
        }
    }
}

void MainWindow::handleChangeBackgroundSlot(){
    QString filePath = QFileDialog::getOpenFileName(
        this, "选择背景图片", QDir::homePath(),
        "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif);;所有文件 (*)"
    );

    if (filePath.isEmpty()) {
        qDebug() << "用户取消了背景图片选择";
        return;
    }

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        QMessageBox::warning(this, "错误", "选择的文件不存在或无效");
        return;
    }

    setBackGround(filePath);
}

void MainWindow::handleToggleListSlot(){
    if (m_listWidget->isVisible()) {
        m_listWidget->hide();
    } else {
        m_listWidget->show();
    }
}

void MainWindow::startPlayMusic() {
    if (!ui->musicList->currentItem()) return;

    QString musicName = ui->musicList->currentItem()->text();
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到音乐文件：" + musicName);
        return;
    }

    QString Abs = musicDir + fullFileName;
    qDebug() << "播放路径：" << Abs;
    m_player->setMedia(QUrl::fromLocalFile(Abs));
    m_player->play();
    ui->playBtn->setIcon(QIcon(":/res/pause.png"));

    m_progressSlider->setValue(0);
    m_currentTimeLabel->setText("00:00");
    m_totalTimeLabel->setText("00:00");
    loadLyricForCurrentSong();
}

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

    if(ui->musicList->count() > 0){
        ui->musicList->setCurrentRow(0);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::setBackGround(const QString &filename){
    setStyleSheet(QString("MainWindow { border-image: url(%1) 0 0 0 0 stretch stretch; }")
                      .arg(filename));
}

QString MainWindow::formatTime(qint64 timeMilliSeconds) {
    qint64 seconds = timeMilliSeconds / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

// 歌词相关实现
bool MainWindow::parseLyricFile(const QString& filePath) {
    m_lyrics.clear();
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QRegExp regExp("\\[(\\d+):(\\d+).(\\d+)\\](.*)");
        if (regExp.indexIn(line) != -1) {
            int minute = regExp.cap(1).toInt();
            int second = regExp.cap(2).toInt();
            int msec = regExp.cap(3).toInt() * 10;
            qint64 totalMsec = minute * 60 * 1000 + second * 1000 + msec;

            Lyric lyric;
            lyric.time = totalMsec;
            lyric.content = regExp.cap(4).trimmed();

            if (!lyric.content.isEmpty()) {
                m_lyrics.append(lyric);
            }
        }
    }

    std::sort(m_lyrics.begin(), m_lyrics.end(),
              [](const Lyric& a, const Lyric& b) {
                  return a.time < b.time;
              });

    file.close();
    return true;
}

void MainWindow::loadLyricForCurrentSong() {
    qDeleteAll(m_lyricLabels);
    m_lyricLabels.clear();
    m_lyrics.clear();
    m_currentLyricIndex = -1;

    if (!ui->musicList->currentItem()) return;

    QString musicName = ui->musicList->currentItem()->text();
    QString fullFileName = m_fullFileNameMap.value(musicName);
    if (fullFileName.isEmpty()) return;

    QString musicFilePath = musicDir + fullFileName;
    QString lyricFilePath = musicFilePath.left(musicFilePath.lastIndexOf('.')) + ".lrc";

    if (!QFile::exists(lyricFilePath)) {
        QLabel* noLyricLabel = new QLabel("未找到歌词文件", m_lyricWidget);
        noLyricLabel->setStyleSheet("color: #CCCCCC; font-size: 14px;");
        noLyricLabel->setAlignment(Qt::AlignCenter);
        m_lyricLabels.append(noLyricLabel);
        m_lyricLayout->addWidget(noLyricLabel);
        m_currentLyricFile = "";
        return;
    }

    m_currentLyricFile = lyricFilePath;

    if (parseLyricFile(lyricFilePath)) {
        for (const Lyric& lyric : m_lyrics) {
            QLabel* label = new QLabel(lyric.content, m_lyricWidget);
            label->setStyleSheet("color: #CCCCCC; font-size: 14px;");
            label->setAlignment(Qt::AlignCenter);
            m_lyricLabels.append(label);
            m_lyricLayout->addWidget(label);
        }
        updateLyricDisplay();
    } else {
        QLabel* errorLabel = new QLabel("歌词文件解析失败", m_lyricWidget);
        errorLabel->setStyleSheet("color: #CCCCCC; font-size: 14px;");
        errorLabel->setAlignment(Qt::AlignCenter);
        m_lyricLabels.append(errorLabel);
        m_lyricLayout->addWidget(errorLabel);
    }
}

void MainWindow::updateLyricDisplay() {
    if (m_lyricLabels.isEmpty() || m_currentLyricIndex < 0) return;

    for (int i = 0; i < m_lyricLabels.size(); ++i) {
        if (i == m_currentLyricIndex) {
            m_lyricLabels[i]->setStyleSheet(
                "color: #FFFFFF; font-size: 20px; font-weight: bold;"
                "text-shadow: 0 0 10px rgba(255, 255, 255, 0.8);"
            );

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
            m_lyricLabels[i]->setStyleSheet(
                "color: #DDDDDD; font-size: 16px;"
            );
        } else {
            m_lyricLabels[i]->setStyleSheet(
                "color: #888888; font-size: 14px;"
            );
        }
    }

    if (m_currentLyricIndex < m_lyricLabels.size()) {
        m_lyricWidget->scroll(0, m_lyricLabels[m_currentLyricIndex]->y() -
                            m_lyricWidget->height()/2 +
                            m_lyricLabels[m_currentLyricIndex]->height()/2);
    }
}

void MainWindow::updateProgress(qint64 position) {
    if (!m_sliderPressed && m_player->duration() > 0) {
        m_progressSlider->setValue(position);
        m_currentTimeLabel->setText(formatTime(position));
    }

    if (!m_lyrics.isEmpty()) {
        int newIndex = -1;
        for (int i = 0; i < m_lyrics.size(); ++i) {
            if (m_lyrics[i].time <= position) {
                newIndex = i;
            } else {
                break;
            }
        }

        if (newIndex != -1 && newIndex != m_currentLyricIndex) {
            m_currentLyricIndex = newIndex;
            updateLyricDisplay();
        }
    }
}

void MainWindow::updateDuration(qint64 duration) {
    m_progressSlider->setRange(0, duration);
    m_totalTimeLabel->setText(formatTime(duration));
}

void MainWindow::handleProgressSliderPressed() {
    m_sliderPressed = true;
}

void MainWindow::handleProgressSliderReleased() {
    m_sliderPressed = false;
    setPosition(m_progressSlider->value());
}

void MainWindow::setPosition(int position) {
    if (m_sliderPressed) {
        m_player->setPosition(position);
        m_currentTimeLabel->setText(formatTime(position));
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
