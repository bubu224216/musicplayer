#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include<QPushButton>
#include<vector>

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
public slots:
    //处理暂停播放
    void handlePlaySlot();
    //处理播放模式
    void handleModeSlot();
    //处理下一首
    void handleNextSlot();
    //处理上一首
    void handlePrevSlot();
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
};
#endif // MAINWINDOW_H
