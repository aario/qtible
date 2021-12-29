#ifndef TREE_VIEW_SOURCE_LIBRARY_H
#define TREE_VIEW_SOURCE_LIBRARY_H

#include <QFileSystemModel>
#include <QMap>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QQueue>
#include <settings.h>
#include <QTimer>
#include "models/progress.h"

class TreeViewSourceLibrary : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TreeViewSourceLibrary();
    static const int COLUMN_PATH = 0;
    static const int COLUMN_LENGTH = 2;
    static const int COLUMN_PROGRESS = 3;
    QString rootPath();
    QModelIndex indexByPath(const QString &path) const;
    QString getSelectedFilePath(int selectionRow, QModelIndex selectionParent) const;
    void refresh();
    void setProgress(QString path, int progress);
    bool isFolder(QModelIndex index) const;
    void setResetProgress(QModelIndex index, bool reset);
    Progress *getProgress(QString path);
    void saveProgressCache();
    QString getNextFilePath(QString currentPath);
    bool directoryIsLoaded(const QString path);
    QString pathByIndex(const QModelIndex &index) const;
    void add(QString path);
Q_SIGNALS:
    void directoryLoaded(const QString &path);
private:
    const QStringList EXTENSIONS_SUPPORTED;
    const QString FILE_PREFIX_PROGRESS = ".qtible-progress.";
    const int SAVE_PROGRESS_CACHE_INTERVAL = 60000;
    const int SORT_SCORE_LEADING_ZEROS = 10;
    QFileSystemModel fileSystemModel;
    QMap<QString, Progress*> progressCache;
    QTimer timerSaveProgressCache;
    QStringList directoriesLoaded;
    bool isFolder(int sourceRow, const QModelIndex &sourceParent) const;
    virtual bool filterAcceptsRow(
            int sourceRow,
            const QModelIndex &sourceParent
            ) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QString getPathForIndex(const QModelIndex *index) const;
    Progress *getProgress(const QModelIndex *index);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int getFileLength(QString filePath) const;
    QString getProgressFilePath(QFileInfo fileInfo) const;
    void saveProgressFile(QFileInfo fileInfo, Progress *progress) const;
    void readProgressFile(QString progressFilePath, Progress *progress) const;
    void scanProgressInChildren(const QModelIndex *index, Progress *progress);
    void notifyUpdate();
    QString convertStringForSortScore(QString string) const;
    QVariant dataForColumnLength(const QModelIndex &index) const;
    QVariant dataForColumnProgress(const QModelIndex &index) const;
    void onDirectoryLoaded(const QString &path);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // TREE_VIEW_SOURCE_LIBRARY_H
