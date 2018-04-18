#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QMap>
#include <QtDBus>

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(const QString block, QObject *parent = NULL);
    QString name;
    QString path;
    QString drive;
    QString mountpoint;
    QString filesystem;
    bool isOptical;
    bool isRemovable;
    bool hasMedia;
    int opticalDataTracks;
    int opticalAudioTracks;
    bool isBlankDisc;
    bool hasPartition;
private:
    QDBusInterface *dbus;
signals:
    void mediaChanged(QString devicePath, bool mediaPresent);
    void mountpointChanged(QString devicePath, QString deviceMountpoint);
    void nameChanged(QString devicePath, QString deviceName);
    void errorMessage(QString error);
public slots:
    void mount();
    void unmount();
    void eject();
private slots:
    void updateDeviceProperties();
    void handlePropertiesChanged(const QString &interfaceType, const QMap<QString, QVariant> &changedProperties);
};

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = NULL);
    QMap<QString,Device*> devices;
signals:
    void showHideTray(bool show);
private slots:
    void setupDBus();
    void scanDevices();
    void deviceAdded(const QDBusObjectPath &obj);
    void deviceRemoved(const QDBusObjectPath &obj);
    void handleDeviceMediaChanged(QString devicePath, bool mediaPresent);
    void handleDeviceMountpointChanged(QString devicePath, QString deviceMountpoint);
    void handleDeviceNameChanged(QString devicePath, QString deviceName);
};

#endif // MANAGER_H
