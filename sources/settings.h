#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* self();

    QString getTheme();
    void setTheme(QString& theme);

    QString getLevelset();
    void setLevelset(QString& theme);
    
signals:
    void themeChanged();
    void levelsetChanged();
    
public slots:

private:
    Settings();

    Settings* instance;

    QSettings qsettings;
};

#endif // SETTINGS_H
