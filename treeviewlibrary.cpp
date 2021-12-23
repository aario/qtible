#include "itemdelegatelibrary.h"
#include "treeviewlibrary.h"

#include <QApplication>
#include <QDebug>
#include <QEventLoop>
#include <QMessageBox>
#include <QTreeWidgetItemIterator>

TreeViewLibrary::TreeViewLibrary(QWidget *parent): QTreeView(parent)
{
    setModel(&library);
    setRootIndex(library.indexByPath(library.rootPath()));
    ItemDelegateLibrary *itemDelegateLibrary = new ItemDelegateLibrary(this);
    setItemDelegate(itemDelegateLibrary );
    setSortingEnabled(true);
    hideColumn(1);
    sortByColumn(Library::COLUMN_PATH, Qt::SortOrder::AscendingOrder);
}

QString TreeViewLibrary::getSelectedFilePath()
{
    QModelIndex index = selectedIndexes()[Library::COLUMN_PATH];
    int row = index.row();
    QModelIndex parent = index.parent();

    return library.getSelectedFilePath(row, parent); //library.getSelectedFilePath();
}

Progress *TreeViewLibrary::getProgressByPath(QString path)
{
    return library.getProgress(path);
}

void TreeViewLibrary::refresh()
{
    library.refresh();
}

void TreeViewLibrary::onMplayerProgressUpdate(QString path, int progress)
{
    library.setProgress(path, progress);
}

void TreeViewLibrary::setResetProgressOfSelection(bool reset)
{
    QString title = reset ? "Reset progress on all audio files" : "Mark all audio files as finished";

    if (QMessageBox::question(
                this,
                title,
                "Are you sure you want to " + title.toLower() + "?"
                ) != QMessageBox::Yes)
        return;

    QModelIndex index = selectedIndexes()[Library::COLUMN_PATH];

    if (library.isFolder(index) && !isExpanded(index)) {
        expand(index);
        if (!library.directoryIsLoaded(
                    library.pathByIndex(
                        index
                        )
                    )) {
            QEventLoop loop;
            QObject::connect(&library, &Library::directoryLoaded, &loop, &QEventLoop::quit);
            loop.exec();
        }
    }

    library.setResetProgress(index, reset);
}

QString TreeViewLibrary::getNextFilePath(QString currentPath)
{
    return library.getNextFilePath(currentPath);
}

QString TreeViewLibrary::getPreviousFilePath(QString currentPath)
{
    QModelIndex currentIndex = library.indexByPath(currentPath);
    int row = currentIndex.row();
    QModelIndex currentParent = currentIndex.parent();

    return library.getSelectedFilePath(--row, currentParent);
}

void TreeViewLibrary::close()
{
    library.saveProgressCache();
}

void TreeViewLibrary::resizeColumns()
{
    int w = width();
    int columnPathWidth = w * COLUMN_WIDTH_PERCENT_PATH / 100;
    int columnLengthWidth = w * COLUMN_WIDTH_PERCENT_LENGTH / 100;
    int columnProgressWidth = w * COLUMN_WIDTH_PERCENT_PROGRESS / 100;
    setColumnWidth(Library::COLUMN_PATH, columnPathWidth);
    setColumnWidth(Library::COLUMN_LENGTH, columnLengthWidth);
    setColumnWidth(Library::COLUMN_PROGRESS, columnProgressWidth);
}

void TreeViewLibrary::goToPath(QString path)
{
    QModelIndex index = library.indexByPath(path);
    scrollTo(index, PositionAtTop); //It expands the node, but it never scrolls to it.
    //  All the solutions I have tried also failed!
    //  Bug has been around since a decade ago!
    //  See https://bugreports.qt.io/browse/QTBUG-22689
    selectionModel()->clear();
    selectionModel()->select(library.indexByPath(path), QItemSelectionModel::Select);
}

bool TreeViewLibrary::isInRootFolder(QModelIndex index)
{
    return index.parent() == rootIndex();
}

void TreeViewLibrary::add(QString path)
{
    library.add(path);
}

void TreeViewLibrary::removeSelectedFolder()
{
    QString title = "Remove folder";

    if (QMessageBox::question(
                this,
                title,
                "Are you sure you want to " + title.toLower() + " from library?"
                ) != QMessageBox::Yes)
        return;

    QModelIndex index = selectedIndexes()[Library::COLUMN_PATH];
    QString path = library.pathByIndex(index);

    if (isInRootFolder(index) && QFileInfo(path).isSymLink()) {
        qInfo() << "Removing symlink from library:" << path;
        QFile::remove(path);
    }
}
