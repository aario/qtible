#ifndef LABELSEEKBAR_H
#define LABELSEEKBAR_H

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QDragMoveEvent>

class LabelSeekBar : public QLabel
{
    Q_OBJECT
public:
    LabelSeekBar(QWidget* parent=nullptr);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent *event);
    void reset();
    void empty();
    void draw(int position);
    void seek(int percent);
    void setEnabled(bool enabled);
signals:
    void sought(int position, int basedOnLimit);

private:
    const QString PIXMAP_PATH_LOADED = QString("://graphics/images/spectrom.svg");
    const QString PIXMAP_PATH_EMPTY = QString("://graphics/images/spectrom-empty.svg");
    const float penWidthMultiplier = 0.005;
    QPen pen;
    QPixmap pixmap;
    int widgetXToPixmapX(int mouseX);
    bool enabled = false;
};

#endif // LABELSEEKBAR_H
