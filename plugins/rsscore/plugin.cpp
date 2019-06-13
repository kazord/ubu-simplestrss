#include <QtQml>
#include <QtQml/QQmlContext>

#include "plugin.h"
#include "rsscore.h"
#include "feed.h"

void RSSCorePlugin::registerTypes(const char *uri) {
    //@uri Pluginname
    qmlRegisterType<Feed>(uri, 1, 0, "Feed");
    qmlRegisterSingletonType<RSSCore>(uri, 1, 0, "RSSCore", [](QQmlEngine*, QJSEngine*) -> QObject* { return new RSSCore; });
}
