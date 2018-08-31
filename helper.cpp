#include "helper.h"
#include "ui_helper.h"

#include <QScrollBar>

Helper::Helper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Helper)
{
    ui->setupUi(this);
    ui->plainTextEdit->verticalScrollBar()->setStyleSheet("width: 30px;");
}

Helper::~Helper()
{
    delete ui;
}

void Helper::insertText(QString message)
{
    ui->plainTextEdit->insertPlainText(message);
}

void Helper::reset()
{
    ui->plainTextEdit->clear();
}

void Helper::setTitle(const QString title)
{
    ui->label->setText(title);
}

void Helper::on_closeButton_released()
{
    this->hide();
}
