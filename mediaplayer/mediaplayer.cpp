#include "mediaplayer.h"
#include "utils.h"

#include <QObject>
#include <QSlider>
#include <QSlider>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include "labelseekbar.h"

MediaPlayer::MediaPlayer(QObject *parent,
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
                         ): QObject(parent)
{
    this->sliderVolume = sliderVolume;
    this->labelVolume = labelVolume;
    this->pushButtonVolumeMute = pushButtonVolumeMute;
    this->sliderSpeed = sliderSpeed;
    this->labelSpeed = labelSpeed;
    this->pushButtonSpeedReset = pushButtonSpeedReset;
    this->labelSeekBar = labelSeekBar;
    this->labelSeekBar->empty();
    this->lcdNumberPlayerStatus = lcdNumberPlayerStatus;
    this->pushButtonPlayerRewind = pushButtonPlayerRewind;
    this->pushButtonPlayerPlayPause = pushButtonPlayerPlayPause;
    this->pushButtonPlayerFastForward = pushButtonPlayerFastForward;
    this->labelTitle = labelTitle;
    this->pushButtonAddBookmark = pushButtonAddBookmark;
}

void MediaPlayer::destroyMpv()
{
    qInfo() << "Wating for mpv thread to shutdown...";
    mpvEventLoop->shutdown();
    mpvEventLoopThread.quit();
    mpvEventLoopThread.wait();
    mpv_terminate_destroy(mpv);
    qInfo() << "Killed mpv thread";
}

void MediaPlayer::open(
        QString path,
        int progress,
        int length
        )
{
    if (mpv)
        destroyMpv();

    mpv = mpv_create();
    if (!mpv)
        throw std::runtime_error("can't create mpv instance");

    this->path = path;

    loaded = true;

    iconPlay.addFile(QString::fromUtf8(":/graphics/images/play-button.svg"), QSize(), QIcon::Normal, QIcon::Off);
    iconPause.addFile(QString::fromUtf8(":/graphics/images/pause.svg"), QSize(), QIcon::Normal, QIcon::Off);
    int volume = sliderVolume->value();
    setVolume(volume);
    int speed = sliderSpeed->value();
    setSpeed(convertSliderSpeedToMpvSpeed(speed));

    this->length = length;
    if (progress >= length)
        progress = 0; //Play again if already been finished
    currentPositionSeconds = progress;
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);

    mpv_set_option(
                mpv,
                "start",
                MPV_FORMAT_INT64,
                &progress
                );

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("mpv failed to initialize");

    const char *args[] = {
        "loadfile",
        path.toUtf8().data(),
        "replace",
        QString(
        "start=" + QString::number(progress)
        ).toUtf8(),
        NULL
    };
    mpv_command(
                mpv,
                args
                );

    mpvEventLoop = new MPVEventLoop(mpv);
    mpvEventLoop->moveToThread(&mpvEventLoopThread);
    connect(&mpvEventLoopThread, &QThread::finished, mpvEventLoop, &QObject::deleteLater);
    connect(this, &MediaPlayer::startMPVEventLoop, mpvEventLoop, &MPVEventLoop::eventLoop);
    connect(mpvEventLoop, &MPVEventLoop::fileLoaded, this, &MediaPlayer::onMpvFileLoaded);
    connect(mpvEventLoop, &MPVEventLoop::endFile, this, &MediaPlayer::onMpvEndFile);
    connect(mpvEventLoop, &MPVEventLoop::timePosChanged, this, &MediaPlayer::onMpvTimePosChanged);
    mpvEventLoopThread.start();
    emit startMPVEventLoop();
}

void MediaPlayer::play()
{
    if (!loaded)
        return;

    if (playing)
        return;

    onPlayPauseClicked();
}

QString MediaPlayer::getTitle()
{
    if (title.length())
        return title;

    mpv_node metadata;
    mpv_get_property(
                mpv,
                "metadata",
                MPV_FORMAT_NODE,
                &metadata
                );
    if (metadata.format != MPV_FORMAT_NODE_MAP
            || metadata.u.list->num <= 0) {
        char *fallBackTitle;
        mpv_get_property(
                    mpv,
                    "media-title",
                    MPV_FORMAT_STRING,
                    &fallBackTitle
                    );
        title = fallBackTitle;

        return title;
    }

    char *title;
    mpv_get_property(
                mpv,
                "metadata/by-key/title",
                MPV_FORMAT_STRING,
                &title
                );
    char *artist;
    mpv_get_property(
                mpv,
                "metadata/by-key/artist",
                MPV_FORMAT_STRING,
                &artist
                );
    this->title = QString(
                (strlen(artist) > 0 ? QString(artist) + " - " : "")
                + title
                );

    return this->title;

}

void MediaPlayer::openUI()
{
    playing = true;

    labelSeekBar->setEnabled(true);
    pushButtonPlayerRewind->setEnabled(true);
    pushButtonPlayerPlayPause->setEnabled(true);
    pushButtonPlayerFastForward->setEnabled(true);
    labelTitle->setText(getTitle());
    pushButtonAddBookmark->setEnabled(true);
    pushButtonVolumeMute->setEnabled(true);
    pushButtonSpeedReset->setEnabled(true);
    sliderVolume->setEnabled(true);
    sliderSpeed->setEnabled(true);
    updatePlayPauseIcon();
    updateUI();
    timerUpdateUI.setInterval(1000);
    timerUpdateUI.setTimerType(Qt::PreciseTimer);
    QObject::connect(labelSeekBar, &LabelSeekBar::sought, this, &MediaPlayer::onLabelSeekBarSought);
    QObject::connect(sliderVolume, &QSlider::valueChanged, this, &MediaPlayer::sliderVolumeChanged);
    QObject::connect(pushButtonVolumeMute, &QPushButton::clicked, this, &MediaPlayer::onVolumeMuteClicked);
    QObject::connect(sliderSpeed, &QSlider::valueChanged, this, &MediaPlayer::sliderSpeedChanged);
    QObject::connect(pushButtonSpeedReset, &QPushButton::clicked, this, &MediaPlayer::onSpeedResetClicked);
    QObject::connect(pushButtonPlayerRewind, &QPushButton::clicked, this, &MediaPlayer::onRewindClicked);
    QObject::connect(pushButtonPlayerPlayPause, &QPushButton::clicked, this, &MediaPlayer::onPlayPauseClicked);
    QObject::connect(pushButtonPlayerFastForward, &QPushButton::clicked, this, &MediaPlayer::onFastForwardClicked);
    QObject::connect(pushButtonAddBookmark, &QPushButton::clicked, this, &MediaPlayer::onAddBookmarkClicked);
    QObject::connect(&timerUpdateUI, &QTimer::timeout, this, &MediaPlayer::onTimerUpdateUI, Qt::DirectConnection);
    timerUpdateUI.start();
    emit began();
}

void MediaPlayer::closeUI()
{
    playing = false;

    this->lcdNumberPlayerStatus->display("--:-- / --:--");
    timerUpdateUI.stop();
    labelSeekBar->reset();
    labelSeekBar->setEnabled(false);
    pushButtonPlayerRewind->setEnabled(false);
    pushButtonPlayerPlayPause->setEnabled(false);
    pushButtonPlayerFastForward->setEnabled(false);
    labelTitle->setText("");
    pushButtonAddBookmark->setEnabled(false);
    pushButtonVolumeMute->setEnabled(false);
    pushButtonSpeedReset->setEnabled(false);
    sliderVolume->setEnabled(false);
    sliderSpeed->setEnabled(false);
    QObject::disconnect(labelSeekBar, &LabelSeekBar::sought, this, &MediaPlayer::onLabelSeekBarSought);
    QObject::disconnect(sliderVolume, &QSlider::valueChanged, this, &MediaPlayer::sliderVolumeChanged);
    QObject::disconnect(pushButtonVolumeMute, &QPushButton::clicked, this, &MediaPlayer::onVolumeMuteClicked);
    QObject::disconnect(sliderSpeed, &QSlider::valueChanged, this, &MediaPlayer::sliderSpeedChanged);
    QObject::disconnect(pushButtonSpeedReset, &QPushButton::clicked, this, &MediaPlayer::onSpeedResetClicked);
    QObject::disconnect(pushButtonPlayerRewind, &QPushButton::clicked, this, &MediaPlayer::onRewindClicked);
    QObject::disconnect(pushButtonPlayerPlayPause, &QPushButton::clicked, this, &MediaPlayer::onPlayPauseClicked);
    QObject::disconnect(pushButtonPlayerFastForward, &QPushButton::clicked, this, &MediaPlayer::onFastForwardClicked);
}

void MediaPlayer::stop()
{
    closeUI();
    destroyMpv();
}

void MediaPlayer::delay(int msecs)
{
    QTime waitUntil = QTime::currentTime().addMSecs(msecs);
    while (QTime::currentTime() < waitUntil )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
}

float MediaPlayer::getFileLength()
{
    if (length)
        return length;

    const char *args[] = {"duration", NULL};
    mpv_node result;
    mpv_command_ret(mpv, args, &result);
    length = result.u.int64;
    return length;
}

void MediaPlayer::updateExpectedPosition(int seconds)
{
    expectedPositionPercent = seconds * 100 / getFileLength();
}

void MediaPlayer::seek(int position, int basedOnLimit)
{
    qDebug() << "Seeking to " << position << " of " << basedOnLimit;
    float seconds = (float) position * getFileLength() / (float) basedOnLimit;
    if (!playing)
        pausedPosition = seconds;
    const char *args[] = {"seek", QString::number(seconds).toUtf8(), "absolute", NULL};
    mpv_command(mpv, args);
    updateExpectedPosition(seconds);
}

void MediaPlayer::onLabelSeekBarSought(int position, int basedOnLimit)
{
    seek(position, basedOnLimit);
    updateUI();
}

void MediaPlayer::onRewindClicked()
{
    QObject::disconnect(pushButtonPlayerRewind, &QPushButton::clicked, this, &MediaPlayer::onRewindClicked);
    int position = currentPositionSeconds - SKIP_SECONDS;
    if (position < 0) {
        position = 0;
    }

    onLabelSeekBarSought(position, getFileLength());

    if (position == 0) {
        closeUI();
        emit rewindToPrevious(path);
    } else {
        QObject::connect(pushButtonPlayerRewind, &QPushButton::clicked, this, &MediaPlayer::onRewindClicked);
    }
}

void MediaPlayer::updatePlayPauseIcon()
{
    pushButtonPlayerPlayPause->setIcon(playing ? iconPause : iconPlay);
}

void MediaPlayer::onPlayPauseClicked()
{
    pausedPosition = currentPositionSeconds;
    const char *args[] = {"cycle", "pause", NULL};
    mpv_command(mpv, args);
    playing = !playing;
    updatePlayPauseIcon();
    updateUI();
}

void MediaPlayer::onFastForwardClicked()
{
    QObject::disconnect(pushButtonPlayerFastForward, &QPushButton::clicked, this, &MediaPlayer::onFastForwardClicked);
    int position = currentPositionSeconds + SKIP_SECONDS;
    int length = getFileLength();
    if (position >= length) {
        position = length;
    }

    onLabelSeekBarSought(position, getFileLength());

    if (position == length) {
        onMpvEndFile();
    } else {
        QObject::connect(pushButtonPlayerFastForward, &QPushButton::clicked, this, &MediaPlayer::onFastForwardClicked);
    }
}

void MediaPlayer::onAddBookmarkClicked()
{
    emit addBookmark(
                path,
                getTitle(),
                currentPositionSeconds
                );
}

void MediaPlayer::setVolume(int64_t Volume)
{
    if (!mpv)
        return;

    int64_t volume = (double) Volume * sleepTimerVolumeLevel;

    mpv_set_property(
                mpv,
                "volume",
                MPV_FORMAT_INT64,
                &volume
                );
}

void MediaPlayer::sliderVolumeChanged(int64_t Volume)
{
    setVolume(Volume);
    labelVolume->setText(QString::number(Volume) + "%");
}

void MediaPlayer::onVolumeMuteClicked()
{
    if (sliderVolume->value()) {
        volumeBeforeMute = sliderVolume->value();
        sliderVolume->setValue(0);
    } else {
        sliderVolume->setValue(volumeBeforeMute);
    }
}

double MediaPlayer::convertSliderSpeedToMpvSpeed(int sliderSpeedValue)
{
    return (float) sliderSpeedValue / 100;
}

void MediaPlayer::setSpeed(double speed)
{
    mpv_set_property(
                mpv,
                "speed",
                MPV_FORMAT_DOUBLE,
                &speed
                );
}

void MediaPlayer::sliderSpeedChanged(int Speed)
{
    double speed = convertSliderSpeedToMpvSpeed(Speed);
    setSpeed(speed);
    labelSpeed->setText(QString::number(speed) + "x");
}

void MediaPlayer::onSpeedResetClicked()
{
    sliderSpeed->setValue(100);
}

int MediaPlayer::currentPosition(int basedOnLimit)
{
    return currentPositionSeconds * basedOnLimit / getFileLength();
}

void MediaPlayer::updateUI()
{
    float length = getFileLength();
    expectedPositionPercent = currentPositionSeconds * 100 / length;
    if (currentPositionSeconds == lastPositionUpdatedOnUI && length == lastLengthUpdatedOnUI) {
        return;
    }
    lastPositionUpdatedOnUI = currentPositionSeconds;
    lastLengthUpdatedOnUI = length;
    QString status = Utils::formatTime(currentPositionSeconds) + " : " + Utils::formatTime(length);
    this->lcdNumberPlayerStatus->display(status);
    labelSeekBar->draw(currentPosition(labelSeekBar->width()));
    emit progressUpdate(path, currentPositionSeconds);
}

void MediaPlayer::onMpvTimePosChanged(double timePos)
{
    currentPositionSeconds = timePos;
}

void MediaPlayer::onTimerUpdateUI()
{
    updateUI();
}

void MediaPlayer::onMpvFileLoaded()
{
    openUI();
}

void MediaPlayer::onMpvEndFile()
{
    currentPositionSeconds = length;
    updateUI();
    closeUI();
    emit finished(path);
}

bool MediaPlayer::isLoaded()
{
    return loaded;
}

bool MediaPlayer::isPlaying()
{
    return playing;
}

QString MediaPlayer::getPath()
{
    return path;
}

void MediaPlayer::onSleepTimerCancelled()
{
    sleepTimerVolumeLevel = 1;
    qInfo() << "onSleepTimerCancelled: " << sleepTimerVolumeLevel;
    sliderVolumeChanged(sliderVolume->value());
}

void MediaPlayer::onSleepTimerFinished()
{
    if (playing)
        onPlayPauseClicked();

    onSleepTimerCancelled();
}

void MediaPlayer::onSleepTimerVolumeFadeOut(double level)
{
    qInfo() << "onSleepTimerVolumeFadeOut: " << level;
    sleepTimerVolumeLevel = level;

    sliderVolumeChanged(sliderVolume->value());
}
