#include <QString>

#include "article.h"

Article::Article(int id) {
	_id = id;
}
Article::Article(QString favIcon, int id) {
	_iconUri = favIcon;
	_id = id;
}
void Article::setLink(QString link) {
	_link = link;
}
void Article::updateAuthor(QString author) {
	_author = author;
}
void Article::updateCategory(QString category) {
	_category = category;
}
void Article::updateTitle(QString title) {
	_title = title;
}
void Article::updateDescription(QString desc) {
	_desc = desc;
}
void Article::setStrDate(QString date) {
	_strDate = date;
}
void Article::setNumDate(qint64 date) {
	_numDate = date;
}
void Article::setMedia(QString media) {
	_media = media;
}
QString Article::getMedia() const {
	return _media;
}
QString Article::getDescription() const {
	return _desc;
}
qint64 Article::getNumDate() const {
	return _numDate;
}
void Article::writeXmlStream(QXmlStreamWriter& stream) const {
	stream.writeStartElement("entry");
	stream.writeTextElement("title", _title);
	stream.writeTextElement("shortDesc", _desc);
	stream.writeTextElement("longDesc", _desc);
	stream.writeTextElement("url", _link);
	stream.writeTextElement("iconUri", _iconUri);
	stream.writeTextElement("timestamp", QString::number(_numDate));
	stream.writeTextElement("date", _strDate);
	stream.writeTextElement("category", _category);
	stream.writeTextElement("author", _author);
	stream.writeTextElement("media", _media);
	stream.writeEndElement();//entry
}
