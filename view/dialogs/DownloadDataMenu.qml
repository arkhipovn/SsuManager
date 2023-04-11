import QtQuick 2.12
import QtQuick.Controls 2.12

import QCC.Controls 1.0 as QCC
import QCC.Controls 2.0 as QCC2
import Theme 1.0

Loader {
    id: loader
    active: false
    onLoaded: item.open()

    function open() {
        time1 = new Date(NaN)
        time2 = new Date(NaN)
        active = true
    }

    function close() {
        active = false
    }

    signal downloadData(var time1, var time2)

    property var time1: new Date(NaN)
    property var time2: new Date(NaN)
    property bool dataLoading: false

    onDataLoadingChanged: if(!dataLoading) close()

    sourceComponent: dataLoading ? null
                                 : downloadMenuComponent

    Component {
        id: downloadMenuComponent

        Menu {
            id: downloadMenu
            width: loader.width
            modal: true
            onClosed: loader.close()

            function getDate(addDate = 0) {
                var now = new Date;
                if(addDate !== 0)
                    now.setDate(now.getDate() + addDate);
                now.setHours(0, 0, 0, 0)
                return now
            }

            MenuItem {
                text: qsTr("Data for today")    // За сегодня
                onClicked: {
                    time1 = downloadMenu.getDate()
                    time2 = downloadMenu.getDate(+1)      // за сегодня
                    loader.downloadData(time1, time2)
//                    downloadMenu.close()
                }
            }

            MenuItem {
                text: qsTr("Weekly data")    // За неделю
                onClicked: {
                    time1 = downloadMenu.getDate(-7)
                    time2 = new Date(NaN)
                    loader.downloadData(time1, time2)
                }
            }

            MenuItem {
                text: qsTr("Data for the month")    // За месяц
                onClicked: {
                    time1 = downloadMenu.getDate(-30)
                    time2 = new Date(NaN)
                    loader.downloadData(time1, time2)
                }
            }

            Menu {
                title: qsTr("Data for specific dates")    // Задать даты

                padding: Theme.smallIndent

                Column {
                    width: parent.width

                    QCC.DateInput {
                        id: dateInput1
                        width: parent.width
                        dateTime: programSettings.getCurrentDateTime(-86400)
                        format: programSettings.dateTimeFormat
                        placeholderText: qsTr("From")
                    }

                    QCC.DateInput {
                        id: dateInput2
                        width: parent.width
                        dateTime: programSettings.getCurrentDateTime()
                        format: programSettings.dateTimeFormat
                        placeholderText: qsTr("To")
                    }

                    Button {
                        enabled: dateInput1.acceptableInput && dateInput2.acceptableInput
                        width: parent.width
                        text: qsTr("Download")
                        onClicked: {
                            time1 = programSettings.dateTimeFromString(dateInput1.text)
                            time2 = programSettings.dateTimeFromString(dateInput2.text)
                            loader.downloadData(time1, time2)
                            downloadMenu.close()
                        }
                    }
                }
            }
        }   // Menu
    }   // downloadMenuComponent
}
