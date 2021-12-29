#ifndef TREEVIEWLIBRARY_H
#define TREEVIEWLIBRARY_H

#include <QKeyEvent>
#include <QTreeView>
#include "treeviews/library/treeviewsourcelibrary.h"
#include "mediaplayer/mediaplayer.h"

class TreeViewLibrary : public QTreeView
{
    Q_OBJECT
public:
    TreeViewLibrary(QWidget *parent);
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
signals:
    void open(QString);
private:
    const int COLUMN_WIDTH_PERCENT_PATH = 45;
    const int COLUMN_WIDTH_PERCENT_LENGTH = 25;
    const int COLUMN_WIDTH_PERCENT_PROGRESS = 25;
    TreeViewSourceLibrary library;
    QString getSelectedFilePath();
    void openSelected();
private slots:
    void keyPressEvent(QKeyEvent *event);
    void onDoubleClicked(const QModelIndex &index);
};

#endif // TREEVIEWLIBRARY_H
