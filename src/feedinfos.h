#ifndef FEEDINFOS_H_
#define FEEDINFOS_H_

#include <QString>
#include <QStringRef>
#include <QDate>

#include "db.h"
#include "autodetectinfos.h"

class BD;
class AutodetectInfos;

enum FeedInfosElements { NONE, MAIN, ITEM, LINK, AUTHOR, CATEGORY, TITLE, DESC, DATE, MULTIMEDIA };

class FeedInfos { 
	public:
		FeedInfos();
		FeedInfos(int id, QString name, QString main, QString item, QString link, QString author, QString category, QString title, QString desc, QString date, QString multimedia, QString color, QString faviconUrl, bool removeHTML, bool tagsDecoding, Qt::DateFormat dateFormat, QString url);
		FeedInfos(int id, QString name, QString url);
		~FeedInfos() = default;
		
		bool isItem(QStringRef cmp) const;
		bool isMain(QStringRef cmp) const;
		FeedInfosElements usedElement(QStringRef cmp) const;
		QString getColor() const;
		int getDateFormat() const;
		bool doTagsDecoding() const;
		bool doRemoveHTML() const;
		bool doNothing() const;
		bool isDateFormatISO() const;
		bool isDateFormatRFC() const;
		QString getFavicon() const;	
		QString getUrl() const;
		QString name() const;
		int dbid() const;
		void setUrl(QString url);
		void setName(QString name);
		void setFavicon(QString favicon);
		
		QString getProp(QString key) const;

	protected:
		int _id;
		QString _name;
		QString _main;
		QString _item;
		QString _link;
		QString _author;
		QString _category;
		QString _title;
		QString _desc;
		QString _date;
		QString _multimedia;
		QString _color;
		bool _removeHTML;
		bool _tagsDecoding;
		Qt::DateFormat _dateFormat;	
		QString _url;
		QString _faviconUrl;
	
//	friend bool DB::addFeed(const FeedInfos &infos);
//	friend bool DB::updateFeed(int id, const FeedInfos &newinfos);
//	friend QQueue<FeedInfos> DB::feeds();
	friend class DB;
	friend class AutodetectInfos;
};

#endif // FEEDINFOS_H_
