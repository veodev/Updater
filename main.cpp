#include "widget.h"

#include "styles/styles.h"
#include <QApplication>
#include <QTranslator>

void setStyleSheets()
{
    qApp->setStyleSheet(Styles::button());
    qApp->setStyleSheet(qApp->styleSheet() + "QListWidget {border-radius: 2px; border: 1px solid #959391; background-color: #ffffff;}");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load(":/russian.qm", ".");
    a.installTranslator(&translator);
    setStyleSheets();
    QApplication::setStyle("fusion");
    Widget w;
    w.show();

    return a.exec();
}
