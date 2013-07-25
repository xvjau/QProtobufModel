import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Window 2.0

import br.com.bitforge 1.0

ApplicationWindow {
    title: qsTr("Protobuf Model Test")
    width: 800
    height: 600

    TableView {
        anchors.fill: parent

        model: ProtobufModel {
            metadata: "qrc:/pb/test.proto"
            message: "DateTest"
            source: "file:/tmp/test_data.pb"
        }
    }
}
