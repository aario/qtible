#include "sleeptimer.h"

#include "utils.h"

SleepTimer::SleepTimer(
        QObject *parent,
        QPushButton *pushButton30m,
        QPushButton *pushButton10m,
        QPushButton *pushButton5m,
        QPushButton *pushButton30s,
        QPushButton *pushButton5s,
        QPushButton *pushButton00s,
        QLCDNumber *lcdNumberTimer
        ) : QObject(parent)
{
    this->pushButton30m = pushButton30m;
    this->pushButton10m = pushButton10m;
    this->pushButton5m = pushButton5m;
    this->pushButton30s = pushButton30s;
    this->pushButton5s = pushButton5s;
    this->pushButton00s = pushButton00s;
    this->lcdNumberTimer = lcdNumberTimer;
    QObject::connect(&timer, &QTimer::timeout, this, &SleepTimer::onTimer, Qt::DirectConnection);
    QObject::connect(pushButton30m, &QPushButton::clicked, this, &SleepTimer::onPushButton30mClicked);
    QObject::connect(pushButton10m, &QPushButton::clicked, this, &SleepTimer::onPushButton10mClicked);
    QObject::connect(pushButton5m, &QPushButton::clicked, this, &SleepTimer::onPushButton5mClicked);
    QObject::connect(pushButton30s, &QPushButton::clicked, this, &SleepTimer::onPushButton30sClicked);
    QObject::connect(pushButton5s, &QPushButton::clicked, this, &SleepTimer::onPushButton5sClicked);
    QObject::connect(pushButton00s, &QPushButton::clicked, this, &SleepTimer::onPushButton00sClicked);
    timer.setInterval(1000);
    timer.setTimerType(Qt::PreciseTimer);
    reset();
    timer.start();
}

void SleepTimer::reset()
{
    seconds = 0;
    updateUI();
    emit cancelled();
}

void SleepTimer::updateUI()
{
    lcdNumberTimer->display(seconds ? Utils::formatTime(seconds) : "--:--");
}

void SleepTimer::onTimer()
{
    if (seconds == 0)
        return;

    seconds--;

    updateUI();

    if (seconds > fadeOutSeconds)
        return;

    if (seconds == 0) {
        emit finished();
        updateUI();
        return;
    }

    emit volumeFadeOut((double) seconds / (double) fadeOutSeconds);
}

void SleepTimer::onPushButton30mClicked()
{
    topUpSeconds(30 * 60);
    fadeOutSeconds = FADE_OUT_SECONDS_MAX;
}

void SleepTimer::adjustFadeOutSeconds()
{
    fadeOutSeconds = (double) seconds * FADE_OUT_SECONDS_PERPORTION;
    if (fadeOutSeconds < FADE_OUT_SECONDS_MIN)
        fadeOutSeconds = FADE_OUT_SECONDS_MIN;

    if (fadeOutSeconds > FADE_OUT_SECONDS_MAX)
        fadeOutSeconds = FADE_OUT_SECONDS_MAX;
}

void SleepTimer::topUpSeconds(int amount)
{
    seconds += amount;
    adjustFadeOutSeconds();
    updateUI();
}

void SleepTimer::onPushButton10mClicked()
{
    topUpSeconds(10 * 60);
}

void SleepTimer::onPushButton5mClicked()
{
    topUpSeconds(5 * 60);
}

void SleepTimer::onPushButton30sClicked()
{
    topUpSeconds(30);
}

void SleepTimer::onPushButton5sClicked()
{
    topUpSeconds(5);
}

void SleepTimer::onPushButton00sClicked()
{
    reset();
}
