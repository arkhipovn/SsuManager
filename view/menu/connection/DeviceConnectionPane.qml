import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC
import QCC.Controls 2.0 as QCC2

import SSU 1.0
import Theme 1.0

import Controls 1.0 as Controls

import ConnectionSettings 1.0

import Menu 1.0

/*
    Основная форма. Которая загружает другие в зависимости от текущего статуса соединения
*/

Pane {
    id: deviceConnectionPane
    property var device: null
    padding: 0

    // загрузчик основной страницы
    ColumnLayout {
        width: parent.width; height: parent.height
        spacing: 0

        QCC2.WaitingBar {
            id: waitingBar
            Layout.fillWidth: true
            running: false
            duration: 1500
            progressColor: Material.accentColor
        }

        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.topMargin: Theme.indent
            Layout.leftMargin: Theme.indent; Layout.rightMargin: Theme.indent
            Layout.bottomMargin: Theme.smallIndent

            sourceComponent: {
                if(device == null)
                    return null

                switch(device.connectionSettings.status) {
                case ConnectionSettings.Unconnected:
                    waitingBar.stop()
                    return connectionComponent
                case ConnectionSettings.Connection:
                    waitingBar.start()
                    return connectionComponent
                case ConnectionSettings.ConnectEstablished:    // соединение установлено, ожидаем ответа
                    waitingBar.start()
                    return connectionComponent  //@N мб что-то другое?
                case ConnectionSettings.WaitingForAuthorization:
                case ConnectionSettings.WaitingForPassword:
                    waitingBar.stop()
                    return authorizationComponent
                case ConnectionSettings.Authorization:
                    waitingBar.start()
                    return authorizationComponent
                case ConnectionSettings.Disconnection:
                    waitingBar.start()
                    return sourceComponent//connectedComponent      // Загружаем туже форму, что и была..
                case ConnectionSettings.Connected:
                    waitingBar.stop()
                    return connectedComponent
                default: break
                }
            }
        }

        QCC.AttemptsSlider {
            id: attemptsSlider
            Layout.fillWidth: true
            Layout.preferredHeight: waitingBar.height
            visible: device == null ? null
                                    : value > 0 && (device.connectionSettings.status === ConnectionSettings.WaitingForAuthorization ||
                                                    device.connectionSettings.status === ConnectionSettings.WaitingForPassword ||
                                                    device.connectionSettings.status === ConnectionSettings.Authorization)
            value: device == null ? 0
                                  : device.connectionSettings.numberOfAuthorizationAttempts
            from: 0
            to: device == null ? 1
                               : device.options.kMaxNumberOfAuthorizationAttempts
        }
    }   // ColumnLayout


    Component {
        id: connectionComponent

        DeviceConnectionSettingsForm {
            device: deviceConnectionPane.device
            waiting: waitingBar.running

            onConnectToHost: {
                waitingBar.start()
                controller.connectToDevice(deviceConnectionPane.device, address, port)
            }
            onCanceled: controller.disconnectFromDevice(deviceConnectionPane.device)
        }
    }

    Component {
        id: authorizationComponent

        AuthorizationForm {
            device: deviceConnectionPane.device
            waiting: waitingBar.running

            onAuthorization: {
                waitingBar.start()
                controller.authorization(deviceConnectionPane.device, user, password, save)
            }
            onDisconnectFromHost: {
                waitingBar.start()
                controller.disconnectFromDevice(deviceConnectionPane.device)
            }
        }
    }

    Component {
        id: connectedComponent

        ConnectionParametersForm {
            device: deviceConnectionPane.device
            waiting: waitingBar.running

            onDisconnectFromHost: {
                waitingBar.start()
                controller.disconnectFromDevice(deviceConnectionPane.device)
            }
        }
    }
}
