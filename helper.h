#ifndef HELPER_H
#define HELPER_H

#include <QWidget>

namespace Ui {
class Helper;
}

class Helper : public QWidget
{
    Q_OBJECT

public:
    explicit Helper(QWidget *parent = 0);
    ~Helper();

    void insertText(QString message);
    void reset();
    void setTitle(const QString title);

private slots:
    void on_closeButton_released();

private:
    Ui::Helper *ui;
};

#endif // HELPER_H
