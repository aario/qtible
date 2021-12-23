#include "labelseekbar.h"
#include<QDebug>

LabelSeekBar::LabelSeekBar(QWidget *parent) : QLabel(parent)
{
    pixmap = QPixmap(PIXMAP_PATH_LOADED);
    QColor color = QColor(Qt::red);
    pen = QPen(color);
    pen.setWidth(pixmap.width() * penWidthMultiplier);
}

int LabelSeekBar::widgetXToPixmapX(int widgetX)
{
    return widgetX * pixmap.width() / width();
}

void LabelSeekBar::draw(int position)
{
    if (position < 0)
        position = 0;

    if (position > width())
        position = width();

    pixmap.load(PIXMAP_PATH_LOADED);
    QPainter painter(&pixmap);
    painter.setPen(pen);
    int xPos = widgetXToPixmapX(position);
    painter.drawLine(xPos, 0, xPos, pixmap.height());
    setPixmap(pixmap);
}

void LabelSeekBar::mousePressEvent(QMouseEvent *e)
{
    if (!enabled)
        return;

    if(e->button() != Qt::LeftButton)
        return;

    int x = e->pos().x();
    draw(x);
    emit sought(x, width());

    QLabel::mousePressEvent(e);
}

void LabelSeekBar::mouseMoveEvent(QMouseEvent *e)
{
    if (!enabled)
        return;

    if (!(e->buttons() & Qt::LeftButton))
        return;

    int x = e->pos().x();
    draw(x);
    emit sought(x, width());
}

void LabelSeekBar::reset()
{
    pixmap.load(PIXMAP_PATH_LOADED);
    setPixmap(pixmap);
}

void LabelSeekBar::empty()
{
    pixmap.load(PIXMAP_PATH_EMPTY);
    setPixmap(pixmap);
}

void LabelSeekBar::setEnabled(bool enabled) {
    this->enabled = enabled;
}
