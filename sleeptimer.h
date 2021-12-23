#ifndef SLEEPTIMER_H
#define SLEEPTIMER_H

#include <QLCDNumber>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QTimer>

class SleepTimer : public QObject
{
    Q_OBJECT
public:
    explicit SleepTimer(
            QObject *parent,
            QPushButton *pushButton30m,
            QPushButton *pushButton10m,
            QPushButton *pushButton5m,
            QPushButton *pushButton30s,
            QPushButton *pushButton5s,
            QPushButton *pushButton00s,
            QLCDNumber *lcdNumberTimer
            );

signals:
    void volumeFadeOut(double);
    void cancelled();
    void finished();
private:
    const int FADE_OUT_SECONDS_MIN = 5;
    const int FADE_OUT_SECONDS_MAX = 5 * 60;
    const double FADE_OUT_SECONDS_PERPORTION = 0.2;
    int seconds = 0;
    int fadeOutSeconds = 0;
    QPushButton *pushButton30m;
    QPushButton *pushButton10m;
    QPushButton *pushButton5m;
    QPushButton *pushButton30s;
    QPushButton *pushButton5s;
    QPushButton *pushButton00s;
    QLCDNumber *lcdNumberTimer;
    QTimer timer;
    void reset();
    void updateUI();
    void adjustFadeOutSeconds();
    void topUpSeconds(int amount);
private slots:
    void onTimer();
    void onPushButton30mClicked();
    void onPushButton10mClicked();
    void onPushButton5mClicked();
    void onPushButton30sClicked();
    void onPushButton5sClicked();
    void onPushButton00sClicked();
};

#endif // SLEEPTIMER_H
