#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QSlider>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QLCDNumber>
#include <QPushButton>
#include <mpv/client.h>
#include "labelseekbar.h"
#include "mpveventloop.h"

class MediaPlayer : public QObject
{
    Q_OBJECT
    QThread mpvEventLoopThread;
public:
    MediaPlayer(
            QObject *parent,
            QSlider *sliderVolume,
            QLabel *labelVolume,
            QPushButton *pushButtonVolumeMute,
            QSlider *sliderSpeed,
            QLabel *labelSpeed,
            QPushButton *pushButtonSpeedReset,
            LabelSeekBar *labelSeekBar,
            QLCDNumber *lcdNumberPlayerStatus,
            QPushButton *pushButtonPlayerRewind,
            QPushButton *pushButtonPlayerPlayPause,
            QPushButton *pushButtonPlayerFastForward,
            QLabel *labelTitle,
            QPushButton *pushButtonAddBookmark
            );
    static const int SKIP_SECONDS = 5;
    void stop();
    bool isLoaded();
    bool  isPlaying();
    QString getPath();
    QString getTitle();
    void open(QString path, int progress, int length);
    void play();
public slots:
    void sliderVolumeChanged(int64_t Position);
    void onVolumeMuteClicked();
    void sliderSpeedChanged(int Speed);
    void onSpeedResetClicked();
    int currentPosition(int basedOnLimit = 100);
    void onLabelSeekBarSought(int position, int basedOnLimit);
    void onRewindClicked();
    void onPlayPauseClicked();
    void onFastForwardClicked();
    void onSleepTimerCancelled();
    void onSleepTimerFinished();
    void onSleepTimerVolumeFadeOut(double level);
signals:
    void progressUpdate(QString, int);
    void began();
    void finished(QString);
    void rewindToPrevious(QString);
    void startMPVEventLoop();
    void addBookmark(
            QString,
            QString,
            int
            );
private:
    const QString MPLAYER_PATH = "/usr/bin/mplayer";
    const QString LADSPA_PATH = "/usr/lib/ladspa/tap_pitch.so";
    const int READ_OUTPUT_MAX_TRIES = 3;
    const int MAX_SEEKBAR_POSITION_DIFFERENCE = 5;
    const int DELAY_READ_VALUE = 50;
    mpv_handle *mpv = NULL;
    MPVEventLoop *mpvEventLoop;
    float currentPositionSeconds;
    QString path;
    QString title;
    QSlider *sliderVolume;
    QLabel *labelVolume;
    QPushButton *pushButtonVolumeMute;
    QSlider *sliderSpeed;
    QLabel *labelSpeed;
    QPushButton *pushButtonSpeedReset;
    LabelSeekBar *labelSeekBar;
    QLCDNumber *lcdNumberPlayerStatus;
    QPushButton *pushButtonPlayerRewind;
    QPushButton *pushButtonPlayerPlayPause;
    QIcon iconPlay;
    QIcon iconPause;
    QPushButton *pushButtonPlayerFastForward;
    QLabel *labelTitle;
    QPushButton *pushButtonAddBookmark;
    QTimer timerUpdateUI;
    int expectedPositionPercent = -1;
    float length = 0;
    int volumeBeforeMute = 100;
    bool playing = false;
    bool loaded = false;
    float pausedPosition = 0;
    float lastPositionUpdatedOnUI = 0;
    float lastLengthUpdatedOnUI = 0;
    double sleepTimerVolumeLevel = 1;
    QString readOutput();
    void delay(int msecs);
    QString readValue(QString command, QString key);
    float getFileLength();
    void updateExpectedPosition(int seconds);
    void updateUI();
    void updatePlayPauseIcon();
    void openUI();
    void closeUI();
    void seek(int position, int basedOnLimit);
    double convertSliderSpeedToMpvSpeed(int sliderSpeedValue);
    void setSpeed(double speed);
    void setVolume(int64_t Volume);
    void destroyMpv();
private slots:
    void onTimerUpdateUI();
    void onMpvFileLoaded();
    void onMpvEndFile();
    void onMpvTimePosChanged(double timePos);
    void onAddBookmarkClicked();
};

#endif // MEDIAPLAYER_H
