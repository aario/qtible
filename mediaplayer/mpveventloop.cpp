#include "mpveventloop.h"

#include <QDebug>

MPVEventLoop::MPVEventLoop(mpv_handle *mpv)
{
    this->mpv = mpv;
    connect(this, &MPVEventLoop::shutdown, this, &MPVEventLoop::shutdown);
}

void MPVEventLoop::shutdown()
{
    mustShutdown = true;
}

void MPVEventLoop::eventLoop()
{
    qInfo() << "Started event loop";
    while (1) {
        if (mustShutdown)
            break;

        mpv_event *event = mpv_wait_event(mpv, 0.01);

        switch (event->event_id) {
        case MPV_EVENT_NONE:
            break;
        case MPV_EVENT_FILE_LOADED:
            emit fileLoaded();
            break;
        case MPV_EVENT_END_FILE:
            emit endFile();
            break;
        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property *property = (mpv_event_property *)event->data;
            if (strcmp(property->name, "time-pos") == 0) {
                if (property->format == MPV_FORMAT_DOUBLE) {
                    emit timePosChanged(*(double *)property->data);
                }
            }
            break;
        }
        default: qDebug() << "Ignoring MPV event: " << event->event_id;
        }
    }
}
