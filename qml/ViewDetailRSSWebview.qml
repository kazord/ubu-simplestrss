import QtQuick 2.4
import QtQuick.Controls 2
import Ubuntu.Components 1.3
import Qt.labs.settings 1.0
    
Page {
    id: viewDetailNews

    header: PageHeader {
        title: i18n.tr("View site")
            
   }
   
    
    property string urlNews: ""

    property string titleNews: ""
    property string descNews: ""        
      
        
        
WebView {
    anchors.fill: parent
    anchors.topMargin: viewDetailNews.header.height
    url: urlNews
}        
        

  
}//page
