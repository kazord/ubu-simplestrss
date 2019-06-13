#include <QGuiApplication>
#include <QCoreApplication>
#include <QUrl>
#include <QString>
#include <QQuickView>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QList>

#include "src/db.h"
#include "src/web.h"
#include "src/xml.h"
#include "src/article.h"

int main(int argc, char *argv[])
{
    qDebug() << "Starting app from main.cpp";
    const char* home_keyword = "homescreen";
    if(argc == 2 && strcmp(argv[1], home_keyword) == 0) {
    	QCoreApplication *app = new QCoreApplication(argc, (char**)argv);
	QXmlStreamReader sr;
	
	FeedInfos fi;
	QList<Article> la;
	QQueue<FeedInfos> feedlist = DB::instance()->feeds();
	qDebug() << "feedlist fetch and contains " << feedlist.size();
	for(const FeedInfos fi : feedlist) {
		qDebug() << fi.name() << fi.getUrl();
		sr.clear();
		Web::instance()->wget(fi.getUrl(),sr);
		XML::extractArticles(fi, la, sr);
	}
	
	QTextStream cout(stdout, QIODevice::WriteOnly);
	QXmlStreamWriter stream(cout.device());
	stream.setAutoFormatting(true);
	stream.writeStartDocument();
	stream.writeStartElement("entries");
	for(const Article a : la) {
		a.writeXmlStream(stream);
	}
	stream.writeEndElement();//entries
	stream.writeEndDocument();
	
	return EXIT_SUCCESS;
    }
    QGuiApplication *app = new QGuiApplication(argc, (char**)argv);
    app->setApplicationName("simplestrss.kazord");

    qDebug() << "Starting app from main.cpp";

    QQuickView *view = new QQuickView();
    view->setSource(QUrl(QStringLiteral("qml/Main.qml")));
    view->show();

    return app->exec();
}
