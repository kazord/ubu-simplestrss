#include <QStandardPaths>
#include <QMutex>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextCodec>
#include <QVariant>
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
	QString baseUrl = url.url(QUrl::RemovePath);
	QString data;
	data = Web::wget(QUrl(baseUrl)); 
	QRegExp rx("<link [^>]*rel=\"shortcut icon\"[^>]*/?>");
	rx.setCaseSensitivity(Qt::CaseInsensitive);
	int pos = rx.indexIn(data);
	if(pos > -1) {
		QRegExp rx2("href=\"([^\"]*)\"");
		rx2.setCaseSensitivity(Qt::CaseInsensitive);
		int pos2 = rx2.indexIn(rx.cap(0));
		if(pos2 > -1){
			if(rx2.cap(1).left(4) != "http")
				return baseUrl+"/"+rx2.cap(1);
			else
				return rx2.cap(1);
		}
	}
	//TODO try on feedly
	return "";
}

QString Web::get_source_url_from_feedly(QUrl src_url, QString redirected_url) {
	QString data;
	data = Web::wget(src_url);
	QRegExp rx("<link>([^<]+)</link>");
	rx.setCaseSensitivity(Qt::CaseInsensitive);
	int pos = rx.indexIn(data);
	if (pos > -1) {
		QRegExp rx2("^((https?://)?[^/]+/?)$");
		rx2.setCaseSensitivity(Qt::CaseInsensitive);
		int pos2 = rx2.indexIn(rx.cap(1));
		if(pos2 == -1) //not a basic feed
			return rx.cap(1);
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
	QRegExp encoding("<\\\?xml version=\"[^\"]+\" encoding=\"([^\"]+)\" ?\\\?>");
	encoding.setCaseSensitivity(Qt::CaseInsensitive);
	int pos = encoding.indexIn(output);
	if (pos != -1) {
		qDebug() << "Encoding found : " << encoding.cap(1).toUpper();
		if(encoding.cap(1).toUpper() != "UTF-8")
			output = QTextCodec::codecForHtml(encoding.cap(1).toUpper().toLatin1())->toUnicode(output_array);
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
