#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QListWidgetItem>
#include <QMap>
#include <QVBoxLayout>
#include <QFileDialog>
#include <vector>
#include <QTimer>
#include<QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 播放模式枚举
enum PLAYMODE {
    LIST_MODE,
    RANDOM_MODE,
    SINGLE_MODE
};

// 歌词结构体
struct Lyric {
    qint64 time;       // 歌词时间点（毫秒）
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

    // ===== UI与界面相关函数 =====
    void setBtnStyle(QAbstractButton* button, const QString& filename);
    void initButtons();                               // 初始化按钮与信号槽
    void resizeEvent(QResizeEvent *event);            // 重写窗口调整大小事件
    void setBackGround(const QString &filename);      // 设置背景图片
    void loadAppointMusicDir(const QString& filename);// 加载指定路径的音乐文件夹

    // ===== 播放控制与状态 =====
    void startPlayMusic();              // 播放当前选中歌曲
    void shufflePlayList();            // 打乱播放顺序（随机播放模式）

    // ===== 时间与进度控制 =====
    QString formatTime(qint64 timeMilliSeconds);

    // ===== 歌词处理 =====
    bool parseLyricFile(const QString& filePath);
    void loadLyricForCurrentSong();
    void updateLyricDisplay();

    void initCoverDisplay();        // 初始化封面显示组件
    QPixmap getEmbeddedCover();     // 获取内嵌封面
    QPixmap findLocalCoverFile();   // 查找本地封面文件
    void setCoverImage(const QPixmap &pixmap);  // 设置封面图片
    void loadAlbumCover();          // 加载专辑封面

    // ===== 成员变量 =====
    QMediaPlayer* m_player;              // 音乐播放器
    PLAYMODE m_mode;                     // 当前播放模式
    std::vector<int> m_shuffleList;      // 随机播放索引列表
    int m_shuffleIndex;                  // 随机播放当前索引
    QString musicDir;                    // 音乐文件夹路径
    QMap<QString, QString> m_fullFileNameMap; // 歌曲名 => 文件名 映射
    bool m_isManualSwitch;              // 是否为手动切歌
    QWidget* m_listWidget;              // 歌曲列表父容器
    bool m_nightmode;
    QAction *SwitchModeAction;
    QAction *deleteMusicAction;
    QAction *addMusicAction;
    QAction *changeThemeAction;

    // 进度条与时间
    QSlider* m_progressSlider;
    QLabel* m_currentTimeLabel;
    QLabel* m_totalTimeLabel;
    bool m_sliderPressed;

    // 歌词显示相关
    QList<Lyric> m_lyrics;
    QWidget* m_lyricWidget;
    QWidget* m_mainlyricWidget;
    QWidget* m_coverWidget;


    QVBoxLayout* m_lyricLayout;
    QVBoxLayout* m_mainlyricLayout;
    QVBoxLayout* m_coverLayout;
    QList<QLabel*> m_lyricLabels;
    int m_currentLyricIndex;
    QString m_currentLyricFile;

    // 添加音量弹出滑块相关
    QWidget* m_volumePopup = nullptr;
    QSlider* m_volumeSlider = nullptr;
    QTimer* m_hideVolumeTimer = nullptr;


    QLabel *m_coverLabel;       // 封面显示标签
    QPixmap m_defaultCover;     // 默认封面图片

    //放入系统托盘当中的代码
    QSystemTrayIcon *m_systemTray;
    QMenu *m_trayMenu;


    // 播放列表持久化相关
    void savePlayList();    // 保存播放列表
    void loadPlayList();    // 加载播放列表

private slots:
    // ===== 播放控制相关槽函数 =====
    void handlePlaySlot();               // 播放/暂停
    void handleModeSlot();               // 切换播放模式
    void handleNextSlot();               // 播放下一首
    void handlePrevSlot();               // 播放上一首
    void handleAddMusicSlot();           // 添加音乐文件
    void handleChangeBackgroundSlot();   // 更换背景图
    void handleToggleListSlot();         // 显示/隐藏播放列表
    void handleMusicListItemClicked(QListWidgetItem *item); // 列表点击播放
    void handleSwitchModeSlot();

    // ===== 播放进度槽函数 =====
    void updateProgress(qint64 position);       // 播放进度更新
    void updateDuration(qint64 duration);       // 播放总时长更新
    void setPosition(int position);             // 设置播放位置
    void handleProgressSliderPressed();         // 拖动开始
    void handleProgressSliderReleased();        // 拖动释放
    void onVolumeSliderValueChanged(int k);//切换静音

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);  // 媒体状态变化槽函数
    void initSystemTray();
    void closeEvent(QCloseEvent* event);

    void showDesktopNotification(const QString &title, const QString &message);

    void handleDeleteMusicSlot();
};

#endif // MAINWINDOW_H
