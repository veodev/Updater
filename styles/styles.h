#ifndef STYLES_H
#define STYLES_H

#include <QBrush>
#include <QPalette>
#include <QString>

#define DEFAULT_STYLE_BUTTON      "color:white;border: 1px solid #000000;border-radius:6px;"
#define DEFAULT_BACKGROUND_BUTTON "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #8C8F8C, stop: 1 #000000);"
#define CHANGED_BACKGROUND_BUTTON "background:lightgray;color:darkred;"
#define CALIBRATION_SET_OK        "color: #ffffff; background-color: #00aa00;"
#define CALIBRATION_SET_FAILED    "color: #ffffff; background-color: #ff3333;"
#define HAND_SCAN_REG_ON          "color: #FFFFFF; border-radius: 2px; border: 1px solid #055cab; background-color: #055cab;"
#define HAND_SCAN_REG_OFF         "color: #000000; border-radius: 2px; border: 1px solid #bbbbbb; background-color: #edeae8;"
#define PAUSE_BUTTON_STATE_ON     "font-size: 10pt; color: #FFFFFF; border-radius: 2px; border: 1px solid #055cab; background-color: #055cab;"
#define PAUSE_BUTTON_STATE_OFF    "font-size: 10pt; color: #000000; border-radius: 2px; border: 1px solid #bbbbbb; background-color: #edeae8;"

#define MARKER_SYMBOLS_COLOR    QColor(0, 0, 0)
#define TEXT_MARKER_COLOR       QColor(25, 227, 49)
#define DEFECT_MARKER_COLOR     QColor(255, 0, 0)
#define COORDINAT_MARKER_COLOR  QColor(0, 0, 0)
#define BOLT_JOINT_MARKER_COLOR QColor(0, 136, 201)
#define RAIL_TYPE_MARKER_COLOR  QColor(0, 136, 201)
#define HAND_MARKER_COLOR       QColor(196, 145, 0)
#define AUTO_DEFECT_MARKER_COLOR QColor(0, 0x99, 0)

class Styles
{
public:
    static QPalette & cellType1Palette();
    static QPalette & cellType2Palette();
    static QPalette & cellSelectedPalette();
    static QPalette & cellAlarmedPalette();
    static QPalette & cellAlarmedAndSelectedPalette();
    static QPalette & borderPalette();
    static QPalette & connectionStatusDisconnected();
    static QPalette & connectionStatusConnecting();
    static QPalette & connectionStatusConnected();

    static QString & button();
    static QString & areaButton();
    static QString & areaCheckedButton();
    static QString & spinBoxButton();

protected:
    Styles();
    static Styles & instance();

private:
    QPalette _cellType1Palette;
    QPalette _cellType2Palette;
    QPalette _cellSelectedPalette;
    QPalette _cellAlarmedPalette;
    QPalette _cellAlarmedAndSelectedPalette;
    QPalette _borderPalette;
    QPalette _connectionStatusDisconnected;
    QPalette _connectionStatusConnecting;
    QPalette _connectionStatusConnected;

    QString _button;
    QString _areaButton;
    QString _areaCheckedButton;
    QString _spinBoxButton;
};

#endif // STYLES_H
