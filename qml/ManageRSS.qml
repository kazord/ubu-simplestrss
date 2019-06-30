import QtQuick 2.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import Ubuntu.Components 1.3
import RSSCore 1.0
import QtQuick.XmlListModel 2.0
import QtGraphicalEffects 1.0

Page {
    id: manageRSS
	clip:true
    header: state == "default" ? headerSettings : headerAddFeedSettings
    
    PageHeader {
        id: headerSettings
        title: i18n.tr("RSS management");
        
            leadingActionBar.actions: [
                Action {
                    iconName: "back"
                    text: "Back"
                    onTriggered: {
                        removePages(manageRSS);
                        root.currentIndexList = -1
                    }
                }
            ]
                
            trailingActionBar {
                actions: [
                    Action {
                        iconName: "add"
                        text: i18n.tr("Add RSS")
                        onTriggered: {
                            manageRSS.state = "add"
                        }
                    }
               ]
               numberOfSlots: 2
            }
   }
    
    PageHeader {
        id: headerAddFeedSettings
        visible: manageRSS.state == "add"
        title: i18n.tr("Search")
        leadingActionBar {
            actions: [
                Action {
                    id: closePageAction
                    text: i18n.tr("Close")
                    iconName: "close"
                    onTriggered: {
                        actionError.visible = false
                        manageRSS.state = "default"
                    }
                }

            ]
        }
      contents: Rectangle {
            color: "#fff"
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            TextField {
                id: searchField
                placeholderText: i18n.tr("Enter URL of feed")
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                primaryItem: Icon {
                    anchors.leftMargin: units.gu(0.2)
                    height: parent.height*0.5
                    width: height
                    name: "stock_website"
                }
                hasClearButton: true
                inputMethodHints: Qt.ImhNoPredictiveText
                onVisibleChanged: {
                    if (visible) {
                        forceActiveFocus()
                    }
                }
                onTextChanged: actionError.visible = false
                        ToolTip {
                            id: tooltipError
                            parent: actionError.handle
                            //visible: actionError.pressed
                            text: "No RSS feeds found at this address"
                        }
                    Timer {
                        id: falseTooltip
                        interval: 2000 
                        running: false
                        onTriggered: tooltipError.visible = false
                    }
            }
        }
            trailingActionBar {
                actions: [
                    Action {
                        iconName: "find"
                        onTriggered: {
                            console.log("searching feed"+searchField.text)
                            var result=RSSCore.searchFeed(searchField.text)
                            if(result == "Unknow"){
                                actionError.visible = true
                                tooltipError.visible = true
                                falseTooltip.running = true
                            }else{
                               manageRSS.state = "default"
                                manageRSS.pageStack.addPageToNextColumn(manageRSS, Qt.resolvedUrl("PreviewRSS.qml"), {"feedURL": searchField.text})
                            }
                            //popup.open()
                        }
                    },
                    Action {
                        id: actionError
                        visible: false
                        iconName: "security-alert"
                    onTriggered: {
                        tooltipError.visible = true
                        falseTooltip.running = true
                    }
                    }
               ]
               numberOfSlots: 2
            }
    }           
        
        
        
	ListView {
        id: listAllFeed
        anchors.fill: parent
        anchors.topMargin: manageRSS.header.height+units.gu(1)
        //anchors.top: styleNews.bottom
		anchors.leftMargin: units.gu(1)
		anchors.rightMargin: units.gu(1)
        height: contentHeight
		model: RSSCore.feedlist.length
		delegate: ListItem {
		width: parent.width
		height: layout.height+units.gu(5)
        divider.visible: false
            
           leadingActions: ListItemActions {
               actions: [
                   Action {
                    iconName: "delete"
                    onTriggered: console.log(RSSCore.removeDBFeed(RSSCore.feedlist[index].dbid))  
                   }
               ]
           }
            
        Rectangle {
                color: "black"
                width: parent.width
                height: layout.height+units.gu(4)
                anchors.horizontalCenter: parent.horizontalCenter
                radius: units.gu(2)
                    
                Image {
                    id: iconFeed
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: units.gu(2)
                    anchors.left: parent.left      
                    width: layout.height
                    height: layout.height
                    fillMode: Image.PreserveAspectCrop
                        source: if(RSSCore.feedlist[index].faviconUrl){RSSCore.feedlist[index].faviconUrl}else{"../assets/empty.jpg" }
                    visible: false
                    Component.onCompleted: console.log(RSSCore.feedlist[index].faviconUrl) 
                }
            
                OpacityMask {
                    anchors.fill: iconFeed
                    source: iconFeed
                    maskSource: Rectangle {
                        width: iconFeed.width
                        height: iconFeed.height
                        radius: units.gu(2)
                        visible: false
                    }
                }
            
              Column {
                    id:layout
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: units.gu(2)
                    anchors.left: iconFeed.right      
                    spacing: 10
                    Text { text: RSSCore.feedlist[index].name; color:"white";}
                    Text { text: RSSCore.feedlist[index].url; color:"white"; }
              }
		
		}
		MouseArea {
			anchors.fill: parent
			onClicked: {
                manageRSS.pageStack.addPageToNextColumn(manageRSS, Qt.resolvedUrl("ViewRSS.qml"), {"dbid": RSSCore.feedlist[index].dbid, "titlePage": RSSCore.feedlist[index].name})
			}
            //onPressAndHold: listAllFeed.ViewItems.dragMode =  !listAllFeed.ViewItems.dragMode
		}

		}
        
    ViewItems.onDragUpdated: {
        if (event.status == ListItemDrag.Moving) {
            model.move(event.from, event.to, 1);
        }
    }
    moveDisplaced: Transition {
        UbuntuNumberAnimation {
            property: "y"
        }
    }
}

    
    
    Component.onCompleted: { 
	console.log(RSSCore.feedlist.length)
	console.log(RSSCore.feedlist[0].name+" "+RSSCore.feedlist[0].url)
    }
}//page
