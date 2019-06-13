#ifndef XMLUTILS_H_
#define XMLUTILS_H_

#include <QStringList>
#include <QXmlStreamReader>

#include "autodetectinfos.h"
#include "article.h"

namespace XML {
    QStringList fromOPML(QXmlStreamReader &reader);
    QStringList foundChannel(QXmlStreamReader &reader, const QString &parent, const QString &localname);
    //void extractInnerLinks(articles::Article& article, QString desc);
    QString fastHTMLDecode(QString desc);
    QStringList parseTitle(QXmlStreamReader &xml);
    bool autodetect(QXmlStreamReader &reader, AutodetectInfos &info);
    void extractArticles(const FeedInfos& param, QList<Article>& result, QXmlStreamReader& XMLroot);
    QString cleanHTML(QString desc);
}




#endif // XMLUTILS_H
