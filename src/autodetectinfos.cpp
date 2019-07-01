#include "autodetectinfos.h"

AutodetectInfos::AutodetectInfos() {
	_htmlDecode = false;
}
AutodetectNode AutodetectInfos::getItemNode() {
	return _item;
}
void AutodetectInfos::updateItem(AutodetectNode node) {
	_item = node;
}
void AutodetectInfos::updateTitle(AutodetectNode node) {
	_title = node;
}
void AutodetectInfos::updateCategory(AutodetectNode node) {
	_category = node;
}
void AutodetectInfos::updateDesc(AutodetectNode node) {
	_desc = node;
}
void AutodetectInfos::updateMultimedia(AutodetectNode node) {
	_multimedia = node;
}
void AutodetectInfos::updateLink(AutodetectNode node) {
	_link = node;
}
void AutodetectInfos::updateDate(AutodetectNode node) {
	_date = node;
}
void AutodetectInfos::updateAuthor(AutodetectNode node) {
	_author = node;
}
void AutodetectInfos::useHtmlDecode() {
	_htmlDecode = true;
}	
void AutodetectInfos::setDateFormatISO() {
	_dateFormat = Qt::ISODate;
}
void AutodetectInfos::setDateFormatRFC() {
	_dateFormat = Qt::RFC2822Date;
}
void AutodetectInfos::setDateFormatTXT() {
	_dateFormat = Qt::TextDate;
}
QStringList AutodetectInfos::node2list() {
	QStringList out;
	for(const AutodetectNode node : _knownNode)
	{
		out << node.second+">"+node.first;
	}
	return out;
}
void AutodetectInfos::updateKnownNode(AutodetectNode node) {
	if(!_knownNode.contains(node)) {
		_knownNode.append(node);
	}
}
#define ISPARENT(parent, child) (parent.first == child.second)
bool AutodetectInfos::isValid() {
	if(!_item.first.isEmpty() && (ISPARENT(_item, _title) || ISPARENT(_item, _desc) || ISPARENT(_item, _link) || ISPARENT(_item, _category) || ISPARENT(_item, _author)))
		return true;
	return false;
}

#define FILLINFOS(x) infos.x = x.first
void AutodetectInfos::fillFeedInfos(FeedInfos &infos) {
	infos._main = _item.second;
	FILLINFOS(_item);
	FILLINFOS(_link);
	FILLINFOS(_author);
	FILLINFOS(_category);
	FILLINFOS(_title);
	FILLINFOS(_desc);
	FILLINFOS(_date);
	FILLINFOS(_multimedia);
	infos._dateFormat = _dateFormat;
	infos._tagsDecoding = _htmlDecode;
}
