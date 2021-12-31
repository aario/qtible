#include "itemdelegatelibrary.h"

#include <QItemDelegate>
#include <QDebug>
#include <QStyleOptionProgressBar>
#include <QObject>
#include <QApplication>
#include "treeviewsourcelibrary.h"

ItemDelegateLibrary::ItemDelegateLibrary(QObject *parent): QItemDelegate(parent)
{
    parentWidget = qobject_cast<QWidget *>(parent);
}

void ItemDelegateLibrary::paintLength(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const {
    int length = index.data().toInt();
    if (length < 0)
        //There has been a problem calculating total
        return;

    QItemDelegate::paint(painter, option, index);
}

void ItemDelegateLibrary::paintProgress(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const {
    int progress = index.data().toInt();
    if (progress < 0)
        //There has been a problem calculating total
        return;

    QStyleOptionProgressBar progressBar;
    progressBar.state = QStyle::State_Enabled;
    progressBar.direction = QApplication::layoutDirection();
    progressBar.rect = option.rect;
    progressBar.fontMetrics = QApplication::fontMetrics();
    progressBar.minimum = 0;
    progressBar.maximum = 100;
    progressBar.textAlignment = Qt::AlignCenter;
    progressBar.textVisible = true;

    progressBar.progress = progress < 0 ? 0 : progress;
    progressBar.text = QString::asprintf("%d%%", progressBar.progress);
    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBar, painter);
}

void ItemDelegateLibrary::paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const {
    switch (index.column()) {
    case TreeViewSourceLibrary::COLUMN_LENGTH:
        paintLength(
                    painter,
                    option,
                    index
                    );
        return;
    case TreeViewSourceLibrary::COLUMN_PROGRESS:
        paintProgress(
                    painter,
                    option,
                    index
                    );
        return;
    }

    QItemDelegate::paint(painter, option, index);
}

