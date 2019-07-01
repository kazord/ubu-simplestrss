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
	return new Feed(fi);
}
Feed* RSSCore::getFeed(int feedindex) {
	FeedInfos fi = DB::instance()->feeds().at(feedindex);
	return new Feed(fi);
}
void RSSCore::removeDBFeed(int dbid) {
	DB::instance()->removeFeed(dbid);
    emit feedlistChanged();
}
//key : name = ?, url = ?, main = ?, item = ?, link = ?, author = ?, category = ?, title = ?, desc = ?, date = ?, multimedia = ?, color = ?, date_format = ?, cleanHTML = ?, tagsRemove = ?, favicon = ?
void RSSCore::updateDBFeed(int dbid, QString &key, QString value) {
	DB::instance()->updateFeed(dbid, key, value);
}
void RSSCore::updateDBFeed(int dbid, QString &key, int value) {
	DB::instance()->updateFeed(dbid, key, value);
}
QString RSSCore::fetchFeed(int dbid) {
	qDebug() << "fetching dbid " << dbid;
	QXmlStreamReader sr;
	FeedInfos fi;
	QString outstr("");
	QList<Article> la;
	if(dbid == 0) {
		QQueue<FeedInfos> feedlist = DB::instance()->feeds();
		for(const FeedInfos fi : feedlist) {
			sr.clear();
			Web::instance()->wget(fi.getUrl(),sr);
			XML::extractArticles(fi, la, sr);
		}
		//sort la
		for(int i = 0; i < la.size();i++) {
			qint64 mindate = la.at(i).getNumDate();
			for(int j = i+1; j < la.size();j++) {
				if(la.at(j).getNumDate() > mindate) {
					mindate = la.at(j).getNumDate();
					la.move(j, i);
				}
			}
		}
	}
	else
	{
		fi = DB::instance()->feed(dbid);
		Web::instance()->wget(QUrl(fi.getUrl()),sr);
		XML::extractArticles(fi, la, sr);
	}
	
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
QString RSSCore::previewFeed(QString url) {
	AutodetectInfos ai;
	QXmlStreamReader sr;
	
	QString xmldata = Web::instance()->wget(QUrl(url),sr);
	qDebug() << XML::autodetect(sr,ai);
	sr.clear();
	sr.addData(xmldata);
	QStringList names = XML::foundChannel(sr,ai.getItemNode().second, ai.getItemNode().first);
	FeedInfos fi;
	ai.fillFeedInfos(fi);
	fi.setUrl(url);
	if(names.size() > 0)
		fi.setName(names.first());
	else
		fi.setName("Unknow");
	fi.setFavicon(Web::instance()->get_favicon_url(QUrl(url)));
	qDebug() << fi.getFavicon() << Web::instance()->get_favicon_url(QUrl(url));
	qDebug() << "fetching preview ";
	QString outstr("");
	QList<Article> la;
	sr.clear();
	sr.addData(xmldata);
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
	qDebug() <<Web::instance()->get_favicon_url(QUrl(url));
	if(!DB::instance()->addFeed(fi))
		return false;
	emit feedlistChanged();
	return true;

}
QStringList RSSCore::getAutodetectValues(int dbid) {
		FeedInfos fi = DB::instance()->feed(dbid);
		return getAutodetectValues(fi.getUrl());
}

QStringList RSSCore::getAutodetectValues(QString url) {

	AutodetectInfos ai;
	QXmlStreamReader sr;
	
	QString xmldata = Web::instance()->wget(QUrl(url),sr);
	XML::autodetect(sr,ai);
	return ai.node2list();

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
QQmlListProperty<Feed> RSSCore::feedlistZero() {
	return QQmlListProperty<Feed>(this, 0, &RSSCore::append_feed, &RSSCore::count_feedZero, &RSSCore::at_feedZero, &RSSCore::clear_feed);
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
Feed* RSSCore::at_feedZero(QQmlListProperty<Feed> *list, int index) {
	RSSCore *rsscore = qobject_cast<RSSCore*>(list->object);
	if(index == 0)
		return new Feed("All feed", "", 0);
	else
		return rsscore->getFeed(index-1);
}
int RSSCore::count_feed(QQmlListProperty<Feed> *list) {
	RSSCore *rsscore = qobject_cast<RSSCore*>(list->object);
	return rsscore->getFeedCount();
}
int RSSCore::count_feedZero(QQmlListProperty<Feed> *list) {
	RSSCore *rsscore = qobject_cast<RSSCore*>(list->object);
	return rsscore->getFeedCount()+1;
}
void RSSCore::clear_feed(QQmlListProperty<Feed> *list) {
	RSSCore	*rsscore = qobject_cast<RSSCore*>(list->object);
	//rsscore->_feeds.clear();
	emit rsscore->feedlistChanged();
}
QString RSSCore::fetchFullArticle(const QString url, const QString mode, const QString data) {
	if(mode == "auto") {
		return Web::instance()->autoFetchFullArticle(QUrl(url), data);
	}
	else if (mode == "manual") {
		return Web::instance()->manualFetchFullArticle(QUrl(url), data);
	}
	return "";
}
