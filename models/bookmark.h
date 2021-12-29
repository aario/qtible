#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QString>

class Bookmark
{
public:
    Bookmark();
    Bookmark(QString sourcePath);
    Bookmark(
            QString sourcePath,
            QString targetPath,
            QString name,
            int seconds
            );
    QString getSourcePath();
    QString getTargetPath();
    void setName(QString name);
    QString getName();
    int getSeconds();

private:
    QString sourcePath;
    QString targetPath;
    QString name;
    int seconds;
};

#endif // BOOKMARK_H
