#include <plawnekjx-core.h>

#include "plawnekjx.h"

#include "device.h"
#include "devicelistmodel.h"
#include "maincontext.h"

Plawnekjx *Plawnekjx::s_instance = nullptr;

Plawnekjx::Plawnekjx(QObject *parent) :
    QObject(parent),
    m_localSystem(nullptr),
    m_mainContext(nullptr)
{
    plawnekjx_init();

    m_mainContext.reset(new MainContext(plawnekjx_get_main_context()));
    m_mainContext->schedule([this] () { initialize(); });

    QMutexLocker locker(&m_mutex);
    while (m_localSystem == nullptr)
        m_localSystemAvailable.wait(&m_mutex);
}

void Plawnekjx::initialize()
{
    m_handle = plawnekjx_device_manager_new();

    plawnekjx_device_manager_get_device_by_type(m_handle, PLAWNEKJX_DEVICE_TYPE_LOCAL, 0, nullptr,
        onGetLocalDeviceReadyWrapper, this);

    g_signal_connect_swapped(m_handle, "added", G_CALLBACK(onDeviceAddedWrapper), this);
    g_signal_connect_swapped(m_handle, "removed", G_CALLBACK(onDeviceRemovedWrapper), this);
    plawnekjx_device_manager_enumerate_devices(m_handle, nullptr, onEnumerateDevicesReadyWrapper, this);
}

void Plawnekjx::dispose()
{
    g_signal_handlers_disconnect_by_func(m_handle, GSIZE_TO_POINTER(onDeviceRemovedWrapper), this);
    g_signal_handlers_disconnect_by_func(m_handle, GSIZE_TO_POINTER(onDeviceAddedWrapper), this);
    g_object_unref(m_handle);
    m_handle = nullptr;
}

Plawnekjx::~Plawnekjx()
{
    m_localSystem = nullptr;
    qDeleteAll(m_deviceItems);
    m_deviceItems.clear();

    plawnekjx_device_manager_close_sync(m_handle, nullptr, nullptr);
    m_mainContext->perform([this] () { dispose(); });
    m_mainContext.reset();

    s_instance = nullptr;

    plawnekjx_deinit();
}

Plawnekjx *Plawnekjx::instance()
{
    if (s_instance == nullptr)
        s_instance = new Plawnekjx();
    return s_instance;
}

void Plawnekjx::onGetLocalDeviceReadyWrapper(GObject *obj, GAsyncResult *res, gpointer data)
{
    Q_UNUSED(obj);

    static_cast<Plawnekjx *>(data)->onGetLocalDeviceReady(res);
}

void Plawnekjx::onGetLocalDeviceReady(GAsyncResult *res)
{
    GError *error = nullptr;
    PlawnekjxDevice *deviceHandle = plawnekjx_device_manager_get_device_by_type_finish(m_handle, res, &error);
    g_assert(error == nullptr);

    auto device = new Device(deviceHandle);
    device->moveToThread(this->thread());

    {
        QMutexLocker locker(&m_mutex);
        m_localSystem = device;
        m_localSystemAvailable.wakeOne();
    }

    QMetaObject::invokeMethod(this, "add", Qt::QueuedConnection, Q_ARG(Device *, device));

    g_object_unref(deviceHandle);
}

void Plawnekjx::onEnumerateDevicesReadyWrapper(GObject *obj, GAsyncResult *res, gpointer data)
{
    Q_UNUSED(obj);

    static_cast<Plawnekjx *>(data)->onEnumerateDevicesReady(res);
}

void Plawnekjx::onEnumerateDevicesReady(GAsyncResult *res)
{
    GError *error = nullptr;
    PlawnekjxDeviceList *devices = plawnekjx_device_manager_enumerate_devices_finish(m_handle, res, &error);
    g_assert(error == nullptr);

    gint count = plawnekjx_device_list_size(devices);
    for (gint i = 0; i != count; i++) {
        PlawnekjxDevice *device = plawnekjx_device_list_get(devices, i);
        onDeviceAdded(device);
        g_object_unref(device);
    }

    g_object_unref(devices);
}

void Plawnekjx::onDeviceAddedWrapper(Plawnekjx *self, PlawnekjxDevice *deviceHandle)
{
    self->onDeviceAdded(deviceHandle);
}

void Plawnekjx::onDeviceRemovedWrapper(Plawnekjx *self, PlawnekjxDevice *deviceHandle)
{
    self->onDeviceRemoved(deviceHandle);
}

void Plawnekjx::onDeviceAdded(PlawnekjxDevice *deviceHandle)
{
    if (deviceHandle == m_localSystem->handle())
        return;

    auto device = new Device(deviceHandle);
    device->moveToThread(this->thread());

    QMetaObject::invokeMethod(this, "add", Qt::QueuedConnection, Q_ARG(Device *, device));
}

void Plawnekjx::onDeviceRemoved(PlawnekjxDevice *deviceHandle)
{
    QMetaObject::invokeMethod(this, "removeById", Qt::QueuedConnection, Q_ARG(QString, plawnekjx_device_get_id(deviceHandle)));
}

void Plawnekjx::add(Device *device)
{
    device->setParent(this);
    m_deviceItems.append(device);
    Q_EMIT deviceAdded(device);
}

void Plawnekjx::removeById(QString id)
{
    for (int i = 0; i != m_deviceItems.size(); i++) {
        auto device = m_deviceItems.at(i);
        if (device->id() == id) {
            m_deviceItems.removeAt(i);
            Q_EMIT deviceRemoved(device);
            delete device;
            break;
        }
    }
}
