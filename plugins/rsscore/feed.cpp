#include "feed.h"

QString Feed::name() const {
	return _infos.name();
}
QString Feed::url() const {
	return _infos.getUrl();
}
int Feed::dbid() const {
	return _infos.dbid();
}
QString Feed::faviconUrl() const {
	return _infos.getFavicon();
}
QString Feed::color() const {
	return _infos.getTitleColor();
}
Feed::Feed(QObject *parent) : QObject(parent) 
{
}
Feed::Feed(QString name, QString url, int dbid, QObject *parent) : QObject(parent), _infos(dbid,name,url)
{
}
Feed::Feed(FeedInfos &infos, QObject *parent) : QObject(parent), _infos(infos)
{
}
