#include "xml.h"

#include <QDateTime>
#include <QRegExp>
#include <QTextCodec>
#include <QStringList>
#include <QDebug>

#include "autodetectinfos.h"
#include "article.h"

#define TRUNC_LENGTH 17
namespace config {
	int articles_limit = 15;
}
namespace XML {

	namespace {
		QString readText(QXmlStreamReader& xml)
		{
			xml.readNext();
			if (xml.tokenType() != QXmlStreamReader::Characters) {
				return QString();
			}

			return xml.text().toString();
		}
		QString readContent(QXmlStreamReader& xml)
		{
			QString content = "";
			QStringRef target = xml.qualifiedName();
			xml.readNext();
			while (!xml.atEnd() && !(xml.isEndElement() && xml.qualifiedName() == target))
			{

				if (xml.isStartElement())
					content.append("<"+xml.qualifiedName().toString()+">");
				else if (xml.isEndElement())
					content.append("</"+xml.qualifiedName().toString()+">");
				else if(xml.tokenType() == QXmlStreamReader::Characters)
					content.append(xml.text().toString());

				xml.readNext();

			}
			return content;//xml.readElementText(QXmlStreamReader::IncludeChildElements);

		}
		QStringList subFoundChannel(QXmlStreamReader &xml, QString parent, QString ignore) {
			QStringList list;
			while (!xml.atEnd() && !(xml.isEndElement() && xml.qualifiedName().toString() == parent))
			{
				if (xml.isStartElement() && !xml.name().toString().isEmpty()) {
					if(xml.qualifiedName().toString() == ignore) {
						subFoundChannel(xml, ignore, "");//ignore entry zone
					}
					else if(xml.name().toString() == "title") {
						list << readText(xml);
					}
				}
				xml.readNext();

			}
			return list;
		}

		void parse4Analyse(QXmlStreamReader &xml, AutodetectInfos &info, QString parentName) {
			AutodetectNode node(xml.qualifiedName().toString(), parentName);	
			//qDebug() << "~" << xml.qualifiedName().toString() << "~";
			info.updateKnownNode(node);
			bool next = true;
			QString name(xml.qualifiedName().toString());
			if(name == "channel" 
					|| name == "feed") {
			}
			if(name == "item"
					|| name == "entry") {
				info.updateItem(node);
			}
			if(name == "title")
				info.updateTitle(node);
			if(name == "category")
				info.updateCategory(node);
			if(name == "description"
					|| name == "subtitle"
					|| name == "summary"
					|| name == "content:encoded" //wordpress default fullmode (content:encoded parsed as encoded)
					|| name == "content") {
				info.updateDesc(node);
				QString content = readContent(xml);
				next = false;
				if(content.isEmpty()) { //this is a trap description, *ignore it*
					//qDebug() << "description with :" << name;
					return;
				}
				QRegExp rx("&[a-zA-Z]{2,4};"); //looking for encoded html
				if(content.contains(rx)) {
					info.useHtmlDecode();	
				}
			}
			if(
					name == "media:content" //wordpress default image (media:content parsed as content)
					|| name == "media"
					|| name == "image"
					|| name == "enclosure")
				info.updateMultimedia(node);
			if(name == "link")
				info.updateLink(node);
			if(name == "pubDate"
					|| name == "published"
					|| name == "updated"
					|| name == "date" ) {
				info.updateDate(node);
				QString datetext = readText(xml);
				if(QDateTime::fromString(datetext,Qt::ISODate).toMSecsSinceEpoch() > QDateTime::fromString(datetext,Qt::RFC2822Date).toMSecsSinceEpoch() 
						&& QDateTime::fromString(datetext,Qt::ISODate).toMSecsSinceEpoch() >= QDateTime::fromString(datetext).toMSecsSinceEpoch()) {
					info.setDateFormatISO();
				}
				else if(QDateTime::fromString(datetext,Qt::RFC2822Date).toMSecsSinceEpoch() > QDateTime::fromString(datetext,Qt::ISODate).toMSecsSinceEpoch() 
						&& QDateTime::fromString(datetext,Qt::RFC2822Date).toMSecsSinceEpoch() >= QDateTime::fromString(datetext).toMSecsSinceEpoch()) {
					info.setDateFormatRFC();
				}
				else {
					info.setDateFormatTXT();
				}
			}
			if(name == "author"
					|| name == "contributor"
					|| name == "dc:creator"
					|| name == "creator") {
				info.updateAuthor(node);
			}
			if(next)
				xml.readNext();
			while (!xml.atEnd() && !(xml.isEndElement() && xml.qualifiedName() == node.first))
			{
				//qDebug() << xml.name() << xml.isStartElement() << xml.isEndElement() << xml.qualifiedName();
				if (xml.isStartElement() && !xml.isEndElement() && !xml.qualifiedName().toString().isEmpty()) {
					parse4Analyse(xml, info, node.first);
				}
				else
					xml.readNext();

			}
		}
	}

	QStringList feedsFromOPML(QXmlStreamReader &reader) {
		QStringList urls;
		QString url;
		QString title;
		while (!reader.atEnd() && !reader.hasError())
		{
			if(reader.isStartElement()) {
				QXmlStreamAttributes attributes = reader.attributes();
				if (attributes.hasAttribute("type") && attributes.value("type") == "rss") {
					if (attributes.hasAttribute("xmlUrl")) {
						url = attributes.value("xmlUrl").toString();
						if(attributes.hasAttribute("title")) {
							title = attributes.value("title").toString();
						}
						else title = "";
						urls << url << title;
					}
				}
			}
			reader.readNext();
		}//loop
		return urls;
	}	

	QStringList foundChannel(QXmlStreamReader &reader, const QString &parent, const QString &localname) {
		QStringList list;
		while (!reader.atEnd() && !reader.hasError()) {
			QXmlStreamReader::TokenType token = reader.readNext();

			/* If token is just StartDocument, we'll go to next.*/
			if (token == QXmlStreamReader::StartDocument) {
				continue;
			}

			/* If token is StartElement, we'll see if we can read it.*/
			if (token == QXmlStreamReader::StartElement) {
				if(reader.qualifiedName().toString() == parent) {
					list  << subFoundChannel(reader, parent, localname);
				}
			}
		}
		return list;
	}


	bool autodetect(QXmlStreamReader &reader, AutodetectInfos &info) {
		parse4Analyse(reader,info,"");
		//if(!info.item.name.isEmpty() && ( info.item.name == info.title.parent || info.item.name == info.desc.parent || info.item.name == info.link.parent || info.item.name == info.category.parent  || info.item.name == info.author.parent ))
		return info.isValid();
	}

	QStringList parseTitle(QXmlStreamReader &xml) {
		xml.readNext();
		QStringList titles;
		while (!xml.atEnd() && !(xml.isEndElement() && xml.name() == "channel"))
		{
			if (xml.isStartElement() && xml.name() == "title") {
				titles << readText(xml);
			}
			xml.readNext();
		}
		if(xml.isEndElement() && xml.name() == "channel")
			xml.readNext();
		return titles;
	}

	/************************** articles utils ******************/
	QString cleanHTML(QString desc) {
		QRegExp rx2 = QRegExp("<p( [^>]+)?>");
		rx2.setMinimal(true);
		desc.replace(rx2, "\n");
		QRegExp rx = QRegExp("<[^>]+>");
		rx.setMinimal(true);
		desc.replace(rx, "");
		return desc;
	}
	QString fastHTMLDecode(QString desc) {
		QRegExp rx1("&#x([a-fA-F0-9]{1,4});");
		int pos = 0;
		bool ok;
		while ((pos = rx1.indexIn(desc,pos)) != -1) {
			desc.replace(pos, rx1.cap(1).size()+4, QChar(rx1.cap(1).toInt(&ok,16)));
		}
		QRegExp rx2("&#([0-9]{1,4});");
		pos = 0;
		while ((pos = rx2.indexIn(desc,pos)) != -1) {
			desc.replace(pos, rx2.cap(1).size()+3, QChar(rx2.cap(1).toInt(&ok,10)));
		}
		desc.replace(QString("&amp;"), QString("&"));
		desc.replace(QString("&apos;"), QString("'"));
		desc.replace(QString("&quot;"), QString("\""));
		desc.replace(QString("&mdash;"), QString("—"));
		desc.replace(QString("&ndash;"), QString("–"));
		desc.replace(QString("&lt;"), QString("<"));
		desc.replace(QString("&gt;"), QString(">"));
		desc.replace(QString("&nbsp;"), QString(" "));
		desc.replace(QString("&rsquo;"), QString("’"));
		desc.replace(QString("&laquo;"), QString("«"));
		desc.replace(QString("&raquo;"), QString("»"));
		//desc.replace(QString("&frac14;"), QString("¼"));
		//desc.replace(QString("&frac12;"), QString("½"));
		//desc.replace(QString("&frac34;"), QString("¾"));
		desc.replace(QString("&Agrave;"), QString("À"));
		desc.replace(QString("&Aacute;"), QString("Á"));
		desc.replace(QString("&Acirc;"), QString("Â"));
		//desc.replace(QString("&Atilde;"), QString("Ã"));
		desc.replace(QString("&Auml;"), QString("Ä"));
		//desc.replace(QString("&Aring;"), QString("Å"));
		//desc.replace(QString("&AElig;"), QString("Æ"));
		desc.replace(QString("&Ccedil;"), QString("Ç"));
		desc.replace(QString("&Egrave;"), QString("È"));
		//desc.replace(QString("&Eacute;"), QString("É"));
		//desc.replace(QString("&Ecirc;"), QString("Ê"));
		desc.replace(QString("&Euml;"), QString("Ë"));
		desc.replace(QString("&Igrave;"), QString("Ì"));
		desc.replace(QString("&Iacute;"), QString("Í"));
		desc.replace(QString("&Icirc;"), QString("Î"));
		desc.replace(QString("&Iuml;"), QString("Ï"));
		//desc.replace(QString("&ETH;"), QString("Ð"));
		//desc.replace(QString("&Ntilde;"), QString("Ñ"));
		desc.replace(QString("&Ograve;"), QString("Ò"));
		desc.replace(QString("&Oacute;"), QString("Ó"));
		desc.replace(QString("&Ocirc;"), QString("Ô"));
		//desc.replace(QString("&Otilde;"), QString("Õ"));
		desc.replace(QString("&Ouml;"), QString("Ö"));
		desc.replace(QString("&times;"), QString("×"));
		desc.replace(QString("&Oslash;"), QString("Ø"));
		desc.replace(QString("&Ugrave;"), QString("Ù"));
		desc.replace(QString("&Uacute;"), QString("Ú"));
		desc.replace(QString("&Ucirc;"), QString("Û"));
		desc.replace(QString("&Uuml;"), QString("Ü"));
		//desc.replace(QString("&Yacute;"), QString("Ý"));
		//desc.replace(QString("&THORN;"), QString("Þ"));
		//desc.replace(QString("&szlig;"), QString("ß"));
		desc.replace(QString("&agrave;"), QString("à"));
		desc.replace(QString("&aacute;"), QString("á"));
		desc.replace(QString("&acirc;"), QString("â"));
		//desc.replace(QString("&atilde;"), QString("ã"));
		desc.replace(QString("&auml;"), QString("ä"));
		//desc.replace(QString("&aring;"), QString("å"));
		//desc.replace(QString("&aelig;"), QString("æ"));
		desc.replace(QString("&ccedil;"), QString("ç"));
		desc.replace(QString("&egrave;"), QString("è"));
		desc.replace(QString("&eacute;"), QString("é"));
		desc.replace(QString("&ecirc;"), QString("ê"));
		desc.replace(QString("&euml;"), QString("ë"));
		desc.replace(QString("&igrave;"), QString("ì"));
		desc.replace(QString("&iacute;"), QString("í"));
		desc.replace(QString("&icirc;"), QString("î"));
		desc.replace(QString("&iuml;"), QString("ï"));
		//desc.replace(QString("&eth;"), QString("ð"));
		//desc.replace(QString("&ntilde;"), QString("ñ"));
		desc.replace(QString("&ograve;"), QString("ò"));
		desc.replace(QString("&oacute;"), QString("ó"));
		desc.replace(QString("&ocirc;"), QString("ô"));
		//desc.replace(QString("&otilde;"), QString("õ"));
		desc.replace(QString("&ouml;"), QString("ö"));
		desc.replace(QString("&divide;"), QString("÷"));
		desc.replace(QString("&oslash;"), QString("ø"));
		desc.replace(QString("&ugrave;"), QString("ù"));
		desc.replace(QString("&uacute;"), QString("ú"));
		desc.replace(QString("&ucirc;"), QString("û"));
		desc.replace(QString("&uuml;"), QString("ü"));
		//desc.replace(QString("&yacute;"), QString("ý"));
		//desc.replace(QString("&thorn;"), QString("þ"));
		//desc.replace(QString("&yuml;"), QString("ÿ"));

		return desc;
	}
	namespace {
		void parseArticle(const FeedInfos& param, Article& article, QXmlStreamReader& xml)
		{

			while (!xml.atEnd() && !(xml.isEndElement() && param.isItem(xml.qualifiedName())))
			{
				bool readnext = true;
				if(xml.isStartElement()) {
					switch (param.usedElement(xml.qualifiedName())) {
						case FeedInfosElements::LINK:
							{
								QXmlStreamAttributes attributes = xml.attributes();
								article.setLink((attributes.hasAttribute("href"))? attributes.value("href").toString() : readText(xml));
								break;
							}
						case FeedInfosElements::AUTHOR:
							{
								article.updateAuthor(cleanHTML(xml.readElementText(QXmlStreamReader::IncludeChildElements)).simplified());
								break;
							}
						case FeedInfosElements::CATEGORY:
							{
								article.updateCategory(readText(xml));
								break;
							}
						case FeedInfosElements::TITLE:
							{
								//article.updateTitle("<font color='"+param.getTitleColor()+"'>"+readText(xml).replace("&","&amp;")+"</font>");
								article.updateTitle(readText(xml).replace("&","&amp;"));
								break;	
							}
						case FeedInfosElements::DESC:
							{
								QString desc = readContent(xml);
								article.updateDescription(desc);
								break;
							}
						case FeedInfosElements::DATE:
							{
								if(param.isDateFormatISO()){
									article.setNumDate(QDateTime::fromString(readText(xml),Qt::ISODate).toMSecsSinceEpoch());
								} else if(param.isDateFormatRFC()) {
									article.setNumDate(QDateTime::fromString(readText(xml),Qt::RFC2822Date).toMSecsSinceEpoch());
								} else {//try basic
									article.setNumDate(QDateTime::fromString(readText(xml)).toMSecsSinceEpoch());
								}
								article.setStrDate(QDateTime::fromMSecsSinceEpoch(article.getNumDate()).toLocalTime().toString(Qt::DefaultLocaleShortDate));
								break;
							}
						case FeedInfosElements::MULTIMEDIA:
							{
								QXmlStreamAttributes attributes = xml.attributes();
								article.setMedia((attributes.hasAttribute("url"))? attributes.value("url").toString() : readText(xml));
								break;
							}
						case FeedInfosElements::ITEM:
							{
								break;
							}
						default:
							xml.readElementText(QXmlStreamReader::IncludeChildElements);
							break;
					}

				}
				if(readnext)
					xml.readNext();
			}//loop
			//TODO use something else to fetch full article.readability = (param.html == feed::HTML::ReadabilityOnOpen);
			//do an article config & copy as it article.icon = param.favicon();

			if(article.getMedia() == "") {
				QRegExp rx("<img [^>]*src=\"([^\"]+)\"");
				rx.setCaseSensitivity(Qt::CaseInsensitive);
				rx.setMinimal(true);
				int pos = rx.indexIn(article.getDescription());
				//cerr << "#" << article.description <<"#"<< pos << endl;
				if (pos > -1) {
					article.setMedia(rx.cap(1));
					//article.updateDescription(rx.cap(1)+" "+rx.cap(4));
				}
				else
					article.setMedia(param.getFavicon());
			}
			if(param.doTagsDecoding())
				article.updateDescription(fastHTMLDecode(article.getDescription()));
			if(param.doRemoveHTML())
				article.updateDescription(cleanHTML(article.getDescription()));
			if(param.doNothing())
				article.updateDescription(article.getDescription().replace("&","&amp;"));

		}

		bool parseRSS(const FeedInfos& param, QList<Article>& articles, QXmlStreamReader& xml)
		{

			QList<Article> parsed;

			while (!xml.atEnd() && !(xml.isEndElement() && param.isMain(xml.qualifiedName())) && parsed.size() <= config::articles_limit)
			{
				if(xml.isStartElement()) {
					if (param.isItem(xml.qualifiedName())) {
						QXmlStreamAttributes attr;
						attr.append("color", param.getColor());
						Article article(param.getFavicon(), attr, parsed.size());
						parseArticle(param, article, xml);
						parsed << article;
					}
				}

				xml.readNext();
			}
			articles << parsed;
			if(parsed.size() > config::articles_limit) {
				return true;
			}
			else
				return false;
		}
	}


	void extractArticles(const FeedInfos& param, QList<Article>& result, QXmlStreamReader& XMLroot) {

		//qDebug() << "expected main:"<<param.main;
		while (!XMLroot.atEnd() && !XMLroot.hasError()) {
			QXmlStreamReader::TokenType token = XMLroot.readNext();

			/* If token is just StartDocument, we'll go to next.*/
			if (token == QXmlStreamReader::StartDocument) {
				continue;
			}

			/* If token is StartElement, we'll see if we can read it.*/
			if (token == QXmlStreamReader::StartElement) {
				if (param.isMain(XMLroot.qualifiedName())) {
					if(parseRSS(param, result, XMLroot))
						break;
				}
				//else qDebug() << "main:"<<XMLroot.name().toString();
			}
		}

		if (XMLroot.hasError()) {
			//qDebug() << "xml error" << XMLroot.characterOffset();
			//throw std::domain_error(XMLroot.errorString().toStdString());
		}
		//qDebug() << "found articles :" << result.size();
	}

} // XML

