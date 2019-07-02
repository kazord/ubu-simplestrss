import QtQuick 2.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import Ubuntu.Components 1.3
import RSSCore 1.0
import QtQuick.XmlListModel 2.0
import QtGraphicalEffects 1.0
import Ubuntu.Components.ListItems 1.3 as ListItem
    
Page {
	id: settingsRSS
	property int dbid: -1
	property var possibleNode: []
	property var possibleSubNode: []

	clip:true
    header: PageHeader {
        id: headerSettings
        title: i18n.tr("RSS settings");
        
            leadingActionBar.actions: [
                Action {
                    iconName: "back"
                    text: "Back"
                    onTriggered: {
                        removePages(settingsRSS);
                    }
                }
            ]
   }       
    Component.onCompleted: {
	possibleNode = RSSCore.getAutodetectValues(dbid)
	possibleSubNode = possibleNode;
	var feed = RSSCore.getDBFeed(settingsRSS.dbid)
	comboboxitem.currentIndex = possibleNode.indexOf(feed.getProp("main")+">"+feed.getProp("item"))
	}




    Flickable {
        id: flickableSettings
        anchors.fill: parent
        anchors.topMargin: settingsRSS.header.height
        contentHeight: settingsRSSColumn.height
        flickableDirection: Flickable.VerticalFlick
        clip: true


        Column {
            id: settingsRSSColumn
            spacing: units.gu(2)
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: units.gu(1)

            }
    
    
                Text {
                    text: i18n.tr("Item")
                    width: units.gu(10)
                    height: units.gu(4)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    verticalAlignment: Text.AlignVCenter
                        
                    ComboBox {
                        id:comboboxitem
                        model: settingsRSS.possibleNode
                        width: units.gu(20)
                        anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        onCurrentIndexChanged: {
                            var item = settingsRSS.possibleNode[currentIndex].split('>')[1];
                            var main = settingsRSS.possibleNode[currentIndex].split('>')[0];
                            settingsRSS.possibleSubNode = []
                            for(var i=0 ; i < settingsRSS.possibleNode.length; i++) {
                                if(settingsRSS.possibleNode[i].startsWith(item+">"))
                                    settingsRSS.possibleSubNode.push(settingsRSS.possibleNode[i]);
                            }
                            //console.log(RSSCore.updateDBFeed(settingsRSS.dbid, "main", main)) 
                            //console.log(RSSCore.updateDBFeed(settingsRSS.dbid, "item", item)) 
                             settingsRSSColumn.comboboxPossibleSubNode = settingsRSS.possibleSubNode //todo a revoir...
                         }                           
                        //the background of the combobox
                          background: Rectangle {
                              height: parent.height
                              color: "#b1b1b1"
                              radius: height/2
                          }

                        
                        Icon {
                            anchors.left: parent.right
                            anchors.leftMargin: units.gu(1.5)
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.height-units.gu(1)
                            height: parent.height-units.gu(1)
                            name: "help"
                                
                            MouseArea {
                                anchors.fill: parent
                                onClicked: ToolTip.visible = true
                                
                                ToolTip.delay: 1000
                                ToolTip.timeout: 5000
                                ToolTip.text: i18n.tr("Container for one article inside the feed")
                                
                            }
                                

                        }
                        
                    }                        
                }    
    
property var comboboxPossibleSubNode: []

property var settingsModel: [
{ name: i18n.tr("Link"), type: "link", help: i18n.tr("Url of the article") },
{ name: i18n.tr("Author"), type: "author", help: i18n.tr("Should contains the source/writer") },
{ name: i18n.tr("Category"), type: "category", help: i18n.tr("For filtering") },
{ name: i18n.tr("Title"), type: "title", help: i18n.tr("Title of article") },
{ name: i18n.tr("Description"), type: "desc", help: i18n.tr("Description of the article") },
{ name: i18n.tr("Date"), type: "date", help: i18n.tr("Don't forget to select the right date format, it's needed ordering") },
{ name: i18n.tr("Media"), type: "multimedia", help: i18n.tr("Image for article") }
]
    
    ListView {
        id: listCombo
        model: settingsRSSColumn.settingsModel
            
        width: parent.width
        height: contentHeight
            
            delegate: Item{
                    height: units.gu(6)

                property var element: model.modelData ? model.modelData : model

                Text {       
                    text: "> " + element.name 
                    width: units.gu(10)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    anchors.verticalCenter: parent.verticalCenter
   
//todo Fonctionne pas !!!                         
Component.onCompleted: {
	var feed = RSSCore.getDBFeed(settingsRSS.dbid)
    comboboxtitle.currentIndex = settingsRSSColumn.comboboxPossibleSubNode.indexOf(feed.getProp("item")+">"+feed.getProp(element.type))
}
    
                    ComboBox {
			            id:comboboxtitle
                        //model: settingsRSSColumn.comboboxPossibleSubNode
                        width: units.gu(20)
                        anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        anchors.verticalCenter: parent.verticalCenter
                        
                            onCurrentIndexChanged: {
                                var item = settingsRSSColumn.comboboxPossibleSubNode[currentIndex].split('>')[1];
                               
                            
                               //todo 
                                //console.log(RSSCore.updateDBFeed(settingsRSS.dbid, element.type, item)) 
                            
                            }
                            
                            
                        //the background of the combobox
                          background: Rectangle {
                              height: parent.height
                              color: "#b1b1b1"
                              radius: height/2
                          }
                            
                        Icon {
                            anchors.left: parent.right
                            anchors.leftMargin: units.gu(1.5)
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.height-units.gu(1)
                            height: parent.height-units.gu(1)
                            name: "help"
                                
                            MouseArea {
                                anchors.fill: parent
                                onClicked: ToolTip.visible = true
                                
                                ToolTip.delay: 1000
                                ToolTip.timeout: 5000
                                ToolTip.text: element.help
                                
                            }
                                

                        }
                        
                    }     
                }
            }
    }


        } // column
    } //flickable

}//page
