#ifndef RSSCOREPLUGIN_H
#define RSSCOREPLUGIN_H

#include <QQmlExtensionPlugin>

class RSSCorePlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif
