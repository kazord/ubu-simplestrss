import QtQuick 2.4
import QtQuick.Controls 2
import Ubuntu.Components 1.3
import Qt.labs.settings 1.0
import RSSCore 1.0

    
Page {
    id: viewDetailNews

    header: PageHeader {
        title: i18n.tr("Detail article")
            
            trailingActionBar {
                actions: [
                    Action {
                        iconName: "stock_website"
                        text: i18n.tr("Attempt to recover content")
                        onTriggered: {     
                    Qt.openUrlExternally(urlNews);
			var fulltext = RSSCore.fetchFullArticle(urlNews, "auto", descNews)
			if(fulltext != "") descNews = fulltext
                    //viewDetailNews.pageStack.addPageToCurrentColumn(viewDetailNews, Qt.resolvedUrl("ViewDetailRSSWebview.qml"), {"urlNews": urlNews, "titleNews": viewDetailNews.titleNews, "descNews": viewDetailNews.descNews}); 
                    
                    
                            }
                    }
               ]
               numberOfSlots: 2
            }
   }
   
    property string urlNews: ""

    property string titleNews: ""
    property string descNews: ""
        
        Text{
            anchors.fill: parent
            anchors.topMargin: viewDetailNews.header.height+units.gu(1)
            width: parent.width
            wrapMode:Text.WordWrap
            text: viewDetailNews.descNews
            color: "black"
        }    

  
}//page
