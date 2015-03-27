#ifndef SHUTDOWNDIALOG_H
#define SHUTDOWNDIALOG_H

#include <QDialog>

namespace Ui {
class ShutdownDialog;
}

class ShutdownDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShutdownDialog(QWidget *parent = 0);
    ~ShutdownDialog();

    QString message() const;
    quint64 timeout();
    bool forceAppsClosed();
    bool rebootAfterShutdown();


private:
    Ui::ShutdownDialog *ui;

};

#endif // SHUTDOWNDIALOG_H
