#include "feed.h"

QString Feed::name() const {
	return _name;
}
QString Feed::url() const {
	return _url;
}
int Feed::dbid() const {
	return _dbid;
}
Feed::Feed(QObject *parent) : QObject(parent) 
{
}
Feed::Feed(QString name, QString url, int dbid, QObject *parent) : QObject(parent), _name(name), _url(url), _dbid(dbid)
{
}
