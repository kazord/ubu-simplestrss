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
	property var feed
        
	onPossibleNodeChanged: { 
		if(!feed) return
		settingsRSS.possibleSubNode = []
                for(var i=0 ; i < settingsRSS.possibleNode.length; i++) {
                	if(settingsRSS.possibleNode[i].startsWith(settingsRSS.possibleNode[comboboxitem.currentIndex].split('>')[1]+">"))
                        	settingsRSS.possibleSubNode.push(settingsRSS.possibleNode[i]);
                }
		console.log(settingsRSS.possibleSubNode)
		possibleSubNodeChanged()
	}
	onPossibleSubNodeChanged: {
		if(!feed) return
		for(var i=0; i< settingsRSSColumn.settingsModel.length+1;i++) {//liewview selector is the +1 and if
			if(listCombo.children[0].children[i].children.length>0) {
				//listCombo.children[0].children[i].children[0].children[0].model = settingsRSS.possibleSubNode
				listCombo.children[0].children[i].children[0].children[0].modelChanged
				
				console.log("sub changed "+i)
			}
		}
		console.log("sub changed")
		//console.log(listCombo.children[0].children[0].children[0].children[0].model)
	 }


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
            trailingActionBar.actions: [
                    Action {
                        iconName: "reset"
                        text: "reset"
                        onTriggered: {   
                            RSSCore.updateDBAutoFeed(dbid) 
                            }
                    }
            ]
   }       
    Component.onCompleted: {
	settingsRSS.feed = RSSCore.getDBFeed(settingsRSS.dbid)
	possibleNode = RSSCore.getAutodetectValues(dbid)
	comboboxitem.currentIndex = possibleNode.indexOf(settingsRSS.feed.getProp("main")+">"+settingsRSS.feed.getProp("item"))
	comboboxdateformat.dateindex = feed.dateFormat;	
	switchCleanHTML.checked = feed.removeHTML;
	switchTagsDecoding.checked = feed.tagsDecoding;
	possibleNodeChanged()
	settingsRSSLoading.z = -1
	settingsRSSLoading.opacity = 0

	}




    Flickable {
        id: flickableSettings
        anchors.fill: parent
        anchors.topMargin: settingsRSS.header.height
        contentHeight: settingsRSSColumn.height+units.gu(2)
        flickableDirection: Flickable.VerticalFlick
        clip: true
	
	Rectangle {
		z:1
		id:settingsRSSLoading
		color: "white"
		anchors.fill: parent
		Label {
			text:"Loading..."	
			anchors.centerIn: parent
			textSize:Label.Large
		}
		MouseArea {
			anchors.fill: parent
		}
	}

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
                    text: i18n.tr("Color")
                    width: units.gu(13)
                    height: units.gu(4)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    verticalAlignment: Text.AlignVCenter
                    color: if(Theme.name != "Ubuntu.Components.Themes.Ambiance"){"white"}else{"black"}
                    
                    TextField {
                        id: textFieldColor
                        width: units.gu(20)
                        height: units.gu(4)
                        anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        text: RSSCore.getDBFeed(settingsRSS.dbid).color
                        onTextChanged: RSSCore.updateDBFeed(settingsRSS.dbid, "color", text)

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
                                ToolTip.text: i18n.tr("Color of feed (ex: #B1B1B1 OR grey)")
                                
                            }
                                

                        }
                        
                    }                        
                }
    
    
                Text {
                    text: i18n.tr("Item")
                    width: units.gu(13)
                    height: units.gu(4)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    verticalAlignment: Text.AlignVCenter
                    color: if(Theme.name != "Ubuntu.Components.Themes.Ambiance"){"white"}else{"black"}

                    ComboBox {
                        id:comboboxitem
                        model: settingsRSS.possibleNode
                        width: units.gu(20)
                        height: units.gu(4)
                        anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        onCurrentIndexChanged: {
                            var item = settingsRSS.possibleNode[currentIndex].split('>')[1];
                            var main = settingsRSS.possibleNode[currentIndex].split('>')[0];
                            console.log(RSSCore.updateDBFeed(settingsRSS.dbid, "main", main)) 
                            console.log(RSSCore.updateDBFeed(settingsRSS.dbid, "item", item)) 
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
    

		property var settingsModel: [
			{ name: i18n.tr("Link"), type: "link", help: i18n.tr("Url of the article") },
			{ name: i18n.tr("Author"), type: "author", help: i18n.tr("Should contains the source/writer") },
			{ name: i18n.tr("Category"), type: "category", help: i18n.tr("For filtering") },
			{ name: i18n.tr("Title"), type: "title", help: i18n.tr("Title of article") },
			{ name: i18n.tr("Description"), type: "desc", help: i18n.tr("Description of the article") },
			{ name: i18n.tr("Media"), type: "multimedia", help: i18n.tr("Image for article") },
            { name: i18n.tr("Date"), type: "date", help: i18n.tr("Don't forget to select the right date format, it's needed ordering") }
	]
    
    ListView {
        id: listCombo
        model: settingsRSSColumn.settingsModel
            
        width: parent.width
        height: contentHeight
            
            delegate: Item{
                    height: units.gu(6)

                property var element: listCombo.model[index]

                Text {       
                    text: "> " + element.name 
                    width: units.gu(13)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    anchors.verticalCenter: parent.verticalCenter
                    color: if(Theme.name != "Ubuntu.Components.Themes.Ambiance"){"white"}else{"black"}
    
                    ComboBox {
			id:comboboxtitle
                        //model: settingsRSSColumn.comboboxPossibleSubNode
                        model: settingsRSS.possibleSubNode
                        width: units.gu(20)
                        height: units.gu(4)
                        anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        anchors.verticalCenter: parent.verticalCenter
                        
                            onCurrentIndexChanged: {
                                if(settingsRSS.possibleSubNode[currentIndex]) {
					var item = settingsRSS.possibleSubNode[currentIndex].split('>')[1];
					console.log(item)
					console.log(RSSCore.updateDBFeed(settingsRSS.dbid, element.type, item)) 
				}
                            
                            }
                            
			onModelChanged: {
			    //console.log(settingsRSS.feed.getProp("item")+">"+settingsRSS.feed.getProp(element.type))
			    currentIndex = settingsRSS.possibleSubNode.indexOf(settingsRSS.feed.getProp("item")+">"+settingsRSS.feed.getProp(element.type))
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

    

                Text {
                    text: i18n.tr("Format date") 
                    width: units.gu(13)
                    height: units.gu(4)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    verticalAlignment: Text.AlignVCenter
                    color: if(Theme.name != "Ubuntu.Components.Themes.Ambiance"){"white"}else{"black"}
                        
                    ComboBox {
			id: comboboxdateformat
			property int dateindex: 0;
                        width: units.gu(20)
                        height: units.gu(4)
                        anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        model: [ "ISO", "RFC", "TXT" ]
                        currentIndex: 2
                        
                        onCurrentIndexChanged: {
				if(!settingsRSS.feed) return;
				if(currentIndex == 0){
					RSSCore.updateDBFeedInt(settingsRSS.dbid, "date_format", 1)
					dateindex = 1
				}
				if(currentIndex == 1){
					RSSCore.updateDBFeedInt(settingsRSS.dbid, "date_format", 8)
					dateindex = 8
				}
				if(currentIndex == 2){
					RSSCore.updateDBFeedInt(settingsRSS.dbid, "date_format", 0)
					dateindex = 0
				} 
                         }         
			onDateindexChanged: {
				if(dateindex == 0)
					currentIndex = 2
				else if(dateindex == 1)
					currentIndex = 0
				else if(dateindex == 8)
					currentIndex = 1
				else
					currentIndex = 0
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
                                ToolTip.text: i18n.tr("Date format to parse")                                
                            }
                                

                        }
                        
                    }                        
                        
                                                
                }
   		Text { 
                	text: i18n.tr("Content: cleanup HTML") 
                    width: units.gu(30)
                    height: units.gu(4)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    verticalAlignment: Text.AlignVCenter
                    color: if(Theme.name != "Ubuntu.Components.Themes.Ambiance"){"white"}else{"black"}
     			Switch {
			anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        	id:switchCleanHTML;
				checked:false
			onCheckedChanged: {
					RSSCore.updateDBFeedInt(settingsRSS.dbid, "cleanHTML", checked)
			}	
                        
                        Icon {
                            anchors.left: parent.right
                            anchors.leftMargin: units.gu(1.5)
                            anchors.verticalCenter: parent.verticalCenter
                            width: units.gu(4)-units.gu(1)
                            height: units.gu(4)-units.gu(1)
                            name: "help"
                                
                            MouseArea {
                                anchors.fill: parent
                                onClicked: ToolTip.visible = true
                                
                                ToolTip.delay: 1000
                                ToolTip.timeout: 5000
                                ToolTip.text: i18n.tr("Remove all HTML elements")                                
                            }
                                

                        }
                        
                       }                    
                        
                                                
                }
		Text { 
                	text: i18n.tr("Content: decode special charachers") 
                    width: units.gu(30)
                    height: units.gu(4)
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)
                    verticalAlignment: Text.AlignVCenter
                    color: if(Theme.name != "Ubuntu.Components.Themes.Ambiance"){"white"}else{"black"}
                        
     			Switch {
			anchors.left: parent.right
                        anchors.leftMargin: units.gu(2)
                        	id:switchTagsDecoding;
				checked:false
			
			onCheckedChanged: {
					RSSCore.updateDBFeedInt(settingsRSS.dbid, "tagsRemove", checked)
			}	
                        
                        Icon {
                            anchors.left: parent.right
                            anchors.leftMargin: units.gu(1.5)
                            anchors.verticalCenter: parent.verticalCenter
                            width: units.gu(4)-units.gu(1)
                            height: units.gu(4)-units.gu(1)
                            name: "help"
                                
                            MouseArea {
                                anchors.fill: parent
                                onClicked: ToolTip.visible = true
                                
                                ToolTip.delay: 1000
                                ToolTip.timeout: 5000
                                ToolTip.text: i18n.tr("Convert &xxx; into readable characters")                                
                            }
                                

                        }
                        
                                           
                       } 
                                                
                }
		
    

        } // column
    } //flickable

}//page
