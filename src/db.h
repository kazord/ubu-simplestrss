#ifndef DB_H
#define DB_H

#include <QSql>
#include <QSqlDatabase>
#include <QQueue>

#include "feedinfos.h"

class FeedInfos;

#define DB_FILENAME "simplestrss"
class DB
{
	public:
		static DB *instance();
		~DB();
		QQueue<FeedInfos> feeds();
		FeedInfos feed(const int id);
		bool addFeed(const FeedInfos &info);
		bool updateFeed(int id, const FeedInfos &info);
		bool updateFeed(int id, QString& key, QString& value);
		bool updateFeed(int id, QString& key, int value);
		bool removeFeed(int id);
		int idFromUrl(const QString& url);

	private:
		DB();
		static DB* _instance;
		QSqlDatabase _db;
};
#endif // DB_H
