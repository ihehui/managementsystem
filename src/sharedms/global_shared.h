/*
 * global_shared.h
 *
 *  Created on: 2009-4-27
 *      Author: 贺辉
 */

#ifndef GLOBAL_SHARED_H_
#define GLOBAL_SHARED_H_




#ifndef APP_NAME
#define APP_NAME	"Service"
#endif

#ifndef APP_VERSION
#define APP_VERSION	"2011.2.25.1"
#endif

#ifndef APP_ICON_PATH
#define APP_ICON_PATH	"/images/app.png"
#endif

#ifndef APP_AUTHOR
#define APP_AUTHOR	"HeHui"
#endif

#ifndef APP_AUTHOR_EMAIL
#define APP_AUTHOR_EMAIL	"hehui@hehui.org"
#endif

#ifndef APP_ORG
#define APP_ORG	"HeHui"
#endif

#ifndef APP_ORG_DOMAIN
#define APP_ORG_DOMAIN	"www.hehui.org"
#endif

#ifndef APP_LICENSE
#define APP_LICENSE	"LGPL"
#endif


#ifndef UNIT_SEPARTOR
#define UNIT_SEPARTOR 0X1F
#endif

#ifndef IP_MULTICAST_GROUP_ADDRESS
#define IP_MULTICAST_GROUP_ADDRESS "239.239.239.1" //"239.255.255.255"
#endif

//#ifndef RUDP_LISTENING_PORT
//#define RUDP_LISTENING_PORT	12346
//#endif

#ifndef UDT_FILE_LISTENING_PORT
#define UDT_FILE_LISTENING_PORT	22347
#endif

#ifndef RTP_LISTENING_PORT
#define RTP_LISTENING_PORT	22346
#endif

#ifndef IP_MULTICAST_GROUP_PORT
#define IP_MULTICAST_GROUP_PORT 22345
#endif

#ifndef TCP_LISTENING_PORT
#define TCP_LISTENING_PORT	22344
#endif

#ifndef INVALID_SOCK_ID
#define INVALID_SOCK_ID	0
#endif

#ifndef WIN_ADMIN_PASSWORD
#define WIN_ADMIN_PASSWORD "trouse@trouse" //"computermisdg"
#endif

#ifndef CRYPTOGRAPHY_KEY
#define CRYPTOGRAPHY_KEY "HEHUI@2015"
#endif

#ifndef HEARTBEAT_TIMER_INTERVAL
#define HEARTBEAT_TIMER_INTERVAL 600000
#endif

#ifndef MIN_THREAD_COUNT
#define MIN_THREAD_COUNT 10
#endif

#ifndef MS_RUDP_KEEPALIVE_TIMER_INTERVAL
#define MS_RUDP_KEEPALIVE_TIMER_INTERVAL 30000 //1 minute
#endif

#ifndef FILE_PIECES_IN_ONE_REQUEST
#define FILE_PIECES_IN_ONE_REQUEST 20 //20 Pieces
#endif

#ifndef DOMAIN_NAME
#define DOMAIN_NAME "sitoy.group"
#endif
//#ifndef DOMAIN_ADMIN_NAME
//#define DOMAIN_ADMIN_NAME "dgadmin"
//#endif
//#ifndef DOMAIN_ADMIN_PASSWORD
//#define DOMAIN_ADMIN_PASSWORD "dmsto&*("
//#endif

#ifndef DEFAULT_MS_SERVER_HOST_NAME
#define DEFAULT_MS_SERVER_HOST_NAME "DGIT01234"
#endif



//#ifndef DB_CONNECTION_NAME
//#define DB_CONNECTION_NAME "SITOY_COMPUTERS_DB-ControlCenter"
//#endif

//MySQL
#ifndef REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME
#define REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME	"MANAGEMENTSYSTEM_DB"
#endif

#ifndef REMOTE_SITOY_COMPUTERS_DB_DRIVER
#define REMOTE_SITOY_COMPUTERS_DB_DRIVER	"QMYSQL"
#endif

#ifndef REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST
#define REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST	"200.200.200.40"
#endif

#ifndef REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT
#define REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT	3306
#endif

#ifndef REMOTE_SITOY_COMPUTERS_DB_USER_NAME
#define REMOTE_SITOY_COMPUTERS_DB_USER_NAME	"hehui"
#endif

#ifndef REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD
#define REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD	"hehui"
#endif

#ifndef REMOTE_SITOY_COMPUTERS_DB_NAME
#define REMOTE_SITOY_COMPUTERS_DB_NAME	"managementsystem"
#endif

//Sitoy SQL Server
#ifndef REMOTE_SITOY_SQLSERVER_DB_CONNECTION_NAME
#define REMOTE_SITOY_SQLSERVER_DB_CONNECTION_NAME	"200.200.200.2/MIS/AssetsInfo"
#endif

#ifndef REMOTE_SITOY_SQLSERVER_DB_DRIVER
#define REMOTE_SITOY_SQLSERVER_DB_DRIVER	"QODBC"
#endif

#ifndef REMOTE_SITOY_SQLSERVER_DB_NAME
#define REMOTE_SITOY_SQLSERVER_DB_NAME	"mis"
#endif

#ifndef REMOTE_SITOY_SQLSERVER_DB_HOST_NAME
#define REMOTE_SITOY_SQLSERVER_DB_HOST_NAME	"200.200.200.2"
#endif

#ifndef REMOTE_SITOY_SQLSERVER_DB_HOST_PORT
#define REMOTE_SITOY_SQLSERVER_DB_HOST_PORT	1433
#endif

#ifndef REMOTE_SITOY_SQLSERVER_DB_USER_NAME
#define REMOTE_SITOY_SQLSERVER_DB_USER_NAME	"appuser"
#endif

#ifndef REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD
#define REMOTE_SITOY_SQLSERVER_DB_USER_PASSWORD	"apppswd"
#endif




#include "HHSharedNetwork/hglobal_network.h"


namespace HEHUI {

    //quint8 MSPacketType = quint8(UserDefinedPacket + 1);

    namespace MS {

        enum Command{

            ClientLookForServer = UserDefinedPacket + 1, //6
            ServerDeclare,
            Update,

            ClientOnlineStatusChanged,
            ServerOnlineStatusChanged,
            ClientMessage,
            ServerMessage,

            AdminLogin,
            ServerResponseAdminLoginResult,
            AdminOnlineStatusChanged,

            ClientInfoRequested,
            ClientInfo,
            SystemInfoFromServer,

            UpdateSysAdminInfo,

            SystemAlarmsFromServer,
            RequestSystemAlarms,
            AcknowledgeSystemAlarms,

            Announcement,
            UpdateAnnouncement,
            ReplyMessage,
            AnnouncementFromServer,
            RequestAnnouncement,
            AnnouncementTargetsFromServer,
            RequestAnnouncementTargets,

            AdminRequestRemoteConsole,
            ClientResponseRemoteConsoleStatus,
            RemoteConsoleCMDFromAdmin,
            RemoteConsoleCMDResultFromClient,

            ClientRequestSoftwareVersion,
            ServerResponseSoftwareVersion,



            ClientLog,

            AdminRequestSetupUSBSD,
            ClientResponseUSBInfo,
            ShowAdmin,
            ModifyAdminGroupUser,
            AdminRequestConnectionToClient,
            ClientResponseAdminConnectionResult,
            
           
            AdminSearchClient,
            AdminRequestRemoteAssistance,
            UserResponseRemoteAssistance,
            
            UpdateMSWUserPassword,
            InformUserNewPassword,
            NewPasswordRetrevedByUser,
            
            LocalServiceServerDeclare,
            LocalUserOnlineStatusChanged,

            //File TX
            RequestFileSystemInfo,
            ResponseFileSystemInfo,
            RequestDownloadFile,
            RequestUploadFile,
            ResponseFileDownloadRequest,
            ResponseFileUploadRequest,
            RequestFileData,
            FileData,
            FileTXStatusChanged,
            FileTXError,

            ModifyAssetNO,
            AssetNOModified,
            RenameComputer,
            JoinOrUnjoinDomain,
            Remark,

            RequestTemperatures,
            ResponseTemperatures,

            RequestScreenshot,
            ResponseScreenshot,
            DesktopInfo,

            RequestShutdown,
            RequestLockWindows,

            RequestCreateOrModifyWinUser,
            RequestDeleteUser,

            RequestChangeServiceConfig,
            ServiceConfigChanged,

            RequestChangeProcessMonitorInfo,

            

        };

        enum FileTXStatus{
            File_TX_Preparing,
            File_TX_Receiving,
            File_TX_Sending,
            //File_TX_Transferring,
            File_TX_Progress,
            File_TX_Paused,
            File_TX_Aborted,
            File_TX_Done
        };

        enum FileType{FILE = 0, DRIVE, FOLDER };


        enum Log{
            LOG_Unknown = 0,
            LOG_Admin_Loggedin = 1,
            LOG_ClientUpdate = 2,

            LOG_Admin_ModifyAssetNO,
            LOG_Admin_RenamePC,

            LOG_UpdateMSUserPassword,
            LOG_CheckMSUsersAccount,

            LOG_AdminSetupUSBSD,
            LOG_AdminSetupProgrames,
            LOG_AdminSetupOSAdministrators,
            LOG_AdminRunCMD,

            LOG_AdminAddNewUserToOS,
            LOG_AdminQueryUserInfo,

            LOG_AdminRequestRemoteAssistance,
            LOG_AdminInformUserNewPassword,

            LOG_ServerAnnouncement,
            LOG_ServerTask
            
        };

        enum ClientMessageType{MSG_Information = 0, MSG_Warning, MSG_Critical};
        
        enum USBSTORStatus{
            USBSTOR_Disabled = 0,
            USBSTOR_ReadWrite = 1,
            USBSTOR_ReadOnly = 2,
            USBSTOR_Unknown = 3
        };

        enum SystemInfoType{
            SYSINFO_UNKNOWN = 0,
            SYSINFO_OS, //Client,Server
            SYSINFO_HARDWARE,//Client,Server
            SYSINFO_SOFTWARE,//Client,Server
            SYSINFO_SERVICES,//Client
            SYSINFO_OSUSERS,//Client
            SYSINFO_PROCESSMONITOR,//Client,Server
            SYSINFO_SERVER_INFO,//Server
            SYSINFO_SYSADMINS, //Server
            SYSINFO_SYSALARMS, //Server
            SYSINFO_SYSLOGS, //Server
            SYSINFO_REALTIME_INFO, //Client,Server
            SYSINFO_ANNOUNCEMENTS, //Server
            SYSINFO_ANNOUNCEMENTTARGETS //Server

        };

        enum AlarmType{
            ALARM_UNKNOWN = 0,
            ALARM_HARDWARECHANGE = 1,
            ALARM_PROCESSMONITOR = 2

        };

        enum AnnouncementType{
            ANNOUNCEMENT_UNKNOWN = 0,
            ANNOUNCEMENT_NORMAL = 1,
            ANNOUNCEMENT_CRITICAL = 2
        };

        enum AnnouncementTarget{
            ANNOUNCEMENT_TARGET_EVERYONE = 0,
            ANNOUNCEMENT_TARGET_SPECIFIC = 1,
            ANNOUNCEMENT_TARGET_ALL = 255
        };




    } // namespace MS



} // namespace HEHUI






#endif /* GLOBAL_SHARED_H_ */
