#include "settings.h"

#include <QDir>
#include <QStandardPaths>

const QString Settings::CONFIG_FILE_NAME = "config.ini";
const QString Settings::LIBRARY_FOLDER_NAME = "library";

Settings::Settings(): QSettings()
{
    QString filePath = getFilePath();
    QDir dir(filePath);
    if (!dir.exists(dir.path()))
        dir.mkpath(dir.path());
    setPath(
                IniFormat,
                UserScope,
                getFilePath()
                );
}

const QString Settings::getConfigFolder()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

const QString Settings::getFilePath()
{
    return getConfigFolder() + QDir::separator() + CONFIG_FILE_NAME;
}

const QString Settings::getLibraryPath()
{
    return getConfigFolder() + QDir::separator() + LIBRARY_FOLDER_NAME;
}
