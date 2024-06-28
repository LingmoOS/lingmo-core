#ifndef UPOWERDEVICE_H
#define UPOWERDEVICE_H

#include <QObject>
#include <QDBusInterface>
#include <QVariantMap>          // 新增：Qt6需要显式包含QVariantMap头文件
#include <QDBusPendingReply>    // 新增：Qt6需要显式包含QDBusPendingReply头文件
#include <QMetaType>            // 新增：确保QMetaType头文件被包含

class UPowerDevice : public QObject
{
    Q_OBJECT

public:
    enum Type { Unknown = 0, GenericInterface = 1, Processor = 2,
                Block = 3, StorageAccess = 4, StorageDrive = 5,
                OpticalDrive = 6, StorageVolume = 7, OpticalDisc = 8,
                Camera = 9, PortableMediaPlayer = 10,
                Battery = 12, NetworkShare = 14, Last = 0xffff
              };
    Q_ENUM(Type)  // 新增：使Type枚举类型在Qt的元对象系统中可用

    explicit UPowerDevice(const QString &udi, QObject *parent = nullptr);  // 修改：使用nullptr初始化指针

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
    mutable QDBusInterface m_device;
    mutable QVariantMap m_cache;

    QString m_udi;
};

#endif // UPOWERDEVICE_H
