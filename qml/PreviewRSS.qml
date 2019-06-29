import QtQuick 2.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import Ubuntu.Components 1.3
import RSSCore 1.0
import QtQuick.XmlListModel 2.0
import QtGraphicalEffects 1.0

Page {
    id: previewRSS

    header: PageHeader {
        id: headerSettings
        title: i18n.tr("Feed preview")
        
            leadingActionBar.actions: [
                Action {
                    iconName: "back"
                    text: "Back"
                    onTriggered: {
                        removePages(previewRSS);
                    }
                }
            ]
                
            trailingActionBar {
                actions: [
                     Action {
                        iconName: "ok"
                        text: i18n.tr("Add RSS")
                        onTriggered: {
                            console.log(RSSCore.insertFeed(previewRSS.feedURL))
                            removePages(previewRSS);
                        }
                    },
                    Action {
                        iconName: "edit"
                        text: i18n.tr("edit")
                        onTriggered: {

                        }
                    }
               ]
               numberOfSlots: 2
            }
   }
    

	property var feedURL: ""
    

    GridView {
        id: gview
		anchors.fill: parent
		anchors.topMargin: previewRSS.header.height+units.gu(2)
        cellHeight: iconbasesize+units.gu(2)
        property real iconbasesize: units.gu(20)
        cellWidth: Math.floor(width/Math.floor(width/iconbasesize))

        focus: true
        model: xmlNewsList

        delegate: Rectangle {
                    width: gview.cellWidth
                    height: gview.iconbasesize
                    color: "transparent"

                    Rectangle {
                        color: "black"
                        width: units.gu(19)
                        height: units.gu(19)
                        anchors.horizontalCenter: parent.horizontalCenter
                        radius: units.gu(2)
                                
                                
                        Image {
                            id: imgIcons
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width-units.gu(2)
                            height: parent.height-units.gu(2)
                            
                            fillMode: Image.PreserveAspectCrop
                            source: media ? media : "../assets/empty.jpg"

                            visible: false
                        }

                        OpacityMask {
                            anchors.fill: imgIcons
                            source: imgIcons
                            maskSource: Rectangle {
                                width: imgIcons.width
                                height: imgIcons.height
                                radius: units.gu(2)
                                visible: false
                            }
                        }

                        Rectangle {
                            width: units.gu(19)
                            height: titleMovie.contentHeight
                            color: "#000000"
                            anchors.bottom: imgIcons.bottom
                            opacity: 0.8
                            radius: units.gu(2)

                            Text{
                                id: titleMovie
                                horizontalAlignment: Text.AlignHCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                                width: parent.width-units.gu(1)
                                wrapMode:Text.WordWrap
                                text: title
                                color: "#ffffff"
                                maximumLineCount: 3
                                elide: Text.ElideRight
                            }
                        }


                    } // Item
            }// delegate Rectangle

        }
        
    
	XmlListModel {
	id:xmlNewsList
	xml: ""
    query: "/entries/entry"

    XmlRole { name: "title"; query: "title/string()" }
    XmlRole { name: "shortDesc"; query: "shortDesc/string()" }
    XmlRole { name: "longDesc"; query: "longDesc/string()" }
    XmlRole { name: "url"; query: "url/string()" }
    XmlRole { name: "iconUri"; query: "iconUri/string()" }
    XmlRole { name: "timestamp"; query: "timestamp/number()" }
    XmlRole { name: "category"; query: "category/string()" }
    XmlRole { name: "author"; query: "author/string()" }
    XmlRole { name: "media"; query: "media/string()" }
        
        Component.onCompleted: {
            xmlNewsList.xml = RSSCore.previewFeed(feedURL);
        }
	}
    
}//page