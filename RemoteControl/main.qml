import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.3
import remoteclient 1.0

Window {
    visible: true
    width: 700
    height: 350
    color: "#bdb5b5"
    title: qsTr("RemoteControl-WiFi")

    // Клиент WiFi-управления робокаром
    RemoteClient
    {
        id : remote_client
    }

    // IP-адрес робокара
    TextEdit
    {
        id: textIP
        x: 4
        y: 8
        width: 148
        height: 28
        text: qsTr("192.168.43.100")
        font.pixelSize: 20
    }

    // Кнопка конекта к IP-адресу робокара
    Button
    {
        id: button
        x: 158
        y: 0
        width: 100
        height: 36
        text: qsTr("connect")

        onClicked:
        {
            if(remote_client.onConnect(textIP.text))
            {
                text  = "stop"
            }
            else
            {
                text  = "connect";
            }
        }
    }

    // Коффициент скорости поворота
    SpinBox
    {
        id: spinBox
        x: 300
        y: 2
        width: 123
        height: 34
        to: 10
        from: -10
        value: 1

        onValueChanged:
        {
            remote_client.coffTurn(value)
        }
    }

    // Кнока вкл\выкл светодиода
    Switch
    {
        id: elementLED
        x: 448
        y: 2
        text: qsTr("LED")

        onCheckedChanged:
        {
            remote_client.enabled_LED(checked)
        }
    }

    // Кнопка фиксации джойстика скорости
    Switch
    {
        id: elementFix
        x: 557
        y: 2
        text: qsTr("Fixation")

        onCheckedChanged:
        {
           sliderSpeed.value = 0;
           remote_client.speed_PWM(sliderSpeed.value)
        }
    }

    // Джойстик поворота робокара
    Dial
    {
        id: dial
        x: 11
        y: 88
        width: 221
        height: 220
        to: 1024
        from: -1024

        background:
        Rectangle
        {
            color: "#1791b9"
            radius: width / 2
            border.color: "#000000"
        }

        onMoved:
        {
            remote_client.speed_Turn(value)
        }

        onPressedChanged:
        {
            value=0;
            remote_client.speed_Turn(value)
        }

        Text
        {
            id: element
            x: 89
            y: 99
            text: qsTr("Turn")
            font.bold: true
            font.pixelSize: 20
        }
    }

    // Джойстик скорости робокара
    Slider
    {
        id: sliderSpeed
        x: 464
        y: 88
        width: 219
        height: 220
        to: 1024
        from: -1024
        orientation: Qt.Vertical
        value: 0.5

        background:
        Rectangle
        {
            x : 0
            y : 0
            height: width
            color: "#1791b9"
            radius: width / 2
            border.color: "#000000"
        }

        onMoved:
        {
            remote_client.speed_PWM(value)
        }

        onPressedChanged:
        {
            if(!elementFix.checked)
            {
                value = 0;
                remote_client.speed_PWM(value)
            }
        }

        Text {
            id: element1
            x: 18
            y: 99
            text: qsTr("Speed")
            font.bold: true
            font.pixelSize: 20
        }
    }

    // Выввод информации от робокара
    TextArea
    {
        id: textArea
        x: 238
        y: 54
        width: 220
        height: 139
        color: "#000000"
        text: remote_client.textInput
    }
}
