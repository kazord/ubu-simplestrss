import QtQuick 2.4
import QtQuick.Controls 2
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import RSSCore 1.0
import QtQuick.XmlListModel 2.0

MainView {
    id: root
    objectName: 'mainView'
    applicationName: 'simplestrss.kazord'
    automaticOrientation: true

    width: units.gu(45)
    height: units.gu(75)

    StackView {
	id: stack
	initialItem: mainpage
	anchors.fill: parent
	}
    Component {
    	id: mainpage
    Page {
        header: PageHeader {
            id: mainpageheader
            title: i18n.tr('SimplestRSS')
        }
	Component {
		id:feeddisplay
		Item {
		width:layout.width
		height:layout.height
		Column {
			id:layout
			spacing: 10
			Text { text: RSSCore.feedlist[index].name }
			Text { text: RSSCore.feedlist[index].url }
			Rectangle { 
				color:"black"
				height:2
				width:units.gu(30)
			}
		
		}
		MouseArea {
			anchors.fill: parent
			onPressed: {
				stack.push(feedpage, {"dbid": RSSCore.feedlist[index].dbid})
			}
		}
		}
	}	
	ListView {
        	anchors.fill: parent
		anchors.topMargin:mainpageheader.height
		model: RSSCore.feedlist.length
		delegate: feeddisplay
    	}
	Rectangle {
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		width: parent.width
		height: units.gu(10)
		color: "red"
		Text { 
			id:addingtext
        		anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			text: "+"
			font.pixelSize:parent.height*0.6
		}
		MouseArea {
			anchors.fill: parent
			onPressed: {
				console.log("adding")
				parent.color = "blue"
				addingtext.font.pixelSize = addingtext.parent.height*0.8
				stack.push(addpage)
			}
			onReleased: {
				addingtext.font.pixelSize = addingtext.parent.height*0.6
				parent.color = "red"
			}
		}
	}
    }
   }
   Component {
	id:addpage
    Page {
	header: PageHeader {
            id: header
            title: i18n.tr('SimplestRSS > Add new feed')
        }
	Rectangle {
		anchors.fill: parent
		color: "purple"
		Item {
			anchors.centerIn:parent
			Column {
				TextField {
					id:mysearchfield
					placeholderText: "Enter url"
				}
				Button {
					text:"search"
					onClicked: {
						console.log("searching feed"+mysearchfield.text)
						var result=RSSCore.searchFeed(mysearchfield.text)
						if(result == "")
							searchresult.text = "Nothing found"
						else
							searchresult.text = result+" feed found"
					}
				}
				Text {
					id:searchresult
				}
			}
		}
	}
	Rectangle {
		id:addpage_footer
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		width: parent.width
		height: units.gu(10)
		color: "blue"
		Row {
			Button { 
				text: "+"
				height:addpage_footer.height
				width:addpage_footer.width/2
					onClicked: {
						console.log(mysearchfield.text)
						console.log(RSSCore.insertFeed(mysearchfield.text))
					}
			}
			Button {
				text: "cancel"
				height:addpage_footer.height
				width:addpage_footer.width/2
				onClicked: {
					stack.pop()
				}
		
			}
		}
		
	}
    }
    }

   Component {
	id:feedpage
    Page {
	id:feedpage_page
	property int dbid: -1
	header: PageHeader {
            id: feedpage_header
            title: i18n.tr('SimplestRSS > View feed')
        }
	Component {
		id:feeditem
		Column {
			spacing: 10
			Text { text: title }
			Text { text: content }
			Rectangle { 
				color:"black"
				height:2
				width:units.gu(30)
			}
		
		}
	}

	ListView {
		anchors.fill: parent
		anchors.topMargin:feedpage_header.height
		model: xmlNewsList
		delegate: feeditem
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
			console.log("xmlmodel:"+feedpage_page.dbid)
                        xmlNewsList.xml = RSSCore.fetchFeed(feedpage_page.dbid);
			console.log(xmlNewsList.xml)
        }

	}
	Rectangle {
		id:feedpage_footer
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		width: parent.width
		height: units.gu(10)
			Button { 
				text: "return"
				height:feedpage_footer.height
				width:feedpage_footer.width
					onClicked: {
						stack.pop()
					}
			}


	}
	}
    }
    

    Component.onCompleted: { 
	console.log(RSSCore.feedlist.length)
	console.log(RSSCore.feedlist[0].name+" "+RSSCore.feedlist[0].url)
    }
}
