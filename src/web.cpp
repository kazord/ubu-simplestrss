#include <QStandardPaths>
#include <QMutex>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextCodec>
#include <QVariant>
#include <QRegularExpression>
#include <QEventLoop>

#include "web.h"

#define CUSTOM_AGENT "SimplestRSS 1"

Web* Web::_instance = nullptr;
Web* Web::instance() {
	QMutex mutex;
	mutex.lock();
	if(nullptr == _instance)
		_instance = new Web();
	mutex.unlock();
	return _instance;
}
Web::Web() {
	_manager = new QNetworkAccessManager(this);
	//QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
	//diskCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
	//_manager->setCache(diskCache);

}

QString Web::get_favicon_url(QUrl url, bool redirected_url) {
	QString baseUrl = url.url(QUrl::RemoveQuery|QUrl::RemovePath);
	QString data;
	data = Web::wget(QUrl(baseUrl)); 
	QRegularExpression rx("<link [^>]*rel=\"(shortcut )?icon\"[^>]*/?>");
	rx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionMatch search = rx.match(data);
	if(search.hasMatch()) {
		QRegularExpression rx2("href=\"([^\"]*)\"");
		rx2.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
		qDebug()<<search.captured(0);
		QRegularExpressionMatch search2 = rx2.match(search.captured(0));
		if(search2.hasMatch()){
			if(search2.captured(1).left(4) != "http")
				return baseUrl+"/"+search2.captured(1);
			else
				return search2.captured(1);
		}
	}
	//TODO try on feedly
	return "";
}

QString Web::get_source_url_from_feedly(QUrl src_url, QString redirected_url) {
	QString data;
	data = Web::wget(src_url);
	QRegularExpression rx("<link>([^<]+)</link>");
	rx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionMatch search = rx.match(data);
	if(search.hasMatch()) {
		QRegularExpression rx2("^((https?://)?[^/]+/?)$");
		rx2.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
		QRegularExpressionMatch search2 = rx2.match(search.captured(1));
		if(!search.hasMatch()) //not a basic feed
			return search.captured(1);
		else
			return src_url.url();
	}
	else
		return  src_url.url();


}
void Web::cancel() {
	_reply->abort();
}

//void get(QUrl path, parameters, QXmlStreamReader &reader);
//void get_feedly(QUrl path, parameters, QJsonDocument &root);
//void get_mercury(QUrl path, parameters, QJsonDocument &root);
QString Web::wget(QUrl url) {
	QNetworkRequest request(url);
	//request.setRedirectPolicy(QNetworkRequest::UserVerifiedRedirectPolicy);
	//request.setAttribute(QNetworkRequest::UserAgentHeader, QVariant(QString(CUSTOM_AGENT)));
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
	//request.setAttribute(QNetworkRequest::FollowRedirectsAttribute,QVariant(true));
	qDebug() << "Starting "<<url<<" request ";
	_reply = _manager->get(request);
	QEventLoop loop;
	connect(_reply, SIGNAL(finished()),
		&loop,SLOT(quit()));
	loop.exec();
	qDebug() << " "<<url<<" download ";
	if(_reply->error() != QNetworkReply::NoError) //something happen, debug&leave
	{
		qDebug() << "Network Error for "<<url<<" : "<<_reply->error();
		_reply->deleteLater();
		_reply = nullptr;
		return "";
	}
	QByteArray output_array = _reply->readAll();
	QString output = QString::fromUtf8(output_array); 
	qDebug() << "convert ok";
	QRegularExpression encoding("<\\\?xml version=\"[^\"]+\" encoding=\"([^\"]+)\" ?\\\?>");
	encoding.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionMatch search = encoding.match(output);
	if (search.hasMatch()) {
		qDebug() << "Encoding found : " << search.captured(1).toUpper();
		if(search.captured(1).toUpper() != "UTF-8")
			output = QTextCodec::codecForHtml(search.captured(1).toUpper().toLatin1())->toUnicode(output_array);
	}
	output.replace("\"//","\"http://");
	_reply->deleteLater();
	_reply = nullptr;
	return output;
}

QString Web::wget(const QUrl url, QXmlStreamReader &reader) {
	QString text = wget(url);
	if(text != "")
		reader.addData(text);
	return text;
}
/*QString Web::autoFetchFullArticle(const QUrl url, const QString desc) {
	QString page = wget(url);
	QString stripedDesc = desc;
	stripedDesc.replace(QRegularExpression("<[^>]+>"), "");
	page.replace(QRegularExpression("<script[^>]+>.+?</script>"), "");
	QString stripedMidDesc = stripedDesc.mid(1,8);
	//stripedDesc.replace("?", ".");
	stripedDesc.replace(" ", "\\s+");

	//QRegularExpression finder("<div[^>]*>((?:[^<]|<[^d]|<d[^i]|<di[^v])+?"+stripedMidDesc+"(?:[^<]|<[^d]|<d[^i]|<di[^v])+?)</div>");
	QString notdiv("(?:[^<]|<[^d]|<d[^i]|<di[^v])");
	QString subdiv("(?:<div[^>]*>"+notdiv+"*</div>)");
	QString sub2div("(?:<div[^>]*>(?:"+notdiv+"|"+subdiv+")*</div>)");
	QString sub3div("(?:<div[^>]*>(?:"+notdiv+"|"+sub2div+")*</div>)");
	QRegularExpression finder("<div[^>]*>((?:"+sub3div+"|"+notdiv+")*"+stripedMidDesc+"(?:"+sub3div+"|"+notdiv+")*)</div>");
	qDebug() << stripedDesc <<finder.pattern();
	finder.setPatternOptions(QRegularExpression::CaseInsensitiveOption|QRegularExpression::DotMatchesEverythingOption|QRegularExpression::UseUnicodePropertiesOption);
		qDebug() << "matchstart : ";
	QRegularExpressionMatch search = finder.match(page);
		qDebug() << "matchend : ";
	if (search.hasMatch()) {
		qDebug() << "fulltext found : " << search.captured(1).size();
		return search.captured(1);
	}
	qDebug() << "search : " << stripedMidDesc;
	return "";
}*/
QString Web::autoFetchFullArticle(const QUrl url, const QString desc) {
	QString page = wget(url);
	QString stripedDesc = desc;
	stripedDesc.replace(QRegularExpression("<[^>]+>"), "");

	page.replace(QRegularExpression("<script[^>]+>.+?</script>",QRegularExpression::CaseInsensitiveOption|QRegularExpression::DotMatchesEverythingOption), "");
	QString stripedMidDesc = stripedDesc.mid(1,8);
	//stripedDesc.replace("?", ".");
	stripedDesc.replace(" ", "\\s+");

	QRegularExpression finder("<div[^>]*>((?:[^<]|<[^d]|<d[^i]|<di[^v])+?"+stripedMidDesc+"(?:[^<]|<[^d]|<d[^i]|<di[^v])+?)</div>");
	finder.setPatternOptions(QRegularExpression::CaseInsensitiveOption|QRegularExpression::DotMatchesEverythingOption|QRegularExpression::UseUnicodePropertiesOption);
	QString notdiv("(?:[^<]|<[^d]|<d[^i]|<di[^v])");
	QRegularExpression subdiv("(?:<div[^>]*>("+notdiv+"*)</div>)");
	subdiv.setPatternOptions(QRegularExpression::CaseInsensitiveOption|QRegularExpression::DotMatchesEverythingOption|QRegularExpression::UseUnicodePropertiesOption);
	do {

		QRegularExpressionMatch search = finder.match(page);
		if (search.hasMatch()) {
			qDebug() << "fulltext found : " << search.captured(1).size();
			return search.captured(1);
		}
		else
			page.replace(subdiv, "\\1");
		qDebug() << "remove div";

	}
	while(page.contains(subdiv));	
	return "";
}
//TODO handle subdiv like auto
QString Web::manualFetchFullArticle(const QUrl url, const QString divID) {
	QString page = wget(url);
	QRegularExpression finder("<div[^>]*(?:class|id)=.?"+divID+"[^>]+>(.+?)</div>");
	finder.setPatternOptions(QRegularExpression::CaseInsensitiveOption|QRegularExpression::DotMatchesEverythingOption);
	QRegularExpressionMatch search = finder.match(page);
	if (search.hasMatch()) {
		qDebug() << "fulltext found : " << search.captured(1);
		return search.captured(1);
	}
	return "";
}
