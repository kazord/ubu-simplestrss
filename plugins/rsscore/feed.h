#ifndef FEED_H
#define FEED_H

#include <QObject>
#include <QString>

class Feed: public QObject {
	Q_OBJECT
	Q_PROPERTY( QString name READ name CONSTANT)
	Q_PROPERTY( QString url READ url CONSTANT)
	Q_PROPERTY( int dbid READ dbid CONSTANT)

	public:
		Feed(QObject *parent = nullptr);
		Feed(QString name, QString url, int dbid = -1, QObject *parent = nullptr);
		QString name() const;
		QString url() const;
		int dbid() const;

	protected:
		QString _name;
		QString _url;
		int _dbid;
};
#endif
