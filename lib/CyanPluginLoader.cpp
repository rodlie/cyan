#include "CyanPluginLoader.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>

CyanPlugins::CyanPlugins(QObject *parent) : QObject(parent)
{
}

CyanPlugins::~CyanPlugins()
{
    removePlugins();
}

void CyanPlugins::scanPluginsFolder(const QString &folder,
                                bool recursive)
{
    if (folder.isEmpty() ||
        folder == QString(".") ||
        folder == QString("..")) { return; }

    QDir pluginsDir(folder);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot)) {
        QFileInfo info(QString("%1/%2").arg(folder).arg(fileName));
        if (info.isDir()) {
            if (recursive) { scanPluginsFolder(info.absoluteFilePath()); }
            continue;
        }
        if (!QLibrary::isLibrary(pluginsDir.absoluteFilePath(fileName))) { continue; }
        QPluginLoader *loader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader->instance();
        if (!plugin) {
            qDebug() << "not a valid plugin object" << fileName;
            removePlugin(loader);
            continue;
        }
        CyanWidgetPlugin *cwp =qobject_cast<CyanWidgetPlugin*>(plugin);
        CyanEffectPlugin *cep =qobject_cast<CyanEffectPlugin*>(plugin);
        if (!cwp && !cep) {
            qDebug() << "not a valid plugin" << fileName;
            removePlugin(loader);
            continue;
        }
        loadPlugin(loader);
    }
}

void CyanPlugins::removePlugins()
{
    for (int i=0;i<plugins.size();++i) {
        removePlugin(plugins.at(i));
    }
}

void CyanPlugins::loadPlugin(QPluginLoader *loader)
{
    if (!loader) { return; }
    QObject *plugin = loader->instance();
    if (!plugin) {
        removePlugin(loader);
        return;
    }
    CyanWidgetPlugin *cwp =qobject_cast<CyanWidgetPlugin*>(plugin);
    CyanWidgetPlugin *cep =qobject_cast<CyanWidgetPlugin*>(plugin);
    if (!cwp && !cep) {
        qDebug() << "not a valid plugin";
        removePlugin(loader);
        return;
    }
    //qDebug() << "is cwp plugin" << cwp->uuid() << cwp->version();

    if (cwp) {
        CyanWidgetPlugin *ocwp = getWidgetPlugin(cwp->uuid());
        if (ocwp) {
            qDebug() << "already loaded plugin" << ocwp->uuid() << ocwp->version();
            if (cwp->version()<=ocwp->version()) { // existing is better or same
                qDebug() << "existing plugin" << ocwp->uuid() << ocwp->version() << "is better or same than new plugin" << cwp->uuid() << cwp->version();
                removePlugin(loader);
                return;
            } else { // existing plugin is older
                qDebug() << "new plugin" << cwp->uuid() << cwp->version() << " is better, remove old plugin"  << ocwp->uuid() << ocwp->version();
                removePlugin(cwp->uuid());
            }
        }
    } else if (cep) {
        CyanEffectPlugin *ocep = getEffectPlugin(cep->uuid());
        if (ocep) {
            qDebug() << "already loaded plugin" << ocep->uuid() << ocep->version();
            if (cep->version()<=ocep->version()) { // existing is better or same
                qDebug() << "existing plugin" << ocep->uuid() << ocep->version() << "is better or same than new plugin" << cwp->uuid() << cwp->version();
                removePlugin(loader);
                return;
            } else { // existing plugin is older
                qDebug() << "new plugin" << cep->uuid() << cep->version() << " is better, remove old plugin"  << ocep->uuid() << ocep->version();
                removePlugin(cep->uuid());
            }
        }
    }
    addPlugin(loader);
}

void CyanPlugins::unloadPlugin(QPluginLoader *loader)
{
    if (!loader) { return; }
    if (!loader->isLoaded()) { return; }
    loader->unload();
}

void CyanPlugins::addPlugin(QPluginLoader *loader)
{
    if (!loader) { return; }
    QObject *plugin = loader->instance();
    if (!plugin) {
        removePlugin(loader);
        return;
    }
    CyanWidgetPlugin *cwp =qobject_cast<CyanWidgetPlugin*>(plugin);
    CyanEffectPlugin *cep =qobject_cast<CyanEffectPlugin*>(plugin);
    if (!cwp && !cep) {
        removePlugin(loader);
        return;
    }
    plugins.append(loader);
}

void CyanPlugins::removePlugin(const QString &uuid)
{
    QPluginLoader *loader = getLoader(uuid);
    if (!loader) { return; }
    unloadPlugin(loader);

    if (plugins.contains(loader)) {
        int id = plugins.indexOf(loader);
        plugins.removeAt(id);
    }
    loader->deleteLater();
}

void CyanPlugins::removePlugin(QPluginLoader *loader)
{
    if (!loader) { return; }
    unloadPlugin(loader);

    if (plugins.contains(loader)) {
        plugins.removeAt(plugins.indexOf(loader));
    }
    loader->deleteLater();
}

QPluginLoader *CyanPlugins::getLoader(const QString &uuid)
{
    for (int i=0;i<plugins.size();++i) {
        QObject *plugin = plugins.at(i)->instance();
        if (!plugin) { continue; }
        CyanWidgetPlugin *cwp = qobject_cast<CyanWidgetPlugin*>(plugin);
        CyanEffectPlugin *cep = qobject_cast<CyanEffectPlugin*>(plugin);
        if (!cwp && !cep) { continue; }
        if (cwp && cwp->uuid()==uuid) { return plugins.at(i); }
        if (cep && cep->uuid()==uuid) { return plugins.at(i); }
    }
    return nullptr;
}

QPluginLoader *CyanPlugins::getLoader(int index)
{
    return plugins.at(index);
}

CyanWidgetPlugin *CyanPlugins::getWidgetPlugin(int index)
{
    if (plugins.at(index)) {
        QObject *plugin = plugins.at(index)->instance();
        if (!plugin) { return nullptr; }
        CyanWidgetPlugin *cwp = qobject_cast<CyanWidgetPlugin*>(plugin);
        return cwp;
    }
    return  nullptr;
}

CyanWidgetPlugin *CyanPlugins::getWidgetPlugin(QPluginLoader *loader)
{
    if (!loader) { return nullptr; }
    QObject *plugin = loader->instance();
    if (!plugin) { return nullptr; }
    CyanWidgetPlugin *cwp = qobject_cast<CyanWidgetPlugin*>(plugin);
    return cwp;
}

CyanEffectPlugin *CyanPlugins::getEffectPlugin(int index)
{
    if (plugins.at(index)) {
        QObject *plugin = plugins.at(index)->instance();
        if (!plugin) { return nullptr; }
        CyanEffectPlugin *p = qobject_cast<CyanEffectPlugin*>(plugin);
        return p;
    }
    return  nullptr;
}

CyanEffectPlugin *CyanPlugins::getEffectPlugin(QPluginLoader *loader)
{
    if (!loader) { return nullptr; }
    QObject *plugin = loader->instance();
    if (!plugin) { return nullptr; }
    CyanEffectPlugin *p = qobject_cast<CyanEffectPlugin*>(plugin);
    return p;
}

CyanWidgetPlugin *CyanPlugins::getWidgetPlugin(const QString &uuid)
{
    qDebug() << "get widget plugin with uuid" << uuid;
    for (int i=0;i<plugins.size();++i) {
        QObject *plugin = plugins.at(i)->instance();
        if (!plugin) { continue; }
        CyanWidgetPlugin *cwp = qobject_cast<CyanWidgetPlugin*>(plugin);
        if (!cwp) { continue; }
        qDebug() << "plugin" << cwp->uuid() << "vs" << uuid;
        if (cwp->uuid()==uuid) { return cwp; }
        //return cwp;
    }
    return nullptr;
}

CyanEffectPlugin *CyanPlugins::getEffectPlugin(const QString &uuid)
{
    qDebug() << "get effect plugin with uuid" << uuid;
    for (int i=0;i<plugins.size();++i) {
        QObject *plugin = plugins.at(i)->instance();
        if (!plugin) { continue; }
        CyanEffectPlugin *p = qobject_cast<CyanEffectPlugin*>(plugin);
        if (!p) { continue; }
        qDebug() << "plugin" << p->uuid() << "vs" << uuid;
        if (p->uuid()==uuid) { return p; }
    }
    return nullptr;
}

QList<QPluginLoader *> CyanPlugins::scan()
{
    if (plugins.size()>0) { removePlugins(); }

    QString addonsDir;

    addonsDir  = QString("%1/plugins").arg(qApp->applicationDirPath());
    if (QFile::exists(addonsDir)) { scanPluginsFolder(addonsDir, true /* recursive */); }
//#ifdef Q_OS_WIN
//    else { scanPluginsFolder(qApp->applicationDirPath()); }
//#endif
    addonsDir  = QString("%1/../lib/Cyan/plugins").arg(qApp->applicationDirPath());
    if (QFile::exists(addonsDir)) { scanPluginsFolder(addonsDir, true /* recursive */); }
    addonsDir  = QString("%1/../lib64/Cyan/plugins").arg(qApp->applicationDirPath());
    if (QFile::exists(addonsDir)) { scanPluginsFolder(addonsDir, true /* recursive */); }
#ifdef Q_OS_WIN
    addonsDir = QString("%1/../plugins").arg(qApp->applicationDirPath());
    if (QFile::exists(addonsDir)) { scanPluginsFolder(addonsDir, true /* recursive */); }
#endif

    return plugins;
}
