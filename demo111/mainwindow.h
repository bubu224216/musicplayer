#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include<QPushButton>
#include<vector>
#include <QFileDialog>
#include<QSlider>
#include<QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//创建播放模式
enum PLAYMODE{
    LIST_MODE,
    RANDOM_MODE,
    SINGLE_MODE

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void setButtonStyle(QPushButton* button, const QString& filename);
    //初始化按钮
    void initButtons();
    //加载指定的文件夹
    void loadAppointMusicDir(const QString&filename);
    void resizeEvent(QResizeEvent *event);
    //洗牌算法
    void shufflePlayList();
    //播放音乐
    void startPlayMusic();
    //设置文件背景
    void setBackGround(const QString & fliename);
    //格式化时间函数
     QString formatTime(qint64 timeMilliSeconds);
public slots:
    //处理暂停播放
    void handlePlaySlot();
    //处理播放模式
    void handleModeSlot();
    //处理下一首
    void handleNextSlot();
    //处理上一首
    void handlePrevSlot();
    // 添加音乐文件的槽函数
    void handleAddMusicSlot();

    //当前时间的槽函数
    void updateProgress(qint64 position);
    //总的时长的槽函数
    void updateDuration(qint64 duration);
    //设置播放位置的槽函数
    void setPosition(int position);
    //进度条按压
    void handleProgressSliderPressed();
    //进度条释放
    void handleProgressSliderReleased();

private:
    //音乐播放器
    QMediaPlayer* m_player;

    //当前的播放模式
    PLAYMODE m_mode;

    //数组
    std::vector<int> m_shuffleList;

    //索引
    int m_shuffleIndex;

    //音乐文件夹
    QString musicDir;

    //完整的文件名称
    QMap<QString, QString> m_fullFileNameMap;//歌曲名称和后缀名对应

    // 标记是否手动切歌
    bool m_isManualSwitch;


    QSlider* m_progressSlider;      // 进度条指针
    QLabel* m_currentTimeLabel;     // 当前时间标签指针
    QLabel* m_totalTimeLabel;       // 总时长标签指针
    bool m_sliderPressed;           // 标记进度条是否正在被拖动
};
#endif // MAINWINDOW_H
