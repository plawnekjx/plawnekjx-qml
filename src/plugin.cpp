#include <plawnekjx-core.h>

#include "plugin.h"

#include "application.h"
#include "device.h"
#include "plawnekjx.h"
#include "iconprovider.h"
#include "process.h"
#include "script.h"

#include <qqml.h>

static QObject *createPlawnekjxSingleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return Plawnekjx::instance();
}

void PlawnekjxQmlPlugin::registerTypes(const char *uri)
{
    qRegisterMetaType<QList<Application *>>("QList<Application *>");
    qRegisterMetaType<QList<Process *>>("QList<Process *>");
    qRegisterMetaType<QSet<unsigned int>>("QSet<unsigned int>");
    qRegisterMetaType<Device::Type>("Device::Type");
    qRegisterMetaType<SessionEntry::DetachReason>("SessionEntry::DetachReason");
    qRegisterMetaType<Script::Status>("Script::Status");
    qRegisterMetaType<Script::Runtime>("Script::Runtime");
    qRegisterMetaType<ScriptInstance::Status>("ScriptInstance::Status");

    qmlRegisterSingletonType<Plawnekjx>(uri, 1, 0, "Plawnekjx", createPlawnekjxSingleton);
}

void PlawnekjxQmlPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    // Ensure Plawnekjx is initialized.
    Plawnekjx::instance();

    engine->addImageProvider("plawnekjx", IconProvider::instance());
}
