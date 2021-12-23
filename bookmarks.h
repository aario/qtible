#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QFileSystemModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QStringList>
#include <settings.h>
#include "bookmark.h"

class Bookmarks : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    Bookmarks();
    static const int COLUMN_NAME = 0;
    static const int COLUMN_TIME = 3;
    QString rootPath();
    QModelIndex indexByPath(const QString &path) const;
    Bookmark getSelectedBookmark(int selectionRow, QModelIndex selectionParent) const;
    bool isFolder(QModelIndex index) const;
    void addBookmark(
            QString path,
            QString name,
            int seconds
            );
    void saveBookmark(Bookmark bookmark);
    void deleteBookmark(Bookmark bookmark);
Q_SIGNALS:
    void directoryLoaded(const QString &path);
private:
    const QString FILE_PREFIX_BOOKMARK = ".qtible-bookmark.";
    QFileSystemModel fileSystemModel;
    Bookmark readBookmark(QString bookmarkFilePath) const;
    bool isFolder(int sourceRow, const QModelIndex &sourceParent) const;
    virtual bool filterAcceptsRow(
            int sourceRow,
            const QModelIndex &sourceParent
            ) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QString getPathForIndex(const QModelIndex *index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void notifyUpdate();
    QVariant dataForColumnName(const QModelIndex &index) const;
    QVariant dataForColumnTime(const QModelIndex &index) const;
    void onDirectoryLoaded(const QString &path);
    bool folderHasBookmarks(QString folderPath) const;
    QString makeBookmarkFilePath(QString targetPath);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    Bookmark getBookmarkForIndex(QModelIndex index) const;
};

#endif // BOOKMARKS_H
