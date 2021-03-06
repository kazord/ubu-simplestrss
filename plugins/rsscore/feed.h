#ifndef FEED_H
#define FEED_H

#include <QObject>
#include <QString>
#include "../../src/feedinfos.h"

class Feed: public QObject {
	Q_OBJECT
	Q_PROPERTY( QString name READ name CONSTANT)
	Q_PROPERTY( QString url READ url CONSTANT)
	Q_PROPERTY( int dbid READ dbid CONSTANT)
	Q_PROPERTY( QString faviconUrl READ faviconUrl CONSTANT)
	Q_PROPERTY( QString color READ color CONSTANT)
	Q_PROPERTY( bool removeHTML READ doRemoveHTML CONSTANT)
	Q_PROPERTY( bool tagsDecoding READ doTagsDecoding CONSTANT)
	Q_PROPERTY( int dateFormat READ getDateFormat CONSTANT)


	public:
		Feed(QObject *parent = nullptr);
		Feed(QString name, QString url, int dbid = -1, QObject *parent = nullptr);
		Feed(FeedInfos &infos, QObject *parent = nullptr);
		QString name() const;
		QString url() const;
		int dbid() const;
		QString faviconUrl() const;
		QString color() const;
		bool doTagsDecoding() const;
		bool doRemoveHTML() const;
		int getDateFormat() const;
		Q_INVOKABLE QString getProp(QString key) const;

	protected:
		FeedInfos _infos;
};
#endif
