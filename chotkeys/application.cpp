#include "application.h"
#include <QFileSystemWatcher>

Application::Application(QObject *parent)
    : QObject{parent}
{
    initSetting();
    QFileSystemWatcher *m_FileWatcher = new QFileSystemWatcher(this);
    m_FileWatcher->addPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/cuteglobalshortcutsrc");
    connect(m_FileWatcher, &QFileSystemWatcher::fileChanged, this, &Application::initSetting);
}
void Application::initSetting()
{
    cleanSetting();
    QFile file(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/cuteglobalshortcutsrc");
    QSettings setting(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/cuteglobalshortcutsrc", QSettings::IniFormat);
    if(!file.exists())
    {
        setting.beginGroup("Ctrl+Alt+A");
        setting.setValue("Comment","Screenshot");
        setting.setValue("Exec","cute-screenshot");
        setting.endGroup();

        setting.beginGroup("Meta+L");
        setting.setValue("Comment","Screenlocker");
        setting.setValue("Exec","cute-screenlocker");
        setting.endGroup();
    }
    all = setting.childGroups();
    for (int i = 0; i < all.size() ; ++i)
    {
        setting.beginGroup(all.at(i));
        allexec<<setting.value("Exec").toString();
        allkey << new QHotkey(QKeySequence(all.at(i)), true, this);
        qDebug() << "Is segistered:" << allkey[i]->isRegistered();
        QObject::connect(allkey.at(i), &QHotkey::activated,[=](){
            qDebug() << i;
            qDebug() << allexec.at(i);
            QProcess::startDetached(allexec.at(i));
        });
        setting.endGroup();
    }
}
void Application::cleanSetting()
{
    all.clear();
    allexec.clear();
    foreach(QHotkey *tmp,allkey)
    {
        if(tmp)
        {
            allkey.removeOne(tmp);
            tmp->disconnect();
            delete tmp;
            tmp = nullptr;
        }
    }
}
