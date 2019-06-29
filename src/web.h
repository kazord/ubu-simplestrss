#ifndef WEB_H_
#define WEB_H_

#include <QXmlStreamReader>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class Web : QObject {

	public:
		static Web *instance();
		
		virtual ~Web() = default;

		virtual QString get_favicon_url(QUrl url, bool redirected_url = false);
		virtual QString get_source_url_from_feedly(QUrl src_url, QString redirected_url = "");
		virtual void cancel();
		
		//void get(QUrl path, parameters, QXmlStreamReader &reader);
		//void get_feedly(QUrl path, parameters, QJsonDocument &root);
		//void get_mercury(QUrl path, parameters, QJsonDocument &root);
		QString wget(QUrl uri);
		QString wget(QUrl uri, QXmlStreamReader &reader);
		QString autoFetchFullArticle(const QUrl url, const QString desc);
		QString manualFetchFullArticle(const QUrl url, const QString divID);
	//slots:
//		void replyReceived(QNetworkReply *reply);	
	private:
		Web();
		static Web* _instance;
		QNetworkAccessManager *_manager;
		QNetworkReply *_reply; 
				
};
#endif //web
