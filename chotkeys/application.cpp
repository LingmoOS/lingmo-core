#include "application.h"
#include "hotkeys.h"
#include <QFileSystemWatcher>

Application::Application(QObject *parent)
    : QObject{parent}
    , m_hotKeys(new Hotkeys)
{
    initSetting();
    setupShortcuts();

    connect(m_hotKeys, &Hotkeys::pressed, this, &Application::onPressed);
    connect(m_hotKeys, &Hotkeys::released, this, &Application::onReleased);
    QFileSystemWatcher *m_FileWatcher = new QFileSystemWatcher(this);
    m_FileWatcher->addPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/lingmoglobalshortcutsrc");
    connect(m_FileWatcher, &QFileSystemWatcher::fileChanged, this, &Application::initSetting);
}

void Application::setupShortcuts()
{
    m_hotKeys->registerKey(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_Delete));
    m_hotKeys->registerKey(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_A));
    m_hotKeys->registerKey(QKeySequence(Qt::META + Qt::Key_L));
    // m_hotKeys->registerKey(QKeySequence(Qt::Key_Super_L));
}
void Application::initSetting()
{
    cleanSetting();
    QFile file(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/lingmoglobalshortcutsrc");
    QSettings setting(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/lingmoglobalshortcutsrc", QSettings::IniFormat);
    if(!file.exists())
    {
        setting.beginGroup("Ctrl+Alt+A");
        setting.setValue("Comment","Screenshot");
        setting.setValue("Exec","lingmo-screenshot");
        setting.endGroup();

        setting.beginGroup("Meta+L");
        setting.setValue("Comment","Screenlocker");
        setting.setValue("Exec","lingmo-screenlocker");
        setting.endGroup();

        setting.beginGroup("Ctrl+Alt+T");
        setting.setValue("Comment","Terminal");
        setting.setValue("Exec","lingmo-terminal");
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
void Application::onReleased(QKeySequence keySeq)
{
    if (keySeq == QKeySequence(Qt::Key_Super_L)) {
        QProcess::startDetached("lingmo-launcher", QStringList());
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
