#include "styles.h"

Styles & Styles::instance()
{
    static Styles _instance;

    return _instance;
}

QPalette & Styles::cellType1Palette()
{
    return instance()._cellType1Palette;
}

QPalette & Styles::cellType2Palette()
{
    return instance()._cellType2Palette;
}

QPalette & Styles::cellSelectedPalette()
{
    return instance()._cellSelectedPalette;
}

QPalette & Styles::cellAlarmedPalette()
{
    return instance()._cellAlarmedPalette;
}

QPalette & Styles::cellAlarmedAndSelectedPalette()
{
    return instance()._cellAlarmedAndSelectedPalette;
}

QPalette & Styles::borderPalette()
{
    return instance()._borderPalette;
}

QPalette & Styles::connectionStatusDisconnected()
{
    return instance()._connectionStatusDisconnected;
}

QPalette & Styles::connectionStatusConnecting()
{
    return instance()._connectionStatusConnecting;
}

QPalette & Styles::connectionStatusConnected()
{
    return instance()._connectionStatusConnected;
}

QString & Styles::button()
{
    return instance()._button;
}

QString & Styles::areaButton()
{
    return instance()._areaButton;
}

QString & Styles::areaCheckedButton()
{
    return instance()._areaCheckedButton;
}

QString & Styles::spinBoxButton()
{
    return instance()._spinBoxButton;
}

Styles::Styles()
{
    QBrush white(QColor(255, 255, 255, 255)); // white
    white.setStyle(Qt::SolidPattern);

    QBrush lightGray(QColor(0xed, 0xea, 0xe8, 255)); // light gray
    lightGray.setStyle(Qt::SolidPattern);

    QBrush black(QColor(0, 0, 0, 255)); // black
    black.setStyle(Qt::SolidPattern);

    QBrush red(QColor(255, 0, 0, 255)); // red
    red.setStyle(Qt::SolidPattern);

    QBrush yellow(QColor(255, 255, 0, 255));
    yellow.setStyle(Qt::SolidPattern);

    QBrush green(QColor(0, 255, 0, 255));
    green.setStyle(Qt::SolidPattern);

    _cellType1Palette.setBrush(QPalette::Active, QPalette::Base, white);
    _cellType1Palette.setBrush(QPalette::Active, QPalette::Window, lightGray);
    _cellType1Palette.setBrush(QPalette::Active, QPalette::WindowText, black);
    _cellType1Palette.setBrush(QPalette::Inactive, QPalette::Base, white);
    _cellType1Palette.setBrush(QPalette::Inactive, QPalette::Window, lightGray);
    _cellType1Palette.setBrush(QPalette::Inactive, QPalette::WindowText, black);
    _cellType1Palette.setBrush(QPalette::Disabled, QPalette::Base, lightGray);
    _cellType1Palette.setBrush(QPalette::Disabled, QPalette::Window, lightGray);
    _cellType1Palette.setBrush(QPalette::Disabled, QPalette::WindowText, black);

    _cellType2Palette.setBrush(QPalette::Active, QPalette::Base, white);
    _cellType2Palette.setBrush(QPalette::Active, QPalette::Window, white);
    _cellType2Palette.setBrush(QPalette::Active, QPalette::WindowText, black);
    _cellType2Palette.setBrush(QPalette::Inactive, QPalette::Base, white);
    _cellType2Palette.setBrush(QPalette::Inactive, QPalette::Window, white);
    _cellType2Palette.setBrush(QPalette::Inactive, QPalette::WindowText, black);
    _cellType2Palette.setBrush(QPalette::Disabled, QPalette::Base, white);
    _cellType2Palette.setBrush(QPalette::Disabled, QPalette::Window, white);
    _cellType2Palette.setBrush(QPalette::Disabled, QPalette::WindowText, black);

    _cellSelectedPalette.setBrush(QPalette::Active, QPalette::Base, white);
    _cellSelectedPalette.setBrush(QPalette::Active, QPalette::Window, black);
    _cellSelectedPalette.setBrush(QPalette::Active, QPalette::WindowText, white);
    _cellSelectedPalette.setBrush(QPalette::Inactive, QPalette::Base, white);
    _cellSelectedPalette.setBrush(QPalette::Inactive, QPalette::Window, black);
    _cellSelectedPalette.setBrush(QPalette::Inactive, QPalette::WindowText, white);
    _cellSelectedPalette.setBrush(QPalette::Disabled, QPalette::Base, white);
    _cellSelectedPalette.setBrush(QPalette::Disabled, QPalette::Window, black);
    _cellSelectedPalette.setBrush(QPalette::Disabled, QPalette::WindowText, white);

    _cellAlarmedPalette.setBrush(QPalette::Active, QPalette::Base, white);
    _cellAlarmedPalette.setBrush(QPalette::Active, QPalette::Window, red);
    _cellAlarmedPalette.setBrush(QPalette::Active, QPalette::WindowText, white);
    _cellAlarmedPalette.setBrush(QPalette::Inactive, QPalette::Base, white);
    _cellAlarmedPalette.setBrush(QPalette::Inactive, QPalette::Window, red);
    _cellAlarmedPalette.setBrush(QPalette::Inactive, QPalette::WindowText, white);
    _cellAlarmedPalette.setBrush(QPalette::Disabled, QPalette::Base, white);
    _cellAlarmedPalette.setBrush(QPalette::Disabled, QPalette::Window, red);
    _cellAlarmedPalette.setBrush(QPalette::Disabled, QPalette::WindowText, white);

    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Active, QPalette::Base, red);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Active, QPalette::Window, black);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Active, QPalette::WindowText, red);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Inactive, QPalette::Base, red);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Inactive, QPalette::Window, black);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Inactive, QPalette::WindowText, red);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Disabled, QPalette::Base, red);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Disabled, QPalette::Window, black);
    _cellAlarmedAndSelectedPalette.setBrush(QPalette::Disabled, QPalette::WindowText, red);

    QBrush border(QColor(149, 147, 145, 255));
    border.setStyle(Qt::SolidPattern);

    _borderPalette.setBrush(QPalette::Active, QPalette::Window, border);
    _borderPalette.setBrush(QPalette::Inactive, QPalette::Window, border);
    _borderPalette.setBrush(QPalette::Disabled, QPalette::Base, border);
    _borderPalette.setBrush(QPalette::Disabled, QPalette::Window, border);

    _connectionStatusDisconnected.setBrush(QPalette::Active, QPalette::Text, red);
    _connectionStatusDisconnected.setBrush(QPalette::Inactive, QPalette::Text, red);
    _connectionStatusDisconnected.setBrush(QPalette::Disabled, QPalette::Text, red);
    _connectionStatusDisconnected.setBrush(QPalette::Active, QPalette::Base, red);
    _connectionStatusDisconnected.setBrush(QPalette::Inactive, QPalette::Base, red);
    _connectionStatusDisconnected.setBrush(QPalette::Disabled, QPalette::Base, red);

    _connectionStatusConnecting.setBrush(QPalette::Active, QPalette::Text, yellow);
    _connectionStatusConnecting.setBrush(QPalette::Inactive, QPalette::Text, yellow);
    _connectionStatusConnecting.setBrush(QPalette::Disabled, QPalette::Text, yellow);
    _connectionStatusConnecting.setBrush(QPalette::Active, QPalette::Base, yellow);
    _connectionStatusConnecting.setBrush(QPalette::Inactive, QPalette::Base, yellow);
    _connectionStatusConnecting.setBrush(QPalette::Disabled, QPalette::Base, yellow);

    _connectionStatusConnected.setBrush(QPalette::Active, QPalette::Text, green);
    _connectionStatusConnected.setBrush(QPalette::Inactive, QPalette::Text, green);
    _connectionStatusConnected.setBrush(QPalette::Disabled, QPalette::Text, green);
    _connectionStatusConnected.setBrush(QPalette::Active, QPalette::Base, green);
    _connectionStatusConnected.setBrush(QPalette::Inactive, QPalette::Base, green);
    _connectionStatusConnected.setBrush(QPalette::Disabled, QPalette::Base, green);

    _areaButton = "QPushButton {color: #000000; border-radius: 2px; border: 1px solid #bbbbbb; background-color: #edeae8;}"
        "QPushButton:pressed {color: #FFFFFF; border-radius: 2px; border: 1px solid #055cab; background-color: #055cab;}";

    _areaCheckedButton = "QPushButton {color: #000000; border-radius: 2px; border: 1px solid #bbbbbb; background-color: #edeae8;}"
        "QPushButton:checked {color: #ffffff; border-radius: 2px; border: 1px solid #959391; background-color: #ff3333;}";

    _button = "QPushButton {color: #000000; border-radius: 2px; border: 1px solid #bbbbbb; background-color: #edeae8;}"
        "QPushButton:pressed {color: #FFFFFF; border-radius: 2px; border: 1px solid #055cab; background-color: #055cab;}"
        "QPushButton:checked {color: #FFFFFF; border-radius: 2px; border: 1px solid #055cab; background-color: #055cab;}"
        "QPushButton:disabled {color: #bcbab8; border-radius: 2px; border: 1px solid #959391; background-color: #edeae8;}";

    _spinBoxButton = "QPushButton {color: #000000; border-radius: 2px; border: 1px solid #bbbbbb; background-color: #edeae8;}"
        "QPushButton:pressed {color: #FFFFFF; border-radius: 2px; border: 1px solid #055cab; background-color: #055cab;}"
        "QPushButton:checked {color: #FFFFFF; border-radius: 2px; border: 1px solid #055cab; background-color: #055cab;}"
        "QPushButton:disabled {color: #c0bebc; border-radius: 2px; border: 1px solid #959391; background-color: #edeae8;}";

}

