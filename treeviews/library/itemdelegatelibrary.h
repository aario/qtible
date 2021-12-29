#ifndef ITEMDELEGATELIBRARY_H
#define ITEMDELEGATELIBRARY_H

#include <QItemDelegate>
#include <QItemDelegate>

class ItemDelegateLibrary : public QItemDelegate
{
    Q_OBJECT
public:
    ItemDelegateLibrary(QObject *parent);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QWidget *parentWidget;
    void paintLength(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paintProgress(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // ITEMDELEGATELIBRARY_H
