#include "settings.h"
#include "globals.h"

Settings* Settings::self()
{
    static Settings instance;
    return &instance;
}

Settings::Settings() :
    QObject(),
    qsettings(APP_NAME)
{
}

void Settings::setTheme(QString& theme)
{
    qsettings.setValue("game/theme", theme);
    emit themeChanged();
}

QString Settings::getTheme()
{
    return qsettings.contains("game/theme") ?
                qsettings.value("game/theme").toString() :
                DEFAULT_THEME;
}

void Settings::setLevelset(QString& levelset)
{
    qsettings.setValue("game/levelset", levelset);
    emit levelsetChanged();
}

QString Settings::getLevelset()
{
    return qsettings.contains("game/levelset") ?
                qsettings.value("game/levelset").toString() :
                DEFAULT_LEVELSET;
}
