#ifndef UPOWERDEVICE_H
#define UPOWERDEVICE_H

#include <QObject>
#include <QDBusInterface>
#include <QVariantMap>
#include <QDBusPendingReply>  // 新增：确保包含QDBusPendingReply头文件
#include <QMetaType>          // 新增：确保包含QMetaType头文件

class UPowerDevice : public QObject
{
    Q_OBJECT

public:
    enum Type {
        Unknown = 0,
        GenericInterface = 1,
        Processor = 2,
        Block = 3,
        StorageAccess = 4,
        StorageDrive = 5,
        OpticalDrive = 6,
        StorageVolume = 7,
        OpticalDisc = 8,
        Camera = 9,
        PortableMediaPlayer = 10,
        Battery = 12,
        NetworkShare = 14,
        Last = 0xffff
    };
    Q_ENUM(Type)  // 确保Type枚举在元对象系统中可用

    explicit UPowerDevice(const QString &udi, QObject *parent = nullptr);

    bool queryDeviceInterface(Type type) const;
    QString description() const;
    QString batteryTechnology() const;
    QString product() const;
    QString vendor() const;

    QString udi() const;

    QVariant prop(const QString &key) const;
    bool propertyExists(const QString &key) const;
    QMap<QString, QVariant> allProperties() const;

signals:
    void changed();

private slots:
    void onPropertiesChanged(const QString &ifaceName, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void login1Resuming(bool active);
    void slotChanged();

private:
    void checkCache(const QString &key) const;

private:
    QDBusInterface m_device;
    QVariantMap m_cache;

    QString m_udi;
};

#endif // UPOWERDEVICE_H
