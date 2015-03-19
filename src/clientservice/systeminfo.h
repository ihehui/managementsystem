#ifndef SYSINFODLG_H
#define SYSINFODLG_H

#include <QThread>
#include <QProcess>

namespace HEHUI {


class SystemInfo: public QThread {
    Q_OBJECT

public:

    SystemInfo(QObject *parent = 0);
    ~SystemInfo();

    static bool isRunning();

protected:
    void run();

signals:
    void signalSystemInfoResultReady(const QByteArray &data);

public slots:
    void getSystemInfo();
    void getInstalledSoftwareInfo();

private:
    void getInstalledSoftwareInfo(QJsonArray *infoArray, const QStringList &keys, bool on64BitView);


private:

    static bool running;


};

} //namespace HEHUI

#endif // SYSINFODLG_H

