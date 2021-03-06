#include "appmanager.h"

#include <QDir>
#include <QFile>
#include <QSettings>

static const QString APP_MANIFESTS_DIR("/usr/share/applications/");

AppManager::AppManager()
{
    loadApplications();
}

std::list<AppManager::AppInfo> AppManager::applicationList()
{
    return m_applicationList;
}

void AppManager::loadApplications()
{
    // Open application mainfests directory
    QDir appsDir(APP_MANIFESTS_DIR);
    if (!appsDir.exists()) {
        qWarning("Application directory %s not found", APP_MANIFESTS_DIR.toStdString().c_str());
        return;
    }

    // Read each desktopfile file
    QStringList nameFilter;
    nameFilter << "*.desktop";
    for (QFileInfo &fileInfo : appsDir.entryInfoList(nameFilter)) {
        QSettings desktopfile(fileInfo.absoluteFilePath(), QSettings::IniFormat);

        desktopfile.beginGroup("Desktop Entry");
        AppInfo appInfo;
        appInfo.name = desktopfile.value("Name").toString();
        appInfo.icon = desktopfile.value("Icon").toString();
        appInfo.appID = fileInfo.absoluteFilePath();
        appInfo.exec = desktopfile.value("Exec").toString();
        desktopfile.endGroup();

        if(!isAppInfoComplete(appInfo)) {
            qWarning("Manifest file incomplete: %s", desktopfile.fileName().toStdString().c_str());
            continue;
        }

        m_applicationList.push_back(appInfo);
    }
}

bool AppManager::isAppInfoComplete(const AppManager::AppInfo &appInfo) const
{
    if (appInfo.appID.isEmpty()) {
        return false;
    }

    if (appInfo.name.isEmpty()) {
        return false;
    }

    if (appInfo.exec.isEmpty()) {
        return false;
    }

    return true;
}

bool AppManager::appExists(const QString &appID) const
{
    return isAppInfoComplete(appInfoFromAppID(appID));
}

AppManager::AppInfo AppManager::appInfoFromAppID(const QString &appID) const
{
    auto it = m_applicationList.begin();
    for (; it != m_applicationList.end(); ++it) {
        if (it->appID == appID)
            return *it;
    }

    return AppManager::AppInfo();
}

AppManager::AppInfo AppManager::appInfoFromExec(const QString &exec) const
{
    auto it = m_applicationList.begin();
    for(; it != m_applicationList.end(); ++it) {
        if (it->exec == exec)
            return *it;
    }

    return AppManager::AppInfo();
}
