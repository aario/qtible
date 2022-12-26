#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mediaplayer/mediaplayer.h"
#include "sleeptimer.h"

#include <QMainWindow>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAdd_folder_triggered();

    void on_treeViewBookmarks_doubleClicked(const QModelIndex &index);

    void on_action_refresh_triggered();

    void on_splitterMain_splitterMoved(int pos, int index);

    void on_actionRename_bookmark_triggered();

    void on_actionDelete_bookmark_triggered();

    void on_actionMark_as_finished_triggered();

    void on_actionReset_progress_triggered();

    void on_actionRemove_folder_triggered();

    void onTreeViewBookmarksContextMenu(const QPoint &point);

    void onTreeViewLibraryContextMenu(const QPoint &point);

    void onTreeViewLibraryOpen(QString path);

    void onMplayerBegan();

    void onMplayerFinished(QString currentPath);

    void onMplayerRewindToPrevious(QString currentPath);

    void on_pushButtonAddBookMarkWithIcon_clicked();

    void onMplayerAddBookmark(QString path, QString name, int seconds);

    void on_tabWidgetLeft_currentChanged(int index);

    void on_actionE_xigt_triggered();

    void on_actionAbout_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionDonate_triggered();

    void on_pushButtonStopAfterCurrent_clicked();

private:
    const QString APPLICATION_TITLE = "Qtible";
    const QString DONATION_URL = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=UJTM2GDGPEFHA";

    Ui::MainWindow *ui;
    MediaPlayer *mplayer = NULL;
    SleepTimer *sleepTimer = NULL;
    QMenu contextMenuBookmarks;
    QMenu contextMenuLibrary;
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
    void openMedia(
            QString path = "",
            int Progress = 0,
            int total = 0);
    void closeMedia();
    void resizeEvent(QResizeEvent *event);
    void play(QString path, int progress = -1);
    void adjustWidgetSizes();
};
#endif // MAINWINDOW_H
