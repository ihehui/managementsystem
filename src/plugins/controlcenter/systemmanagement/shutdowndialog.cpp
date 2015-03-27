#include "shutdowndialog.h"
#include "ui_shutdowndialog.h"

ShutdownDialog::ShutdownDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShutdownDialog)
{
    ui->setupUi(this);
}

ShutdownDialog::~ShutdownDialog()
{
    delete ui;
}

QString ShutdownDialog::message() const{
    return ui->textEdit->toPlainText();
}

quint64 ShutdownDialog::timeout(){
    return ui->spinBoxTimeout->value();
}

bool ShutdownDialog::forceAppsClosed(){
    return ui->checkBoxForceAppsClosed->isChecked();
}

bool ShutdownDialog::rebootAfterShutdown(){
    return ui->checkBoxReboot->isChecked();
}
