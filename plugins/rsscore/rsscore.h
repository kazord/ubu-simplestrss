#ifndef RSSCORE_H
#define RSSCORE_H

#include <QObject>
#include <QString>
#include <QQmlListProperty>

#include "../../src/db.h"
#include "../../src/web.h"
#include "../../src/xml.h"
#include "../../src/article.h"

#include "feed.h"

class RSSCore: public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Feed> feedlist READ feedlist NOTIFY feedlistChanged)
    Q_PROPERTY(QQmlListProperty<Feed> feedlistZero READ feedlistZero NOTIFY feedlistChanged)

public:
    RSSCore(QObject *parent = nullptr);
    ~RSSCore() = default;

    Q_INVOKABLE void speak();
    Feed* getDBFeed(int dbid);
    Feed* getFeed(int feedindex);
    Q_INVOKABLE void removeDBFeed(int dbid);
    Q_INVOKABLE void updateDBFeed(int dbid,QString &key, QString value);
    Q_INVOKABLE void updateDBFeed(int dbid,QString &key, int value);
    Q_INVOKABLE QString fetchFeed(int dbid);
    Q_INVOKABLE QString searchFeed(QString url);
    Q_INVOKABLE QString previewFeed(QString url);
    Q_INVOKABLE bool insertFeed(QString url);
    Q_INVOKABLE int getFeedCount();
    Q_INVOKABLE QString fetchFullArticle(const QString url, const QString mode, const QString data);
    QQmlListProperty<Feed> feedlist();
    QQmlListProperty<Feed> feedlistZero();
    void insert(Feed *feed);

signals:
    void feedlistChanged();
private:
	static void append_feed(QQmlListProperty<Feed> *list, Feed *feed);
	static Feed* at_feed(QQmlListProperty<Feed> *list, int index);
	static int count_feed(QQmlListProperty<Feed> *list); 
	static void clear_feed(QQmlListProperty<Feed> *list);
	static Feed* at_feedZero(QQmlListProperty<Feed> *list, int index);
	static int count_feedZero(QQmlListProperty<Feed> *list); 
};

#endif
