#ifndef TREEVIEWLIBRARY_H
#define TREEVIEWLIBRARY_H

#include <QTreeView>
#include <library.h>
#include "mediaplayer.h"

class TreeViewLibrary : public QTreeView
{
public:
    TreeViewLibrary(QWidget *parent);
    QString getSelectedFilePath();
    Progress *getProgressByPath(QString path);
    void refresh();
    void onMplayerProgressUpdate(QString path, int progress);
    void setResetProgressOfSelection(bool reset);
    QString getNextFilePath(QString currentPath);
    QString getPreviousFilePath(QString currentPath);
    void close();
    void resizeColumns();
    void goToPath(QString path);
    bool isInRootFolder(QModelIndex index);
    void add(QString path);
    void removeSelectedFolder();
private:
    const int COLUMN_WIDTH_PERCENT_PATH = 45;
    const int COLUMN_WIDTH_PERCENT_LENGTH = 25;
    const int COLUMN_WIDTH_PERCENT_PROGRESS = 25;
    Library library;
};

#endif // TREEVIEWLIBRARY_H
