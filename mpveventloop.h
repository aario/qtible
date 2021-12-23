#ifndef MPVEVENTLOOP_H
#define MPVEVENTLOOP_H

#include <QObject>
#include <mpv/client.h>

class MPVEventLoop : public QObject
{
    Q_OBJECT

public:
    MPVEventLoop(mpv_handle *mpv);
public slots:
    void eventLoop();
    void shutdown();
signals:
    void fileLoaded();
    void endFile();
    void timePosChanged(double);
private:
    mpv_handle *mpv;
    bool mustShutdown = false;
};

#endif // MPVEVENTLOOP_H
