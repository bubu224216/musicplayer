#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
#include <vector>
#include <QFileDialog>
#include <QSlider>
#include <QLabel>
#include <QListWidgetItem>
#include <QMap>
#include <QPropertyAnimation>
#include<QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 播放模式枚举
enum PLAYMODE{
    LIST_MODE,
    RANDOM_MODE,
    SINGLE_MODE
};

// 歌词结构体
struct Lyric {
    qint64 time;       // 歌词时间点(毫秒)
    QString content;   // 歌词内容
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // 界面初始化相关函数
    void setButtonStyle(QAbstractButton* button, const QString& filename);
    void initButtons();
    void loadAppointMusicDir(const QString& filename);
    void resizeEvent(QResizeEvent *event);
    void setBackGround(const QString &filename);

    // 歌词相关函数
    bool parseLyricFile(const QString& filePath);
    void loadLyricForCurrentSong();
    void updateLyricDisplay();
    QString formatTime(qint64 timeMilliSeconds);

    // 播放控制相关函数
    void shufflePlayList();
    void startPlayMusic();

    // 成员变量
    QMediaPlayer* m_player;
    PLAYMODE m_mode;
    std::vector<int> m_shuffleList;
    int m_shuffleIndex;
    QString musicDir;
    QMap<QString, QString> m_fullFileNameMap;
    bool m_isManualSwitch;
    QWidget* m_listWidget;

    // 进度条相关
    QSlider* m_progressSlider;
    QLabel* m_currentTimeLabel;
    QLabel* m_totalTimeLabel;
    bool m_sliderPressed;

    // 歌词相关
    QList<Lyric> m_lyrics;
    QWidget* m_lyricWidget;
    QVBoxLayout* m_lyricLayout;
    QList<QLabel*> m_lyricLabels;
    int m_currentLyricIndex;
    QString m_currentLyricFile;

private slots:
    // 播放控制槽函数
    void handlePlaySlot();
    void handleModeSlot();
    void handleNextSlot();
    void handlePrevSlot();
    void handleAddMusicSlot();
    void handleChangeBackgroundSlot();
    void handleToggleListSlot();
    void handleMusicListItemClicked(QListWidgetItem *item);

    // 进度与歌词同步槽函数
    void updateProgress(qint64 position);
    void updateDuration(qint64 duration);
    void setPosition(int position);
    void handleProgressSliderPressed();
    void handleProgressSliderReleased();
};

#endif // MAINWINDOW_H
