import QtQuick 2.0
import QtQuick          2.3
import QtQuick.Dialogs  1.2
import QtLocation       5.3
import QtPositioning    5.3
import QtQuick.Layouts  1.2
import QtQuick.Window   2.2
import QtQuick.Controls 2.12
import QtQuick 2.12
import QtQuick.Controls 1.4

Popup {
    id: waitingAnimation
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    width:  parent.width/2.5
    height: parent.height/2.5
    background: qgcPal.windowShade

    BusyIndicator {
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)
        width:  parent.width/3
        height: parent.height/3
    }
}
