#include <QDebug>
#include <QXmlStreamReader>

#include "../../src/feedinfos.h"
#include "rsscore.h"

RSSCore::RSSCore(QObject *parent) : QObject(parent) {

}

void RSSCore::speak() {
    qDebug() << "hello world!";
}
Feed* RSSCore::getDBFeed(int dbid) {
	FeedInfos fi = DB::instance()->feed(dbid);
	return new Feed(fi.name(), fi.getUrl(), fi.dbid());
}
Feed* RSSCore::getFeed(int feedindex) {
	FeedInfos fi = DB::instance()->feeds().at(feedindex);
	return new Feed(fi.name(), fi.getUrl(), fi.dbid());
}
QString RSSCore::fetchFeed(int dbid) {
	QXmlStreamReader sr;
	QString outstr("");	
	qDebug() << "fetching dbid " << dbid;
	FeedInfos fi = DB::instance()->feed(dbid);
	QList<Article> la;
	Web::instance()->wget(QUrl(fi.getUrl()),sr);
	XML::extractArticles(fi, la, sr);
	
	QXmlStreamWriter stream(&outstr);
	stream.setAutoFormatting(true);
	stream.writeStartDocument();
	stream.writeStartElement("entries");
	for(const Article a : la) {
		a.writeXmlStream(stream);
	}
	stream.writeEndElement();//entries
	stream.writeEndDocument();

	return outstr;
}

QString RSSCore::searchFeed(QString url) {
	AutodetectInfos ai;
	QXmlStreamReader sr;
	
	QString xmldata = Web::instance()->wget(QUrl(url),sr);
	qDebug() << XML::autodetect(sr,ai);
	sr.clear();
	sr.addData(xmldata);
	QStringList names = XML::foundChannel(sr,ai.getItemNode().second, ai.getItemNode().first);
	qDebug() << ai.isValid();

	FeedInfos fi;
	ai.fillFeedInfos(fi);
	qDebug() << url << names;
	fi.setUrl(url);
	if(names.size() > 0)
		fi.setName(names.first());
	else
		fi.setName("Unknow");
	qDebug() << fi.isDateFormatISO() << fi.isDateFormatRFC();
	return fi.name();
}
bool RSSCore::insertFeed(QString url) {
	AutodetectInfos ai;
	QXmlStreamReader sr;
	
	QString xmldata = Web::instance()->wget(QUrl(url),sr);
	XML::autodetect(sr,ai);
	sr.clear();
	sr.addData(xmldata);
	QStringList names = XML::foundChannel(sr,ai.getItemNode().second, ai.getItemNode().first);

	if(!ai.isValid())
		return false;

	FeedInfos fi;
	ai.fillFeedInfos(fi);
	fi.setUrl(url);
	if(names.size() > 0)
		fi.setName(names.first());
	else
		fi.setName("Unknow");
	fi.setFavicon(Web::instance()->get_favicon_url(QUrl(url)));
	
	if(!DB::instance()->addFeed(fi))
		return false;
	emit feedlistChanged();
	return true;

}
int RSSCore::getFeedCount() {
	qDebug() << DB::instance()->feeds().size();
	return DB::instance()->feeds().size();
}
void RSSCore::insert(Feed *feed) {
	qDebug() << "insert" << feed->url() << feed->name();
}
QQmlListProperty<Feed> RSSCore::feedlist() {
	return QQmlListProperty<Feed>(this, 0, &RSSCore::append_feed, &RSSCore::count_feed, &RSSCore::at_feed, &RSSCore::clear_feed);

}
void RSSCore::append_feed(QQmlListProperty<Feed> *list, Feed *feed)
{
	RSSCore *rsscore = qobject_cast<RSSCore*>(list->object);
	if(feed) {
		rsscore->insert(feed);
		emit rsscore->feedlistChanged();
	}
		
}
Feed* RSSCore::at_feed(QQmlListProperty<Feed> *list, int index) {
	RSSCore *rsscore = qobject_cast<RSSCore*>(list->object);
	return rsscore->getFeed(index);
}
int RSSCore::count_feed(QQmlListProperty<Feed> *list) {
	RSSCore *rsscore = qobject_cast<RSSCore*>(list->object);
	return rsscore->getFeedCount();
}
void RSSCore::clear_feed(QQmlListProperty<Feed> *list) {
	RSSCore	*rsscore = qobject_cast<RSSCore*>(list->object);
	//rsscore->_feeds.clear();
	emit rsscore->feedlistChanged();
}
