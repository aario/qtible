#include "library.h"
#include "utils.h"

#include <QDir>
#include <QDebug>
#include <QVariant>
#include <QFileInfo>
#include <QProcess>
#include <QChar>
#include "mediaplayer.h"

Library::Library():
    QSortFilterProxyModel(),
    EXTENSIONS_SUPPORTED {
        "aac",
        "wmv",
        "avi",
        "mpeg",
        "mov",
        "3gp",
        "flv",
        "mp3",
        "mp4",
        "flac",
        "ogg",
        },
    fileSystemModel()
{
    QDir dir(Settings::getLibraryPath());
    if (!dir.exists(dir.path()))
        dir.mkpath(dir.path());

    fileSystemModel.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    fileSystemModel.setRootPath(dir.path());

    setSourceModel(&fileSystemModel);

    timerSaveProgressCache.setInterval(SAVE_PROGRESS_CACHE_INTERVAL);
    timerSaveProgressCache.setTimerType(Qt::PreciseTimer);
    QObject::connect(
                &timerSaveProgressCache,
                &QTimer::timeout,
                this,
                &Library::saveProgressCache,
                Qt::DirectConnection
                );
    QObject::connect(
                &fileSystemModel,
                &QFileSystemModel::directoryLoaded,
                this,
                &Library::onDirectoryLoaded
                );
    timerSaveProgressCache.start();
}

QString Library::rootPath()
{
    return fileSystemModel.rootPath();
}

QModelIndex Library::indexByPath(const QString &path) const
{
    return mapFromSource(
                fileSystemModel.index(path, COLUMN_PATH)
                );
}

QString Library::pathByIndex(const QModelIndex &index) const
{
    return fileSystemModel.filePath(
                    mapToSource(
                        index
                        )
                    );
}

QString Library::getSelectedFilePath(int selectionRow, QModelIndex selectionParent) const
{
    QModelIndex selectionIndex = index(selectionRow, COLUMN_PATH, selectionParent);
    if (!selectionIndex.isValid()) {
        return "";
    }

    if (isFolder(selectionIndex)) {
        return "";
    }

    return fileSystemModel.filePath(
                mapToSource(
                    index(selectionRow, COLUMN_PATH, selectionParent)
                    )
                );
}

void Library::notifyUpdate()
{
    QModelIndex firstCell = index(0, COLUMN_PATH);
    QModelIndex root = firstCell.parent();
    int rc = rowCount(root);
    QModelIndex lastCell = index(rc, 1);
    emit dataChanged(firstCell, lastCell);
}

void Library::refresh()
{
    progressCache.clear();
    notifyUpdate();
}

bool Library::isFolder(QModelIndex index) const
{
    return fileSystemModel.isDir(mapToSource(index));
}

bool Library::isFolder(int sourceRow, const QModelIndex &sourceParent) const
{
    return fileSystemModel.isDir(sourceModel()->index(sourceRow, COLUMN_PATH, sourceParent));
}

bool Library::filterAcceptsRow(
        int sourceRow,
        const QModelIndex &sourceParent
        ) const {
    if (isFolder(sourceRow, sourceParent)) {
        return true;
    }

    QModelIndex indexName = sourceModel()->index(sourceRow, COLUMN_PATH, sourceParent);
    QString fileName = sourceModel()->data(indexName).toString();

    foreach (QString extension, EXTENSIONS_SUPPORTED) {
        if (fileName.endsWith(extension)) {
            return true;
        }
    }

    return false;
}

QVariant Library::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case COLUMN_LENGTH:
            return "Length";
        case COLUMN_PROGRESS:
            return "Progress";
        }
    }

    return QSortFilterProxyModel::headerData(section, orientation, role);
}

QString Library::getPathForIndex(const QModelIndex *index) const
{
    return fileSystemModel.filePath(
                mapToSource(*index)
                );
}

int Library::getFileLength(QString filePath) const
{
    qInfo() << "Calculating length for" << filePath;
    QProcess ffprobe;
    QStringList arguments;
    arguments << "-i" << filePath << "-show_format" << "-v" << "quiet";

    ffprobe.start("ffprobe", arguments);
    ffprobe.waitForFinished();

    QString ffprobeOutput = ffprobe.readAll();
    QStringList ffprobeOutputLines = ffprobeOutput.split("\n");
    QStringList durationLines = ffprobeOutputLines.filter("duration=");
    if (durationLines.isEmpty()) {
        qWarning() << "Could not read duration of file" << filePath;

        return 0;
    }

    QString durationLine = durationLines.first();
    QStringList durationLineParts = durationLine.split("=");
    if (durationLineParts.count() < 2) {
        qWarning() << "Could not read duration of file" << filePath;

        return 0;
    }

    QString durationValue = durationLineParts[1];
    float resultFloat = durationValue.toFloat();
    int result = (int) resultFloat;

    return result;
}

QString Library::getProgressFilePath(QFileInfo fileInfo) const
{
    return fileInfo.path()
            + QDir::separator()
            + FILE_PREFIX_PROGRESS
            + fileInfo.fileName();
}

void Library::readProgressFile(QString progressFilePath, Progress *progress) const
{
    qInfo() << "Reading progress file" << progressFilePath;
    QFile progressFile(progressFilePath);
    progressFile.open(QFile::ReadOnly | QFile::Text);
    QString progressString = progressFile.readAll();
    QStringList progressStringParts = progressString.split(':');
    progress->setProgress(progressStringParts[0].toInt());
    progress->setTotal(progressStringParts[1].toInt());
}

void Library::saveProgressFile(QFileInfo fileInfo, Progress *progress) const
{
    if (!progress->isChanged())
        return;

    QString progressFilePath = getProgressFilePath(fileInfo);
    qInfo() << "Writing progress to file" << progressFilePath;
    QFile progressFile(progressFilePath);
    progressFile.open(QFile::WriteOnly | QFile::Text);
    progressFile.write(
                QString("%1:%2")
                .arg(progress->getProgress())
                .arg(progress->getTotal()
                     ).toUtf8());
    progress->setChanged(false);
}

void Library::scanProgressInChildren(const QModelIndex *index, Progress *progress)
{
    QModelIndex child;
    int row = 0;
    while (1) {
        child = Library::index(row, COLUMN_PATH, *index);
        if (!child.isValid())
            break;
        Progress *childProgress = getProgress(&child);
        progress->setProgress(progress->getProgress() + childProgress->getProgress());
        progress->setTotal(progress->getTotal() + childProgress->getTotal());
        row++;
    }
}

Progress *Library::getProgress(const QModelIndex *index)
{
    QString filePath = getPathForIndex(index);

    return getProgress(filePath);
}

void Library::setProgress(QString path, int progress)
{
    Progress *p = progressCache.value(path);
    p->setProgress(progress);
    notifyUpdate();
}

void Library::setResetProgress(QModelIndex index, bool reset)
{
    if (!isFolder(index)) {
        Progress *progress = getProgress(&index);
        progress->setProgress(reset ? 0 : progress->getTotal());

        saveProgressCache();
        notifyUpdate();
        return;
    }

    QModelIndex child;
    int row = 0;
    while (1) {
        child = Library::index(row, COLUMN_PATH, index);
        if (!child.isValid())
            break;

        if (isFolder(child))
            setResetProgress(child, reset);

        Progress *childProgress = getProgress(&child);
        childProgress->setProgress(reset ? 0 : childProgress->getTotal());
        row++;
    }

    saveProgressCache();
    notifyUpdate();
}

Progress *Library::getProgress(QString filePath)
{
    if (progressCache.contains(filePath))
        return progressCache.value(filePath);

    Progress *progress = new Progress();
    progressCache.insert(filePath, progress);

    if (QFileInfo(filePath).isDir()) {
        QModelIndex index = indexByPath(filePath);
        scanProgressInChildren(&index, progress);
        return progress;
    }

    QFileInfo fileInfo(filePath);
    QString progressFilePath = getProgressFilePath(fileInfo);
    if (QFileInfo::exists(progressFilePath)) {
        readProgressFile(progressFilePath, progress);
        return progress;
    }

    int fileLength = getFileLength(filePath);
    progress->setTotal(fileLength);
    saveProgressFile(fileInfo, progress);

    return progress;
}

QString Library::convertStringForSortScore(QString string) const
{
    QStringList nameParts;
    bool lastPartWasNumeric = false;
    for (int i = 0; i < string .size(); i++) {
        QChar character = string .at(i);
        if (nameParts.isEmpty()) {
            nameParts.append(character);
            continue;
        }

        if (character.isDigit()) {
            if (lastPartWasNumeric) {
                nameParts.replace(
                            nameParts.count() - 1,
                            nameParts.last() + character
                            );
            } else {
                nameParts.append(character);
                lastPartWasNumeric = true;
            }
        } else {
            if (lastPartWasNumeric) {
                nameParts.replace(
                            nameParts.count() -1,
                            QString::number(
                                nameParts.last().toInt()
                                ).rightJustified(SORT_SCORE_LEADING_ZEROS, '0')
                            );
                nameParts.append(character);
                lastPartWasNumeric = false;
            } else {
                nameParts.replace(
                            nameParts.count() - 1,
                            nameParts.last() + character
                            );
            }
        }
    }
    QString result = nameParts.join("");

    return result;
}

bool Library::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (left.column() != COLUMN_PATH)
        return QSortFilterProxyModel::lessThan(left, right);

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    QString leftString = leftData.toString();
    QString rightString = rightData.toString();

    QString leftStringForSortScore = convertStringForSortScore(leftString);
    QString rightStringForSortScore = convertStringForSortScore(rightString);

    return leftStringForSortScore.compare(rightStringForSortScore) < 0;
}

QVariant Library::dataForColumnLength(const QModelIndex &index) const
{
    Progress *progress = const_cast<Library *>(this)->getProgress(&index);
    if (progress->getTotal() == 0)
        return -1; //There has been a problem calculating total

    return Utils::formatTime(progress->getTotal());
}

QVariant Library::dataForColumnProgress(const QModelIndex &index) const
{
    Progress *progress = const_cast<Library *>(this)->getProgress(&index);
    if (progress->getTotal() == 0)
        return -1; //There has been a problem calculating total

    return progress->getProgressPercent();
}

void Library::onDirectoryLoaded(const QString &path)
{
    if (!directoriesLoaded.contains(path))
        directoriesLoaded.append(path);

    emit directoryLoaded(path);
}

bool Library::directoryIsLoaded(const QString path)
{
    return directoriesLoaded.contains(path);
}

QVariant Library::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch(index.column()) {
        case COLUMN_LENGTH:
            return dataForColumnLength(index);
        case COLUMN_PROGRESS:
            return dataForColumnProgress(index);
        }
    }

    return QSortFilterProxyModel::data(index, role);
}

void Library::saveProgressCache()
{
    qInfo() << "Storing progress cache";
    for (auto filePath : progressCache.keys()) {
        saveProgressFile(QFileInfo(filePath), progressCache.value(filePath));
    }
}

QString Library::getNextFilePath(QString currentPath)
{
    QModelIndex currentIndex = indexByPath(currentPath);
    int row = currentIndex.row();
    QModelIndex currentParent = currentIndex.parent();

    return getSelectedFilePath(++row, currentParent);
}

void Library::add(QString path)
{
    QString destination = Settings::getLibraryPath() + QDir::separator() + QFileInfo(path).fileName();
    qInfo() << "Making symlink from" << path << " to " << destination;
    QFile::link(path, destination);
}
