import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import RSSCore 1.0

MainView {
    id: root
    objectName: 'mainView'
    applicationName: 'simplestrss.kazord'
    automaticOrientation: true

    width: units.gu(45)
    height: units.gu(75)

    Page {
        anchors.fill: parent

        header: PageHeader {
            id: header
            title: i18n.tr('SimplestRSS')
        }
	Component {
		id:feeddisplay
		Row {
			spacing: 10
			Text { text: RSSCore.listFeed[index].title }
			Text { text: RSSCore.listFeed[index].url }
		}
	}
	
	ListView {
        	anchors.fill: parent
		model: RSSCore.listFeed.length
		delegate: feeddisplay
    	}
	footer: Column {
		TextInput {

		}
		Button {

		}
	}
    }

    Component.onCompleted: { 
	Pluginname.speak()
	RSSCore.listFeed.append(Feed("test", "http://exemple.com"))
    }
}
