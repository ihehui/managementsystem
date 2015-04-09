#ifndef RULEINFOWIDGET_H
#define RULEINFOWIDGET_H

#include <QWidget>

namespace Ui {
class RuleInfoWidget;
}

namespace HEHUI {


class RuleInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RuleInfoWidget(bool hashMode = true, bool readonly = false, QWidget *parent = 0);
    ~RuleInfoWidget();

    void setRuleInfo(const QString &ruleString, const QString &ruleComment, bool blacklistRule);

    QString rule() const;
    QString comment() const;
    bool blacklistRule();

signals:
    void accepted();
    void rejected();


private slots:
    void on_toolButtonBrowse_clicked();
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();


private:
    Ui::RuleInfoWidget *ui;

    bool m_hashMode;


};

} //namespace HEHUI

#endif // RULEINFOWIDGET_H
