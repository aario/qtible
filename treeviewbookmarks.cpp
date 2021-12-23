#include "treeviewbookmarks.h"

#include <QDebug>
#include <QEventLoop>
#include <QInputDialog>
#include <QMessageBox>

TreeViewBookmarks::TreeViewBookmarks(QWidget *parent): QTreeView(parent)
{
    setModel(&bookmarks);
    setRootIndex(bookmarks.indexByPath(bookmarks.rootPath()));
    hideColumn(1);
    hideColumn(2);
    setSortingEnabled(true);
    sortByColumn(Bookmarks::COLUMN_NAME, Qt::SortOrder::AscendingOrder);
}

Bookmark TreeViewBookmarks::getSelectedBookmark()
{
    QModelIndex index = selectedIndexes()[Bookmarks::COLUMN_NAME];
    int row = index.row();
    QModelIndex parent = index.parent();

    return bookmarks.getSelectedBookmark(row, parent); //bookmarks.getSelectedFilePath();
}

void TreeViewBookmarks::resizeColumns()
{
    int w = width();
    int columnNameWidth = w * COLUMN_WIDTH_PERCENT_NAME / 100;
    int columnTimeWidth = w * COLUMN_WIDTH_PERCENT_TIME / 100;
    qInfo() << "Resize: " << w << columnNameWidth << columnTimeWidth;
    setColumnWidth(Bookmarks::COLUMN_NAME, columnNameWidth);
    setColumnWidth(Bookmarks::COLUMN_TIME, columnTimeWidth);
}

void TreeViewBookmarks::addBookmark(
        QString path,
        QString name,
        int seconds
        )
{
    bookmarks.addBookmark(
                path,
                name,
                seconds
                );
}

bool TreeViewBookmarks::isFolder(QModelIndex index)
{
    return bookmarks.isFolder(index);
}

void TreeViewBookmarks::renameSelectedBookmark()
{
    QModelIndex index = selectedIndexes()[Bookmarks::COLUMN_NAME];
    Bookmark bookmark = bookmarks.getSelectedBookmark(index.row(), index.parent());
    bool ok;
    QString name = QInputDialog::getText(
                    this,
                    "Rename bookmark",
                    "Name:",
                    QLineEdit::Normal,
                    bookmark.getName(),
                    &ok
                    );

    if (!ok)
        return;

    bookmark.setName(name);
    bookmarks.saveBookmark(bookmark);
}

void TreeViewBookmarks::deleteSelectedBookmark()
{
    QString title = "Delete bookmark";

    if (QMessageBox::question(
                this,
                title,
                "Are you sure you want to " + title.toLower() + "?"
                ) != QMessageBox::Yes)
        return;

    QModelIndex index = selectedIndexes()[Bookmarks::COLUMN_NAME];
    Bookmark bookmark = bookmarks.getSelectedBookmark(index.row(), index.parent());

    bookmarks.deleteBookmark(bookmark);
}
