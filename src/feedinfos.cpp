#include "feedinfos.h"
#include <QDebug>

FeedInfos::FeedInfos(int id, QString name, QString main, QString item, QString link, QString author, QString category, QString title, QString desc, QString date, QString multimedia, QString titleColor, QString faviconUrl, bool removeHTML, bool tagsDecoding, Qt::DateFormat dateFormat,QString url) : _id(id),_name(name), _main(main), _item(item),  _link(link), _author(author), _category(category), _title(title), _desc(desc), _date(date), _multimedia(multimedia), _titleColor(titleColor),_faviconUrl(faviconUrl), _removeHTML(removeHTML), _tagsDecoding(tagsDecoding), _dateFormat(dateFormat), _url(url) {

}
FeedInfos::FeedInfos(int id, QString name, QString url) : _id(id),_name(name), _url(url) {
}
FeedInfos::FeedInfos() {
}
void FeedInfos::setName(QString name) {
	_name = name;
}
void FeedInfos::setUrl(QString url) {
	_url = url;
}
void FeedInfos::setFavicon(QString faviconurl) {
	_faviconUrl = faviconurl;
}
bool FeedInfos::isItem(QStringRef cmp) const
{
	return cmp == _item;
}
bool FeedInfos::isMain(QStringRef cmp) const
{
	return cmp == _main;
}
FeedInfosElements FeedInfos::usedElement(QStringRef cmp) const
{
	if(cmp == _main)
		return FeedInfosElements::MAIN;
	if(cmp == _item)
		return FeedInfosElements::ITEM;
	if(cmp == _link)
		return FeedInfosElements::LINK;
	if(cmp == _author)
		return FeedInfosElements::AUTHOR;
	if(cmp == _category)
		return FeedInfosElements::CATEGORY;
	if(cmp == _title)
		return FeedInfosElements::TITLE;
	if(cmp == _desc)
		return FeedInfosElements::DESC;
	if(cmp == _date)
		return FeedInfosElements::DATE;
	if(cmp == _multimedia)
		return FeedInfosElements::MULTIMEDIA;
	return FeedInfosElements::NONE;
}
QString FeedInfos::getTitleColor() const
{
	return _titleColor;
}
bool FeedInfos::doTagsDecoding() const
{
	return _tagsDecoding;
}
bool FeedInfos::doRemoveHTML() const
{
	return _removeHTML;
}
bool FeedInfos::doNothing() const
{
	return !(_removeHTML || _tagsDecoding);
}
bool FeedInfos::isDateFormatISO() const
{
	return _dateFormat == Qt::ISODate;
}
bool FeedInfos::isDateFormatRFC() const
{
	return _dateFormat == Qt::RFC2822Date;
}
QString FeedInfos::getFavicon() const
{
	return _faviconUrl;
}
QString FeedInfos::getUrl() const
{
	return _url;
}
QString FeedInfos::name() const
{
	return _name;
}
int FeedInfos::dbid() const
{
	return _id;
}

