#ifndef PLAWNEKJXQML_PLAWNEKJX_H
#define PLAWNEKJXQML_PLAWNEKJX_H

#include "plawnekjxfwd.h"

#include <QMutex>
#include <QQmlEngine>
#include <QWaitCondition>

Q_MOC_INCLUDE("device.h")
class Device;
class MainContext;
class Scripts;

class Plawnekjx : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Plawnekjx)
    Q_PROPERTY(Device *localSystem READ localSystem CONSTANT)
    QML_ELEMENT
    QML_SINGLETON

public:
    enum class Scope {
        Minimal,
        Metadata,
        Full
    };
    Q_ENUM(Scope)

    explicit Plawnekjx(QObject *parent = nullptr);
private:
    void initialize();
    void dispose();
public:
    ~Plawnekjx();

    static Plawnekjx *instance();

    Device *localSystem() const { return m_localSystem; }

    QList<Device *> deviceItems() const { return m_deviceItems; }

Q_SIGNALS:
    void localSystemChanged(Device *newLocalSystem);
    void deviceAdded(Device *device);
    void deviceRemoved(Device *device);

private:
    static void onGetLocalDeviceReadyWrapper(GObject *obj, GAsyncResult *res, gpointer data);
    void onGetLocalDeviceReady(GAsyncResult *res);
    static void onEnumerateDevicesReadyWrapper(GObject *obj, GAsyncResult *res, gpointer data);
    void onEnumerateDevicesReady(GAsyncResult *res);
    static void onDeviceAddedWrapper(Plawnekjx *self, PlawnekjxDevice *deviceHandle);
    static void onDeviceRemovedWrapper(Plawnekjx *self, PlawnekjxDevice *deviceHandle);
    void onDeviceAdded(PlawnekjxDevice *deviceHandle);
    void onDeviceRemoved(PlawnekjxDevice *deviceHandle);

private Q_SLOTS:
    void add(Device *device);
    void removeById(QString id);

private:
    QMutex m_mutex;
    PlawnekjxDeviceManager *m_handle;
    QList<Device *> m_deviceItems;
    Device *m_localSystem;
    QWaitCondition m_localSystemAvailable;
    QScopedPointer<MainContext> m_mainContext;

    static Plawnekjx *s_instance;
};

#endif
