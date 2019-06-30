import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import QtQuick.Window 2.2
import RSSCore 1.0
import QtQuick.XmlListModel 2.0
import Qt.labs.settings 1.0
    
MainView {
    id: root
    objectName: 'mainView'
    applicationName: 'simplestrss.kazord'
    automaticOrientation: true
        
    width: if(Screen.desktopAvailableWidth > units.gu(60)){units.gu(125)}else{Screen.desktopAvailableWidth}
    height:if(Screen.desktopAvailableHeight > units.gu(100)){units.gu(125)}else{Screen.desktopAvailableHeight}
                
property int currentIndexList: 0        

    AdaptivePageLayout {
        anchors.fill: parent
        primaryPage: homeRSS

        Page {
            id: homeRSS
                
        header: PageHeader {
            id: header
            title: i18n.tr('SimplestRSS')
                
            StyleHints {
                foregroundColor: "white"
                backgroundColor: "black"
                dividerColor: "black"
            }
            
            contents: Rectangle {
                anchors.fill: parent
                color: "black"
                Label {
                    anchors.centerIn: parent
                    text: header.title
                    color: "white"
                    textSize: Label.Large
                    font.bold: true
                }
            }
            
            trailingActionBar {
                actions: [
                    Action {
                        iconName: "settings"
                        text: "add"
                        onTriggered: {
                            homeRSS.pageStack.addPageToNextColumn(homeRSS, Qt.resolvedUrl("ManageRSS.qml"))
                            root.currentIndexList = -1
                        }
                    }
               ]
               numberOfSlots: 2
            }
        }
            
            
     Rectangle {
        anchors.fill: parent
        color: "black"
          
            
	   ListView {
            id: list
        	anchors.fill: parent
            anchors.topMargin: homeRSS.header.height+units.gu(1)
            anchors.leftMargin: units.gu(2)
            currentIndex: root.currentIndexList
            model: RSSCore.feedlistZero.length
            delegate: Row {
                id:layout
                height: units.gu(6)
                width: parent.width

                Text { anchors.verticalCenter: parent.verticalCenter; text:  RSSCore.feedlistZero[index].name; color: if(list.currentIndex == index){"black"}else{"#b1b1b1"} }

                                
          
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    homeRSS.pageStack.addPageToNextColumn(homeRSS, Qt.resolvedUrl("ViewRSS.qml"), {"dbid": RSSCore.feedlistZero[index].dbid, "titlePage": RSSCore.feedlistZero[index].name})
                    onClicked: root.currentIndexList = index                   
                }
            }
            }//delegate
           
                
            highlight: Rectangle {
                color: "transparent"
                focus: true
                Image {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter;     
                    anchors.right: middlebackground.left
                    source: "../assets/menug.png"
                } 
                Image {
                    id: middlebackground
                    height: units.gu(5)
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    source: "../assets/menum.png"
                }
                Image {
                    height: units.gu(8)
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.right: parent.right
                    source: "../assets/menud.png"
                }
            }

    	}
                
                
            }
    
        }//page
        
        Component.onCompleted: {
            homeRSS.pageStack.addPageToNextColumn(homeRSS, Qt.resolvedUrl("ViewRSS.qml"), {"dbid": 0})
        }
    
    }
}
