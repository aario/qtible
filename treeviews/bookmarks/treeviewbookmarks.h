#ifndef TREEVIEWBOOKMARKS_H
#define TREEVIEWBOOKMARKS_H

#include <QTreeView>
#include "treeviews/bookmarks/treeviewsourcebookmarks.h"
#include "models/bookmark.h"

class TreeViewBookmarks : public QTreeView
{
public:
    TreeViewBookmarks(QWidget *parent);
    Bookmark getSelectedBookmark();
    void resizeColumns();
    void addBookmark(
            QString path,
            QString name,
            int seconds
            );
    bool isFolder(QModelIndex index);
    void renameSelectedBookmark();
    void deleteSelectedBookmark();
private:
    const int COLUMN_WIDTH_PERCENT_NAME = 75;
    const int COLUMN_WIDTH_PERCENT_TIME = 20;
    TreeViewSourceBookmarks bookmarks;
};

#endif // TREEVIEWBOOKMARKS_H
