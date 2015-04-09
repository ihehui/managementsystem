#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QObject>


namespace HEHUI {

typedef unsigned int SOCKETID;

class SystemInfo: public QObject {
    Q_OBJECT

public:

    SystemInfo(QObject *parent = 0);
    ~SystemInfo();

    static bool isRunning();



protected:
//    void run();

signals:
    void signalSystemInfoResultReady(const QByteArray &data, quint8 infoType, SOCKETID socketID);

public slots:
//    void getSystemInfo();
    static QByteArray getOSInfo();
    void getHardwareInfo(SOCKETID socketID);

    void getInstalledSoftwaresInfo(SOCKETID socketID);
    void getServicesInfo(SOCKETID socketID);

    void getUsersInfo(SOCKETID socketID);


private:
    void getInstalledSoftwaresInfo(QJsonArray *infoArray, const QStringList &keys, bool on64BitView);


private:

    static bool running;


};

} //namespace HEHUI

#endif // SYSTEMINFO_H

