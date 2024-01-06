#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include "QHotkey/qhotkey.h"
#include "hotkeys.h"
#include <QSettings>
#include <QStandardPaths>
#include <QProcess>
#include <QList>
#include <QFile>

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = nullptr);

private slots:
    void initSetting();
    void onPressed(QKeySequence keySeq);
    void onReleased(QKeySequence keySeq);

private:
    void cleanSetting();
    Hotkeys *m_hotKeys;
    QStringList all;
    QStringList allexec;
    QList<QHotkey*> allkey;
    QSettings setting;
};

#endif // APPLICATION_H
