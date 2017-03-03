#include "processmonitor.h"

#include <QCoreApplication>

#include <QtConcurrent>

#ifdef Q_OS_WIN
    // Windows Header Files:
    #include <windows.h>
    // C RunTime Header Files
    #include <tchar.h>
#endif

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>



#include "HHSharedCore/hcryptography.h"


#pragma comment(lib,"Advapi32")


namespace HEHUI
{

const WCHAR serviceName[] = L"ProcessMonitor";
const WCHAR deviceName[] = L"\\\\.\\HHPROCMON";


ProcessMonitor::ProcessMonitor(QObject *parent) : QObject(parent)
{

    m_errorCode = ERROR_SUCCESS;

    m_useGlobalRules = true;
    m_passthroughEnabled = true;
    m_logAllowedProcessEnabled = false;
    m_logBlockedProcessEnabled = true;

}

ProcessMonitor::~ProcessMonitor()
{
    cleanupDriver();

    cleanRules(true);
    cleanRules(false);
}

unsigned long ProcessMonitor::lastErrorCode() const
{
    return m_errorCode;
}

bool ProcessMonitor::init()
{
    qDebug() << "--ProcessMonitor::init()";

    m_errorCode = ERROR_SUCCESS;

    if(!setupDriver()) {
        return false;
    }

    DWORD controlbuff[64];
    DWORD dw;


    //CreateThread(0,0,(LPTHREAD_START_ROUTINE)monitor,0,0,&dw);
    QThreadPool *pool = QThreadPool::globalInstance();
    int maxThreadCount = pool->maxThreadCount();
    if(pool->activeThreadCount() == pool->maxThreadCount()) {
        pool->setMaxThreadCount(++maxThreadCount);
    }
    QtConcurrent::run(this, &ProcessMonitor::monitor);

    //Open Device
    HANDLE device = CreateFileW(deviceName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
    if( INVALID_HANDLE_VALUE == device) {
        m_errorCode = GetLastError();
        qCritical() << QString("ERROR! CreateFileW failed. Error Code: %1").arg(m_errorCode);

        cleanupDriver();
        return false;
    }

    //Get NtCreateSection address, 将它传给驱动, 也将缓冲区的地址传给驱动
    DWORD *addr = (DWORD *)(1 + (DWORD)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtCreateSection"));
    ZeroMemory(outputbuff, 256);
    controlbuff[0] = addr[0];
    controlbuff[1] = (DWORD)&outputbuff[0];
    bool ok = DeviceIoControl(device, 1000, controlbuff, 256, controlbuff, 256, &dw, 0);
    if(!ok) {
        m_errorCode = GetLastError();
        qCritical() << QString("ERROR! DeviceIoControl failed. Error Code: %1").arg(m_errorCode);

        cleanupDriver();
    }

    return ok;

}

void ProcessMonitor::setRulesData(const QByteArray &jsonData)
{

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()) {
        return;
    }

    QJsonArray array = object["Rules"].toArray();

    for(int i = 0; i < array.size(); i++) {
        QJsonArray infoArray = array.at(i).toArray();
        if(infoArray.size() != 5) {
            qCritical() << "ERROR! Invalid JSON array.";
            continue;
        }

        ProcessMonitorRule *rule = new ProcessMonitorRule();
        int index = 0;
        rule->ruleString = infoArray.at(index++).toString();
        rule->comment = infoArray.at(index++).toString();
        rule->hashRule = infoArray.at(index++).toString().toUInt();
        rule->blacklistRule = infoArray.at(index++).toString().toUInt();
        rule->globalRule = infoArray.at(index++).toString().toUInt();

        QString key = rule->ruleString;
        if(rule->hashRule) {
            hashRulesHash.insert(key, rule);
            if(rule->blacklistRule) {
                md5BlackList.append(key);
            } else {
                md5WhiteList.append(key);
            }
        } else {
            key = key.replace("\\", "\\\\");
            key = key.replace("*", "\\S+");
            pathRulesHash.insert(key, rule);
            if(rule->blacklistRule) {
                filePathBlackList.append(key);
            } else {
                filePathWhiteList.append(key);
            }
        }

    }

}

QJsonArray ProcessMonitor::getLocalRules()
{

    QJsonArray array;

    foreach (ProcessMonitorRule *rule, hashRulesHash) {
        if(rule->globalRule) {
            continue;
        }
        QJsonArray infoArray;
        infoArray.append(rule->ruleString);
        infoArray.append(rule->comment);
        infoArray.append(QString::number(rule->hashRule));
        infoArray.append(QString::number(rule->blacklistRule));

        array.append(infoArray);
    }

    foreach (ProcessMonitorRule *rule, pathRulesHash) {
        if(rule->globalRule) {
            continue;
        }
        QJsonArray infoArray;
        infoArray.append(rule->ruleString);
        infoArray.append(rule->comment);
        infoArray.append(QString::number(rule->hashRule));
        infoArray.append(QString::number(rule->blacklistRule));

        array.append(infoArray);
    }

    return array;

}

void ProcessMonitor::setBasicInfo(bool useGlobalRules, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess)
{
    m_useGlobalRules = useGlobalRules;
    m_passthroughEnabled = enablePassthrough;
    m_logAllowedProcessEnabled = enableLogAllowedProcess;
    m_logBlockedProcessEnabled = enableLogBlockedProcess;
}

void ProcessMonitor::cleanRules(bool globalRules)
{

    foreach (ProcessMonitorRule *rule, hashRulesHash.values()) {
        if(rule->globalRule == globalRules) {
            QString key = hashRulesHash.key(rule);
            Q_ASSERT(key == rule->ruleString);
            md5WhiteList.removeAll(key);
            md5BlackList.removeAll(key);
            hashRulesHash.remove(key);

            delete rule;
            rule = 0;
        }
    }

    foreach (ProcessMonitorRule *rule, pathRulesHash.values()) {
        if(rule->globalRule == globalRules) {
            QString key = pathRulesHash.key(rule);
            filePathWhiteList.removeAll(key);
            filePathBlackList.removeAll(key);
            hashRulesHash.remove(key);

            delete rule;
            rule = 0;
        }
    }


}

void ProcessMonitor::monitor()
{

    qDebug() << "--ProcessMonitor::monitor()";


    DWORD a, x;
    wchar_t msgbuff[512];

    while(1) {
        memmove(&a, &outputbuff[0], 4);

        //如果缓冲区为空，则休眠10ms，继续检查
        if(!a) {
            Sleep(10);
            continue;
        }

        // 如果文件的名字和路径在机器的运行进程列表中，则发送一个OK的回应
        char *name = (char *)&outputbuff[8];
        qDebug() << "name:" << name;
        QString appPath = QString::fromLocal8Bit(name);
        qDebug() << "----appPath:" << appPath;

        QString errorString;
        QString md5 = HEHUI::Cryptography::getFileMD5HexString(appPath, &errorString);
        qDebug() << QString("MD5: %1 \n%2").arg(md5).arg(errorString);
        if(md5.isEmpty() && m_passthroughEnabled) {
            a = 1;
            goto skip;
        }

        if(md5WhiteList.contains(md5, Qt::CaseInsensitive)) {
            a = 1;
            goto skip;
        }

        if(!md5BlackList.contains(md5, Qt::CaseInsensitive)) {
            a = 1;
            goto skip;
        }


//        msg = QString("Do you want to run '%1'?").arg(appPath);
//        wcscpy(msgbuff, msg.toStdWString().c_str());

//        // 如果用户同意，则添加该程序到信任列表里
//        if(IDYES==MessageBoxW(0, msgbuff,L"WARNING",MB_YESNO|MB_ICONQUESTION|0x00200000L))
//        {
//            a=1;
//            md5WhiteList.append(appPath);
//        }else{
        a = 0;
//        }


        qDebug() << "-------2-------";
        // 把用户的选择写进通信缓冲区，驱动将接收
skip:
        memmove(&outputbuff[4], &a, 4);
        qDebug() << "-------3-------";

        //通知驱动继续进行运行
        a = 0;
        memmove(&outputbuff[0], &a, 4);

        qDebug() << "-------4-------";
    }

}


bool ProcessMonitor::setupDriver()
{

    QString drvPath = QCoreApplication::applicationDirPath() + "\\ProcessMonitor.sys";
    if(!QFileInfo::exists(drvPath)) {
        m_errorCode = ERROR_FILE_NOT_FOUND;
        qDebug() << QString("ERROR! Driver '%1' not found.").arg(drvPath);
        return false;
    }

    //Load Driver
    SC_HANDLE schSCManager = OpenSCManagerW(0, 0, SC_MANAGER_ALL_ACCESS);
    if (NULL == schSCManager) {
        m_errorCode = GetLastError();
        qCritical() << QString("ERROR! OpenSCManagerW failed. Error Code: %1").arg(m_errorCode);
        return false;
    }

    SC_HANDLE schService = OpenServiceW(schSCManager, serviceName, SERVICE_ALL_ACCESS);
    if (schService == NULL) {
        schService = CreateServiceW(schSCManager, serviceName, serviceName, SERVICE_START | SERVICE_STOP, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, drvPath.toStdWString().c_str(), 0, 0, 0, 0, 0);
    }

    if (schService == NULL) {
        m_errorCode = GetLastError();
        qCritical() << QString("ERROR! OpenServiceW failed. Error Code: %1").arg(m_errorCode);
        CloseServiceHandle(schSCManager);
        return false;
    }

    bool ok = StartServiceW(schService, 0, 0);
    if(!ok) {
        m_errorCode = GetLastError();
        qCritical() << QString("ERROR! StartServiceW failed. Error Code: %1").arg(m_errorCode);

        if(ERROR_SERVICE_ALREADY_RUNNING == m_errorCode) {
            ok = true;
        } else {
            DeleteService(schService);
        }

    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return  ok;

}

void ProcessMonitor::cleanupDriver()
{
    SC_HANDLE schSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    SERVICE_STATUS stat;
    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);
    ControlService(schService, SERVICE_CONTROL_STOP, &stat);
    DeleteService(schService);

}








} //namespace HEHUI

