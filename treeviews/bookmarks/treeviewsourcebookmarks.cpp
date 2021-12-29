#include "treeviews/bookmarks/treeviewsourcebookmarks.h"
#include "treeviews/library/treeviewsourcelibrary.h"
#include "utils.h"

#include <QDir>
#include <QVariant>
#include <QFileInfo>
#include <QtDebug>
#include "mediaplayer/mediaplayer.h"

TreeViewSourceBookmarks::TreeViewSourceBookmarks():
    QSortFilterProxyModel(),
    fileSystemModel()
{
    QDir dir(Settings::getLibraryPath());
    if (!dir.exists(dir.path()))
        dir.mkpath(dir.path());

    fileSystemModel.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden);
    fileSystemModel.setRootPath(dir.path());

    setSourceModel(&fileSystemModel);

    QObject::connect(
                &fileSystemModel,
                &QFileSystemModel::directoryLoaded,
                this,
                &TreeViewSourceBookmarks::onDirectoryLoaded
                );
}

QString TreeViewSourceBookmarks::rootPath()
{
    return fileSystemModel.rootPath();
}

QModelIndex TreeViewSourceBookmarks::indexByPath(const QString &path) const
{
    return mapFromSource(
                fileSystemModel.index(path, COLUMN_NAME)
                );
}

Bookmark TreeViewSourceBookmarks::readBookmark(QString bookmarkFilePath) const
{
    return Bookmark(bookmarkFilePath);
}

Bookmark TreeViewSourceBookmarks::getSelectedBookmark(int selectionRow, QModelIndex selectionParent) const
{
    QModelIndex selectionIndex = index(selectionRow, COLUMN_NAME, selectionParent);
    if (!selectionIndex.isValid()) {
        return Bookmark();
    }

    if (isFolder(selectionIndex)) {
        return Bookmark();
    }

    return readBookmark(
                fileSystemModel.filePath(
                    mapToSource(
                        index(selectionRow, COLUMN_NAME, selectionParent)
                        )
                    )
                );
}

void TreeViewSourceBookmarks::notifyUpdate()
{
    QModelIndex firstCell = index(0, COLUMN_NAME);
    QModelIndex root = firstCell.parent();
    int rc = rowCount(root);
    QModelIndex lastCell = index(rc, 1);
    emit dataChanged(firstCell, lastCell);
}

bool TreeViewSourceBookmarks::isFolder(QModelIndex index) const
{
    return fileSystemModel.isDir(mapToSource(index));
}

void TreeViewSourceBookmarks::saveBookmark(Bookmark bookmark)
{
    QString sourcePath = bookmark.getSourcePath();
    qInfo() << "Writing bookmark to file" << sourcePath;
    QFile bookmarkFile(sourcePath);
    bookmarkFile.open(QFile::WriteOnly | QFile::Text);
    bookmarkFile.write(
                QString("%1\n%2\n%3")
                .arg(bookmark.getTargetPath())
                .arg(bookmark.getName())
                .arg(bookmark.getSeconds())
                .toUtf8()
                );
}

void TreeViewSourceBookmarks::deleteBookmark(Bookmark bookmark)
{
    QString sourcePath = bookmark.getSourcePath();
    qInfo() << "Deleting bookmark file" << sourcePath;
    QFile bookmarkFile(sourcePath);
    bookmarkFile.remove();
}

void TreeViewSourceBookmarks::addBookmark(
        QString targetPath,
        QString name,
        int seconds
        )
{
    QString sourcePath = makeBookmarkFilePath(targetPath);
    saveBookmark(
                Bookmark(
                     sourcePath,
                     targetPath,
                     name,
                     seconds
                     )
                );
    notifyUpdate();
}

bool TreeViewSourceBookmarks::isFolder(int sourceRow, const QModelIndex &sourceParent) const
{
    return fileSystemModel.isDir(sourceModel()->index(sourceRow, COLUMN_NAME, sourceParent));
}

bool TreeViewSourceBookmarks::filterAcceptsRow(
        int sourceRow,
        const QModelIndex &sourceParent
        ) const {
    if (isFolder(sourceRow, sourceParent)) {
        QString folderPath = fileSystemModel.filePath(
                    sourceModel()->index(
                        sourceRow,
                        COLUMN_NAME,
                        sourceParent
                        )
                    );

        return folderHasBookmarks(folderPath);
    }

    QModelIndex indexName = sourceModel()->index(sourceRow, COLUMN_NAME, sourceParent);
    QString fileName = sourceModel()->data(indexName).toString();

    return fileName.startsWith(FILE_PREFIX_BOOKMARK);
}

QVariant TreeViewSourceBookmarks::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case COLUMN_TIME:
            return "Time";
        }
    }

    return QSortFilterProxyModel::headerData(section, orientation, role);
}

QString TreeViewSourceBookmarks::getPathForIndex(const QModelIndex *index) const
{
    return fileSystemModel.filePath(
                mapToSource(*index)
                );
}


QVariant TreeViewSourceBookmarks::dataForColumnName(const QModelIndex &index) const
{
    return readBookmark(getPathForIndex(&index)).getName();
}

Bookmark TreeViewSourceBookmarks::getBookmarkForIndex(QModelIndex index) const
{
    return readBookmark(
                getPathForIndex(
                    &index
                    )
                );
}

QVariant TreeViewSourceBookmarks::dataForColumnTime(const QModelIndex &index) const
{
    return Utils::formatTime(
                getBookmarkForIndex(index).getSeconds()
                );
}

void TreeViewSourceBookmarks::onDirectoryLoaded(const QString &path)
{
    emit directoryLoaded(path);
}

bool TreeViewSourceBookmarks::folderHasBookmarks(QString folderPath) const
{
    QDir dir(folderPath);
    QStringList nameFilters;
    nameFilters << FILE_PREFIX_BOOKMARK + "*";
    QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::NoDotAndDotDot|QDir::AllDirs|QDir::Files|QDir::Hidden);

    for (int i = 0; i < fileInfoList.size(); i++) {
        QFileInfo fileInfo = fileInfoList.at(i);
        if (fileInfo.isDir()) {
            if (!fileInfo.absoluteFilePath().startsWith(Settings::getLibraryPath()))
                return true; //Ignore wrong folders sent to our way due to internal issues of Qt!

            if (folderHasBookmarks(fileInfo.absoluteFilePath()))
                return true;
        } else {
            return true;
        }
    }

    return false;
}

QString TreeViewSourceBookmarks::makeBookmarkFilePath(QString targetPath)
{
    QString result;
    int counter = 0;
    do {
        QFileInfo target(targetPath);
        result = target.absolutePath()
                + QDir::separator()
                + FILE_PREFIX_BOOKMARK
                + QString::number(counter++)
                + "." + target.fileName();
    } while (QFileInfo(result).exists());

    return result;
}

bool TreeViewSourceBookmarks::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (left.column() != COLUMN_NAME || isFolder(left) || isFolder(right))
        return QSortFilterProxyModel::lessThan(left, right);

    Bookmark leftBookmark = readBookmark(fileSystemModel.filePath(left));
    Bookmark rightBookmark = readBookmark(fileSystemModel.filePath(right));

    QString leftName = leftBookmark.getName();
    QString rightName = rightBookmark.getName();

    int stringCompare = leftName.compare(rightName);
    if (stringCompare != 0)
        return stringCompare < 0;

    int leftSeconds = leftBookmark.getSeconds();
    int rightSeconds = rightBookmark.getSeconds();

    return leftSeconds < rightSeconds;
}

QVariant TreeViewSourceBookmarks::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (isFolder(index))
            return QSortFilterProxyModel::data(index, role);

        switch(index.column()) {
        case COLUMN_NAME:
            return dataForColumnName(index);
        case COLUMN_TIME:
            return dataForColumnTime(index);
        }
    }

    return QSortFilterProxyModel::data(index, role);
}
