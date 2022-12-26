#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QProgressDialog>
#include <QTime>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(APPLICATION_TITLE);
    ui->tabWidgetLeft->setCurrentIndex(0);
    ui->tabWidgetRight->setCurrentIndex(0);
    ui->lcdNumberPlayerStatus->display("--:-- : --:--");
    contextMenuBookmarks.addAction(ui->actionRename_bookmark);
    contextMenuBookmarks.addAction(ui->actionDelete_bookmark);
    ui->treeViewBookmarks->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
                ui->treeViewBookmarks,
                SIGNAL(customContextMenuRequested(const QPoint &)),
                this,
                SLOT(onTreeViewBookmarksContextMenu(const QPoint &))
                );
    contextMenuLibrary.addAction(ui->actionReset_progress);
    contextMenuLibrary.addAction(ui->actionMark_as_finished);
    contextMenuLibrary.addAction(ui->actionRemove_folder);
    ui->treeViewLibrary->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
                ui->treeViewLibrary,
                SIGNAL(customContextMenuRequested(const QPoint &)),
                this,
                SLOT(onTreeViewLibraryContextMenu(const QPoint &))
                );
    connect(
                ui->treeViewLibrary,
                SIGNAL(open(QString)),
                this,
                SLOT(onTreeViewLibraryOpen(QString))
                );
    mplayer = new MediaPlayer(
                this,
                ui->horizontalSliderVolume,
                ui->labelCurrentVolume,
                ui->pushButtonVolumeMute,
                ui->horizontalSliderSpeed,
                ui->labelCurrentSpeed,
                ui->pushButtonSpeedReset,
                ui->labelSeekBar,
                ui->lcdNumberPlayerStatus,
                ui->pushButtonPlayerRewind,
                ui->pushButtonPlayerPlayPause,
                ui->pushButtonPlayerFastForward,
                ui->labelPlayerTitle,
                ui->pushButtonAddBookmark,
                ui->pushButtonStopAfterCurrentWithIcon
                );
    connect(
                mplayer,
                &MediaPlayer::progressUpdate,
                ui->treeViewLibrary,
                &TreeViewLibrary::onMplayerProgressUpdate
                );
    connect(
                mplayer,
                &MediaPlayer::began,
                this,
                &MainWindow::onMplayerBegan
                );
    connect(
                mplayer,
                &MediaPlayer::finished,
                this,
                &MainWindow::onMplayerFinished
                );
    connect(
                mplayer,
                &MediaPlayer::rewindToPrevious,
                this,
                &MainWindow::onMplayerRewindToPrevious
                );
    connect(
                mplayer,
                &MediaPlayer::addBookmark,
                this,
                &MainWindow::onMplayerAddBookmark
                );

    sleepTimer = new SleepTimer(
                this,
                ui->pushButtonTimer30m,
                ui->pushButtonTimer10m,
                ui->pushButtonTimer5m,
                ui->pushButtonTimer30s,
                ui->pushButtonTimer5s,
                ui->pushButtonTimer00s,
                ui->lcdNumberSleepTimer
                );
    connect(
                sleepTimer,
                &SleepTimer::cancelled,
                mplayer,
                &MediaPlayer::onSleepTimerCancelled
                );
    connect(
                sleepTimer,
                &SleepTimer::finished,
                mplayer,
                &MediaPlayer::onSleepTimerFinished
                );
    connect(
                sleepTimer,
                &SleepTimer::volumeFadeOut,
                mplayer,
                &MediaPlayer::onSleepTimerVolumeFadeOut
                );
}

void MainWindow::adjustWidgetSizes()
{
    ui->treeViewLibrary->resizeColumns();
    ui->treeViewBookmarks->resizeColumns();
}

void MainWindow::showEvent( QShowEvent* event ) {
    QWidget::showEvent( event );

    QProgressDialog d;
    d.setLabelText("Initializing database. Please wait...");
    d.setMinimum(0);
    d.setMaximum(0);
    d.setValue(0);
    d.show();

    adjustWidgetSizes();

    d.hide();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    adjustWidgetSizes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openMedia(
        QString path,
        int progress,
        int total
        )
{
    if (mplayer->isLoaded() && mplayer->getPath() == path) {
        if (mplayer->currentPosition(total) != progress)
            mplayer->onLabelSeekBarSought(progress, total);

        if (!mplayer->isPlaying())
            mplayer->play();

        return;
    }

    mplayer->open(
                path,
                progress,
                total
                );
}

void MainWindow::on_actionAdd_folder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Add Folder"),
                QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                QFileDialog::ShowDirsOnly
                | QFileDialog::DontResolveSymlinks
                );
    ui->treeViewLibrary->add(dir);
}

void MainWindow::play(QString path, int progress)
{
    Progress* p = ui->treeViewLibrary->getProgressByPath(path);
    if (progress >= 0) {
        p->setProgress(progress);
    }

    ui->treeViewLibrary->goToPath(path);

    openMedia(
                path,
                p->getProgress(),
                p->getTotal()
                );
}

void MainWindow::on_treeViewBookmarks_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    Bookmark bookmark = ui->treeViewBookmarks->getSelectedBookmark();
    QString path = bookmark.getTargetPath();
    if (path.isEmpty()) {
        return;
    }

    ui->treeViewLibrary->collapseAll();
    play(path, bookmark.getSeconds());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    if (mplayer->isLoaded())
        mplayer->stop();
    ui->treeViewLibrary->close();
}

void MainWindow::on_action_refresh_triggered()
{
    ui->treeViewLibrary->refresh();
}

void MainWindow::on_splitterMain_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);
    adjustWidgetSizes();
}

void MainWindow::onTreeViewBookmarksContextMenu(const QPoint &point)
{
    QModelIndex index = ui->treeViewBookmarks->indexAt(point);
    if (index.isValid() && index.column() == 0 && !ui->treeViewBookmarks->isFolder(index)) {
        contextMenuBookmarks.exec(ui->treeViewBookmarks->viewport()->mapToGlobal(point));
    }
}

void MainWindow::onTreeViewLibraryContextMenu(const QPoint &point)
{
    QModelIndex index = ui->treeViewLibrary->indexAt(point);
    ui->actionRemove_folder->setEnabled(ui->treeViewLibrary->isInRootFolder(index));
    if (index.isValid() && index.column() == 0) {
        contextMenuLibrary.exec(ui->treeViewLibrary->viewport()->mapToGlobal(point));
    }
}

void MainWindow::onTreeViewLibraryOpen(QString path)
{
    play(path);
}

void MainWindow::on_actionRename_bookmark_triggered()
{
    ui->treeViewBookmarks->renameSelectedBookmark();
}

void MainWindow::on_actionDelete_bookmark_triggered()
{
    ui->treeViewBookmarks->deleteSelectedBookmark();
}

void MainWindow::on_actionMark_as_finished_triggered()
{
    ui->treeViewLibrary->setResetProgressOfSelection(false);
}

void MainWindow::on_actionReset_progress_triggered()
{
    ui->treeViewLibrary->setResetProgressOfSelection(true);
}

void MainWindow::on_actionRemove_folder_triggered()
{
    ui->treeViewLibrary->removeSelectedFolder();
}

void MainWindow::onMplayerBegan()
{
    setWindowTitle(APPLICATION_TITLE + " | " + mplayer->getTitle());
    ui->pushButtonAddBookMarkWithIcon->setEnabled(true);
    ui->pushButtonStopAfterCurrent->setEnabled(true);
}

void MainWindow::onMplayerFinished(QString currentPath)
{
    setWindowTitle(APPLICATION_TITLE);
    if (this->mplayer->shouldStopAfterCurrent()) {
        return;
    }
    QString nextPath = ui->treeViewLibrary->getNextFilePath(currentPath);
    if (nextPath.isEmpty()) {
        return;
    }

    ui->pushButtonAddBookMarkWithIcon->setEnabled(false);
    ui->pushButtonStopAfterCurrentWithIcon->setEnabled(false);

    play(nextPath, 0);
}

void MainWindow::onMplayerRewindToPrevious(QString currentPath)
{
    QString previousPath = ui->treeViewLibrary->getPreviousFilePath(currentPath);
    if (previousPath.isEmpty()) {
        play(currentPath, 0);
        return;
    }

    Progress *previousProgress = ui->treeViewLibrary->getProgressByPath(previousPath);
    play(previousPath, previousProgress->getTotal() - MediaPlayer::SKIP_SECONDS);
}

void MainWindow::onMplayerAddBookmark(
        QString path,
        QString name,
        int seconds
        )
{
    ui->treeViewBookmarks->addBookmark(
                path,
                name,
                seconds
                );
}

void MainWindow::on_pushButtonAddBookMarkWithIcon_clicked()
{
    emit ui->pushButtonAddBookmark->clicked();
}

void MainWindow::on_pushButtonStopAfterCurrent_clicked()
{
    ui->pushButtonStopAfterCurrentWithIcon->setChecked(!ui->pushButtonStopAfterCurrentWithIcon->isChecked());
    emit ui->pushButtonStopAfterCurrentWithIcon->clicked();
}

void MainWindow::on_tabWidgetLeft_currentChanged(int index)
{
    Q_UNUSED(index)
    adjustWidgetSizes();
}

void MainWindow::on_actionE_xigt_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(
                this,
                "About Qtible",
                "<h1>Qtible</h1>"
                "Qtible is the world's best audio book player, running native on Linux, based on Qt.<br/>"
                "<br/>"
                "Copyright 2021 Aario Shahbany<br/>"
                "<br/>"
                "This program is distributed in the hope that it will be useful,"
                "but WITHOUT ANY WARRANTY; without even the implied warranty of"
                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
                "GNU General Public License for more details.<br/>"
                );
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionDonate_triggered()
{
    QDesktopServices::openUrl(QUrl(DONATION_URL));
}
