#include "db.h"

#include <QMutex>
#include <QDebug>

#include <QStringList>
#include <QSqlQuery>
#include <qsqlerror.h>
#include <QStandardPaths>
#include <QQueue>
#include <QDir>
#include <QLatin1String>

#define TABLE_NAME "feeds_v1"

DB* DB::_instance = nullptr;

DB::DB()
{
	qDebug() << "Starting database";
	_db = QSqlDatabase::addDatabase("QSQLITE");
	QDir dbpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	dbpath.cd("../simplestrss.kazord/");
	if(!dbpath.exists())
		dbpath.mkpath(dbpath.absolutePath());
	qDebug() << dbpath;
	_db.setDatabaseName(dbpath.absolutePath()+QString::fromStdString("/%1_sqlite.db").arg(DB_FILENAME));
	if(!_db.open())
		qDebug() << "Database starting fail" << _db.databaseName() << _db.lastError().text();
	else {
		QStringList tables = _db.tables();
		if (!tables.contains(TABLE_NAME, Qt::CaseInsensitive)) {
			QSqlQuery q;
			if (!q.exec(QLatin1String(
							"create table " TABLE_NAME "(id integer primary key, name varchar, url varchar, favicon varchar, main varchar, item varchar, link varchar, author varchar, category varchar, title varchar, desc varchar, date varchar, multimedia varchar, color varchar, date_format int, cleanHTML bool, tagsRemove bool)")))
				qDebug() << "Can't create feeds table : "<< q.lastError().text();
			else {
				qDebug() << "Feeds table created";
			}
		}
	}
}

DB::~DB() {
	_db.close();
}

DB* DB::instance() {
	QMutex mutex;
	mutex.lock();
	if(nullptr == _instance)
		_instance = new DB();
	mutex.unlock();
	return _instance;
}

bool DB::removeFeed(int id) {
	QSqlQuery q;
	if (!q.prepare(QLatin1String(
					"delete from " TABLE_NAME " where id = ?"
				    ))) {
		qDebug() << "remove feed " << id << " failed : "<< q.lastError().text();
		return false;
	}
	q.addBindValue(id);
	return q.exec();
}

int DB::idFromUrl(const QString& url) {
	//prequery
	QSqlQuery pq;
	if (!pq.prepare(QLatin1String(
					"select id from " TABLE_NAME " where url = ?"
				     ))) {
		qDebug() << "url check fail "<< pq.lastError().text();
		return -1;
	}
	pq.addBindValue(url);
	pq.exec();
	if(pq.next()) {
		return pq.value(0).toInt();
	}
	else
		return -1;
}

bool DB::addFeed(const FeedInfos &info) {
	if(idFromUrl(info._url) != -1) {
		qDebug() << "url already in DB "<< info._url;
		return false;
	}
	QSqlQuery q;
	if (!q.prepare(QLatin1String(
					"insert into " TABLE_NAME "(name, url, main, item, link, author, category, title, desc, date, multimedia, color, date_format, cleanHTML, tagsRemove, favicon) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
				    ))) {
		qDebug() << "New feed saving failed : "<< q.lastError().text();
		return false;
	}
	q.addBindValue(info._name);
	q.addBindValue(info._url);
	q.addBindValue(info._main);
	q.addBindValue(info._item);
	q.addBindValue(info._link);
	q.addBindValue(info._author);
	q.addBindValue(info._category);
	q.addBindValue(info._title);
	q.addBindValue(info._desc);
	q.addBindValue(info._date);
	q.addBindValue(info._multimedia);
	q.addBindValue(info._color);
	q.addBindValue(info._dateFormat);
	q.addBindValue((info._removeHTML));
	q.addBindValue((info._tagsDecoding));
	q.addBindValue(info._faviconUrl);

	return q.exec();
}

bool DB::updateFeed(int id, const FeedInfos &newinfos) {
	QSqlQuery q;
	if (!q.prepare(QLatin1String(
					"update " TABLE_NAME " SET name = ?, url = ?, main = ?, item = ?, link = ?, author = ?, category = ?, title = ?, desc = ?, date = ?, multimedia = ?, color = ?, date_format = ?, cleanHTML = ?, tagsRemove = ?, favicon = ? WHERE id = ?"
				    ))) {
		qDebug() << "Update feed failed : "<< q.lastError().text();
		return false;
	}
	q.addBindValue(newinfos._name);
	q.addBindValue(newinfos._url);
	q.addBindValue(newinfos._main);
	q.addBindValue(newinfos._item);
	q.addBindValue(newinfos._link);
	q.addBindValue(newinfos._author);
	q.addBindValue(newinfos._category);
	q.addBindValue(newinfos._title);
	q.addBindValue(newinfos._desc);
	q.addBindValue(newinfos._date);
	q.addBindValue(newinfos._multimedia);
	q.addBindValue(newinfos._color);
	q.addBindValue(newinfos._dateFormat);
	q.addBindValue(newinfos._removeHTML);
	q.addBindValue(newinfos._tagsDecoding);
	q.addBindValue(newinfos._faviconUrl);

	q.addBindValue(id);

	if (q.exec())
		return true;
	else {
		qDebug() << "Update feed failed : "<< q.lastError().text();
		return false;
	}
}
bool DB::updateFeed(int id, QString& key, QString& value) {
	QSqlQuery q;
	QString req = QString("update ").append(TABLE_NAME).append(" SET ").append(key).append(" = ? WHERE id = ?");
	if (!q.prepare(req.toLatin1())) {
		qDebug() << "Update feed failed : "<< q.lastError().text();
		return false;
	}
	q.addBindValue(value);
	q.addBindValue(id);

	if (q.exec())
		return true;
	else {
		qDebug() << "Update feed failed : "<< q.lastError().text();
		return false;
	}
}
bool DB::updateFeed(int id, QString& key, int value) {
	QSqlQuery q;
	QString req = QString("update ").append(TABLE_NAME).append(" SET ").append(key).append(" = ? WHERE id = ?");
	if (!q.prepare(req.toLatin1())) {
		qDebug() << "Update feed failed : "<< q.lastError().text();
		return false;
	}
	q.addBindValue(value);
	q.addBindValue(id);

	if (q.exec())
		return true;
	else {
		qDebug() << "Update feed failed : "<< q.lastError().text();
		return false;
	}
}

QQueue<FeedInfos>  DB::feeds() {

	QQueue<FeedInfos> ret;
	QSqlQuery q;
	if(!q.exec("SELECT id, url, main, item, link, author, category, title, desc, date, multimedia, color, date_format, cleanHTML, tagsRemove, name, favicon FROM " TABLE_NAME))
	{
		qDebug() << "Feeds fetch failed : "<< q.lastError().text();
		return ret;
	}
	while (q.next()) {
		//qDebug() << q.value(0) << q.value(1) << q.value(2) << q.value(3);
		FeedInfos f;
		f._id = q.value(0).toInt();
		f._url = q.value(1).toString();
		f._main = q.value(2).toString();
		f._item = q.value(3).toString();
		f._link = q.value(4).toString();
		f._author = q.value(5).toString();
		f._category = q.value(6).toString();
		f._title = q.value(7).toString();
		f._desc = q.value(8).toString();
		f._date = q.value(9).toString();
		f._multimedia = q.value(10).toString();
		f._color = q.value(11).toString();
		f._dateFormat = (Qt::DateFormat) q.value(12).toInt();
		f._removeHTML = q.value(13).toBool();
		f._tagsDecoding = q.value(14).toBool();
		f._name = q.value(15).toString();
		f._faviconUrl = q.value(16).toString();
		ret.append(f);
	}
	return ret;
}
FeedInfos DB::feed(const int id) {
	FeedInfos f;
	QSqlQuery q;
	if(!q.prepare("SELECT id, url, main, item, link, author, category, title, desc, date, multimedia, color, date_format, cleanHTML, tagsRemove, name, favicon FROM " TABLE_NAME " WHERE id = ?"))
	{
		qDebug() << "Feeds fetch failed : "<< q.lastError().text();
		return f;
	}
	q.addBindValue(id);
	if(!q.exec()) {
		qDebug() << "Feeds fetch failed : "<< q.lastError().text();
		return f;
	}
	if(q.next()) {
		f._id = q.value(0).toInt();
		f._url = q.value(1).toString();
		f._main = q.value(2).toString();
		f._item = q.value(3).toString();
		f._link = q.value(4).toString();
		f._author = q.value(5).toString();
		f._category = q.value(6).toString();
		f._title = q.value(7).toString();
		f._desc = q.value(8).toString();
		f._date = q.value(9).toString();
		f._multimedia = q.value(10).toString();
		f._color = q.value(11).toString();
		f._dateFormat = (Qt::DateFormat) q.value(12).toInt();
		f._removeHTML = q.value(13).toBool();
		f._tagsDecoding = q.value(14).toBool();
		f._name = q.value(15).toString();
		f._faviconUrl = q.value(16).toString();
	}
	return f;
}
