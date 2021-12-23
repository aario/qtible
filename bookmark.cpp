#include "bookmark.h"

#include <QDebug>
#include <QFile>
#include <QException>

Bookmark::Bookmark()
{
    this->targetPath = "";
    this->name = "";
    this->seconds = 0;
}

Bookmark::Bookmark(QString sourcePath)
{
    qInfo() << "Reading bookmark file" << sourcePath;
    QFile bookmarkFile(sourcePath);
    bookmarkFile.open(QFile::ReadOnly | QFile::Text);
    QString bookmarkString = bookmarkFile.readAll();
    QStringList bookmarkStringParts = bookmarkString.split('\n');
    if (bookmarkStringParts.size() != 3)
       qFatal("Corrupt bookmark file: %s", qPrintable(sourcePath));

    this->sourcePath = sourcePath;
    targetPath = bookmarkStringParts[0];
    name = bookmarkStringParts[1];
    seconds = bookmarkStringParts[2].toInt();
}

Bookmark::Bookmark(
        QString sourcePath,
        QString targetPath,
        QString name,
        int seconds
        )
{
    this->sourcePath = sourcePath;
    this->targetPath = targetPath;
    this->name = name;
    this->seconds = seconds;
}

QString Bookmark::getSourcePath()
{
    return sourcePath;
}

QString Bookmark::getTargetPath()
{
    return targetPath;
}

QString Bookmark::getName()
{
    return name;
}

void Bookmark::setName(QString name)
{
    this->name = name;
}

int Bookmark::getSeconds()
{
    return seconds;
}
