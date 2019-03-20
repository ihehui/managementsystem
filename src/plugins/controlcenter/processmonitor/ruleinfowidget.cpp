#include "ruleinfowidget.h"
#include "ui_ruleinfowidget.h"

#include <QFileDialog>
#include <QMessageBox>

#include "HHSharedCore/Cryptography"


namespace HEHUI
{


RuleInfoWidget::RuleInfoWidget(bool hashMode, bool readonly, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RuleInfoWidget)
{
    ui->setupUi(this);

    m_hashMode = hashMode;
    if(m_hashMode) {
        ui->labelRule->setText("MD5:");
    } else {
        ui->labelRule->setText(tr("Expression:"));
    }

    ui->lineEditRule->setReadOnly(readonly);
    ui->lineEditComment->setReadOnly(readonly);
    ui->toolButtonBrowse->setEnabled(!readonly);
    ui->comboBox->setEnabled(readonly);

}

RuleInfoWidget::~RuleInfoWidget()
{
    delete ui;
}

void RuleInfoWidget::setRuleInfo(const QString &ruleString, const QString &ruleComment, bool blacklistRule)
{
    ui->lineEditRule->setText(ruleString);
    ui->lineEditRule->setText(ruleComment);
    if(blacklistRule) {
        ui->comboBox->setCurrentIndex(2);
    } else {
        ui->comboBox->setCurrentIndex(1);
    }

    if(ruleString.isEmpty()) {
        ui->comboBox->setCurrentIndex(0);
    }

}

QString RuleInfoWidget::rule() const
{
    return ui->lineEditRule->text().trimmed();
}

QString RuleInfoWidget::comment() const
{
    return ui->lineEditComment->text();
}

bool RuleInfoWidget::blacklistRule()
{
    return (ui->comboBox->currentIndex() == 2);
}

void RuleInfoWidget::on_toolButtonBrowse_clicked()
{

    QFileDialog dialog(this);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setNameFilter(tr("EXE (*.exe);;All files (*.*)"));

    if(m_hashMode) {
        dialog.setFileMode(QFileDialog::AnyFile);
    } else {
        dialog.setFileMode(QFileDialog::Directory);
    }

    if(!dialog.exec()) {
        return;
    }

    QStringList files = dialog.selectedFiles();
    if(files.isEmpty()) {
        return;
    }

    QString fileName = files.at(0);
    if(m_hashMode) {
        QString errorString;
        QString md5 = HEHUI::Cryptography::getFileMD5HexString(fileName, &errorString);
        ui->lineEditRule->setText(md5);

    } else {
        ui->lineEditRule->setText(fileName);
    }
    ui->lineEditComment->setText(fileName);

}

void RuleInfoWidget::on_pushButtonOK_clicked()
{
    QString rule = ui->lineEditRule->text().trimmed();
    if(m_hashMode) {
        if(rule.size() != 32) {
            QMessageBox::critical(this, tr("Error"), tr("Invalid MD5!"));
            ui->lineEditRule->setFocus();
            return;
        }
    } else {
        if(rule.isEmpty()
                || rule.contains("\\\\")
                || rule.contains("//")
                || rule.startsWith("\\")
                || rule.startsWith("/")
                || rule.endsWith("\\")
                || rule.endsWith("/")
          ) {
            QMessageBox::critical(this, tr("Error"), tr("Invalid expression!"));
            ui->lineEditRule->setFocus();
            return;
        }

    }

    if(ui->comboBox->currentIndex() == 0) {
        QMessageBox::critical(this, tr("Error"), tr("Please select rule type!"));
        ui->comboBox->setFocus();
        return;
    }

    emit accepted();
}

void RuleInfoWidget::on_pushButtonCancel_clicked()
{
    emit rejected();
}





} //namespace HEHUI
