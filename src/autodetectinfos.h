#ifndef AUTODETECTINFOS_H_
#define AUTODETECTINFOS_H_

#include <QString>
#include <QPair>
#include <QDateTime>

#include "feedinfos.h"

class FeedInfos;
/*class AutodetectNode {
	public:
		AutodetectNode(QString name, QString parentName) : _name(name), _parent(parentName) {}
		~AutodetectNode() = default;
		QString getName() const {return _name;}
		QString getParentName() const {return _parent;}
	private
		QString _name;
		QString _parent;
}*/
//QPair : <name, parent>
typedef QPair<QString,QString> AutodetectNode;

class AutodetectInfos {
	public:
		AutodetectInfos();
		~AutodetectInfos() = default;
		
		void updateItem(AutodetectNode node);
		void updateTitle(AutodetectNode node);
		void updateCategory(AutodetectNode node);
		void updateDesc(AutodetectNode node);
		void updateMultimedia(AutodetectNode node);
		void updateLink(AutodetectNode node);
		void updateDate(AutodetectNode node);
		void updateAuthor(AutodetectNode node);
		void useHtmlDecode();
		void setDateFormatISO();
		void setDateFormatRFC();
		void setDateFormatTXT();

		void updateKnownNode(AutodetectNode node);
		
		bool isValid();	
		
		void fillFeedInfos(FeedInfos &infos);
		AutodetectNode getItemNode();
		QStringList node2list();
	private:
		QList<AutodetectNode> _knownNode;
		AutodetectNode _item;
		AutodetectNode _link;
		AutodetectNode _author;
		AutodetectNode _category;
		AutodetectNode _title;
		AutodetectNode _desc;
		AutodetectNode _date;
		AutodetectNode _multimedia;
		bool _htmlDecode;
		Qt::DateFormat _dateFormat;	
};

#endif //AUTODETECTINFOS_H_
