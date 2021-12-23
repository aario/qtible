#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>

class Settings : public QSettings
{
public:
    Settings();

    static const QString getLibraryPath();
private:
    static const QString CONFIG_FILE_NAME;
    static const QString LIBRARY_FOLDER_NAME;
    static const QString getConfigFolder();
    const QString getFilePath();
};

#endif // SETTINGS_H
