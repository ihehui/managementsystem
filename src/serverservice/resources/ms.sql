-- MySQL dump 10.13  Distrib 5.5.15, for Win32 (x86)
--
-- Host: 200.200.200.40    Database: managementsystem
-- ------------------------------------------------------
-- Server version	5.5.28a-MariaDB-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `managementsystem`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `managementsystem` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `managementsystem`;

--
-- Table structure for table `Alarm`
--

DROP TABLE IF EXISTS `Alarm`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Alarm` (
  `ID` int(10) unsigned NOT NULL,
  `AssetNO` varchar(32) NOT NULL,
  `AlarmType` tinyint(1) unsigned NOT NULL,
  `Message` varchar(255) NOT NULL,
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Acknowledged` tinyint(1) unsigned DEFAULT '0',
  `AcknowledgedBy` varchar(32) DEFAULT NULL,
  `AcknowledgedTime` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Alarm`
--

LOCK TABLES `Alarm` WRITE;
/*!40000 ALTER TABLE `Alarm` DISABLE KEYS */;
/*!40000 ALTER TABLE `Alarm` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Hardware`
--

DROP TABLE IF EXISTS `Hardware`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Hardware` (
  `AssetNO` varchar(32) NOT NULL,
  `CPU` varchar(128) DEFAULT NULL,
  `Motherboard` varchar(255) DEFAULT NULL,
  `Memory` varchar(64) DEFAULT NULL,
  `Storage` varchar(256) DEFAULT NULL,
  `Monitor` varchar(96) DEFAULT NULL,
  `Video` varchar(128) DEFAULT NULL,
  `Audio` varchar(128) DEFAULT NULL,
  `NIC` varchar(512) DEFAULT NULL,
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`AssetNO`),
  UNIQUE KEY `AssetNO_UNIQUE` (`AssetNO`),
  KEY `FK_AssetNO` (`AssetNO`),
  CONSTRAINT `FK_Hardware_AssetNO` FOREIGN KEY (`AssetNO`) REFERENCES `OS` (`AssetNO`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Hardware`
--

LOCK TABLES `Hardware` WRITE;
/*!40000 ALTER TABLE `Hardware` DISABLE KEYS */;
/*!40000 ALTER TABLE `Hardware` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `InstalledSoftware`
--

DROP TABLE IF EXISTS `InstalledSoftware`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `InstalledSoftware` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `AssetNO` varchar(32) NOT NULL,
  `SoftwareName` varchar(255) NOT NULL,
  `SoftwareVersion` varchar(45) DEFAULT NULL,
  `InstallationDate` varchar(45) DEFAULT NULL,
  `Publisher` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `ID_UNIQUE` (`ID`),
  KEY `FK_InstalledSoftware_AssetNO` (`AssetNO`),
  CONSTRAINT `FK_InstalledSoftware_AssetNO` FOREIGN KEY (`AssetNO`) REFERENCES `OS` (`AssetNO`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `InstalledSoftware`
--

LOCK TABLES `InstalledSoftware` WRITE;
/*!40000 ALTER TABLE `InstalledSoftware` DISABLE KEYS */;
/*!40000 ALTER TABLE `InstalledSoftware` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Logs`
--

DROP TABLE IF EXISTS `Logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Logs` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `AssetNO` varchar(32) NOT NULL,
  `IPAddress` varchar(16) NOT NULL,
  `LogType` tinyint(1) unsigned DEFAULT NULL,
  `Content` varchar(512) NOT NULL,
  `ServerTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ClientTime` datetime DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `Fk_Logs_AssetNO` (`AssetNO`),
  CONSTRAINT `Fk_Logs_AssetNO` FOREIGN KEY (`AssetNO`) REFERENCES `OS` (`AssetNO`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Logs`
--

LOCK TABLES `Logs` WRITE;
/*!40000 ALTER TABLE `Logs` DISABLE KEYS */;
/*!40000 ALTER TABLE `Logs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `OS`
--

DROP TABLE IF EXISTS `OS`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `OS` (
  `AssetNO` varchar(32) NOT NULL,
  `ComputerName` varchar(32) NOT NULL,
  `OSVersion` varchar(45) NOT NULL,
  `InstallationDate` timestamp NULL DEFAULT NULL,
  `OSKey` varchar(45) DEFAULT NULL,
  `Workgroup` varchar(32) NOT NULL,
  `JoinedToDomain` tinyint(3) unsigned DEFAULT '0',
  `Users` varchar(255) DEFAULT NULL,
  `Administrators` varchar(255) DEFAULT NULL,
  `IP` varchar(255) NOT NULL DEFAULT '',
  `ClientVersion` varchar(32) DEFAULT NULL,
  `ProcessMonitorEnabled` tinyint(1) unsigned DEFAULT '0',
  `LastOnlineTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`AssetNO`) USING BTREE,
  UNIQUE KEY `Index_ComputerName` (`ComputerName`),
  UNIQUE KEY `AssetNO_UNIQUE` (`AssetNO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `OS`
--

LOCK TABLES `OS` WRITE;
/*!40000 ALTER TABLE `OS` DISABLE KEYS */;
/*!40000 ALTER TABLE `OS` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ProcessMonitorLogs`
--

DROP TABLE IF EXISTS `ProcessMonitorLogs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ProcessMonitorLogs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `AssetNO` varchar(64) NOT NULL,
  `IP` varchar(45) NOT NULL,
  `Block` tinyint(1) unsigned NOT NULL COMMENT 'Action: 0:Pass 1:Block',
  `AppPath` varchar(261) NOT NULL COMMENT 'EXE Path',
  `Rule` varchar(261) NOT NULL COMMENT 'Rule',
  `LogTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `FK_ProcessMonitorLogs_AssetNO` (`AssetNO`),
  CONSTRAINT `FK_ProcessMonitorLogs_AssetNO` FOREIGN KEY (`AssetNO`) REFERENCES `OS` (`AssetNO`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ProcessMonitorLogs`
--

LOCK TABLES `ProcessMonitorLogs` WRITE;
/*!40000 ALTER TABLE `ProcessMonitorLogs` DISABLE KEYS */;
/*!40000 ALTER TABLE `ProcessMonitorLogs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ProcessMonitorSettings`
--

DROP TABLE IF EXISTS `ProcessMonitorSettings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ProcessMonitorSettings` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `AssetNO` varchar(32) DEFAULT NULL,
  `ProcessMonitorEnabled` tinyint(1) unsigned DEFAULT '0',
  `Rules` text,
  `PassthroughEnabled` tinyint(1) unsigned DEFAULT '1',
  `LogAllowedProcess` tinyint(1) unsigned DEFAULT '0',
  `LogBlockedProcess` tinyint(1) unsigned DEFAULT '1',
  `UseGlobalRules` tinyint(1) DEFAULT '1',
  `RulesVersion` int(10) unsigned DEFAULT '0',
  `Admin` varchar(32) NOT NULL,
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`),
  KEY `FK_ProcessMonitorSettings_AssetNO` (`AssetNO`),
  KEY `FK_ProcessMonitorSettings_Admin` (`Admin`),
  CONSTRAINT `FK_ProcessMonitorSettings_Admin` FOREIGN KEY (`Admin`) REFERENCES `UsersLogin` (`UserID`) ON DELETE NO ACTION ON UPDATE CASCADE,
  CONSTRAINT `FK_ProcessMonitorSettings_AssetNO` FOREIGN KEY (`AssetNO`) REFERENCES `OS` (`AssetNO`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ProcessMonitorSettings`
--

LOCK TABLES `ProcessMonitorSettings` WRITE;
/*!40000 ALTER TABLE `ProcessMonitorSettings` DISABLE KEYS */;
/*!40000 ALTER TABLE `ProcessMonitorSettings` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SystemAdministrators`
--

DROP TABLE IF EXISTS `SystemAdministrators`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `SystemAdministrators` (
  `UserID` varchar(32) NOT NULL,
  `UserName` varchar(24) NOT NULL,
  `BusinessAddress` varchar(16) NOT NULL,
  `PassWD` varchar(56) NOT NULL,
  `LastLoginTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `Remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SystemAdministrators`
--

LOCK TABLES `SystemAdministrators` WRITE;
/*!40000 ALTER TABLE `SystemAdministrators` DISABLE KEYS */;
INSERT INTO `SystemAdministrators` VALUES ('admindg','','DG','c8HdVaYFOAXZ2oYjM4L6gqTIiFk=','0000-00-00 00:00:00',''),('adminhk','','HK','ImosDlUaBd8YyNUrX7pfHTuuI3A=','0000-00-00 00:00:00',''),('adminyd','','YD','MXuRPpxRE1xTPP/X4zO1bNkgy/0=','0000-00-00 00:00:00',''),('hehui','','DG','KlcsSsfmfp6B3ya+LliE2bHO2uc=','2015-03-12 10:17:01',''),('king','','DG','apJt8QFtRPAZaUawJkZZTVFEiOo=','0000-00-00 00:00:00',''),('lhc','','DG','es9NRQZZf2kbdGvMp/GrkV0+OV0=','0000-00-00 00:00:00',''),('lj','','DG','v+7twKPCWLNfk9pWrsDVB/CEQ1c=','0000-00-00 00:00:00',''),('ljf','','DG','RvhSIcEL1/AeICOvyeoCNMvXh3g=','0000-00-00 00:00:00',''),('zk','','DG','usJrQRUF6lLB0kUaoWJco3bFht8=','0000-00-00 00:00:00','');
/*!40000 ALTER TABLE `SystemAdministrators` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `UsersLogin`
--

DROP TABLE IF EXISTS `UsersLogin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `UsersLogin` (
  `UserID` varchar(32) NOT NULL,
  `LoginTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `AssetNO` varchar(32) NOT NULL,
  `IP` varchar(60) NOT NULL,
  `Remark` varchar(800) DEFAULT NULL,
  KEY `FK_UsersLogin_UserID` (`UserID`),
  CONSTRAINT `FK_UsersLogin_UserID` FOREIGN KEY (`UserID`) REFERENCES `SystemAdministrators` (`UserID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `UsersLogin`
--

LOCK TABLES `UsersLogin` WRITE;
/*!40000 ALTER TABLE `UsersLogin` DISABLE KEYS */;
/*!40000 ALTER TABLE `UsersLogin` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `versioninfo`
--

DROP TABLE IF EXISTS `versioninfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `versioninfo` (
  `LatestVersion` int(11) NOT NULL,
  `Remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`LatestVersion`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `versioninfo`
--

LOCK TABLES `versioninfo` WRITE;
/*!40000 ALTER TABLE `versioninfo` DISABLE KEYS */;
/*!40000 ALTER TABLE `versioninfo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping routines for database 'managementsystem'
--
/*!50003 DROP PROCEDURE IF EXISTS `sp_Logs_Save` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Logs_Save`(in p_AssetNO varchar(32), 
                                                                in p_IPAddress varchar(32), 
                                                                 in p_LogType tinyint, 
                                                                 in p_Content varchar(512), 
                                                                  in p_ServerTime TIMESTAMP ,
                                                                  in p_ClientTime TIMESTAMP 
                                                                  
                                                                )
BEGIN

###保存日志信息###

		insert into Logs(AssetNO, IPAddress,  LogType, Content,  ServerTime, ClientTime) values(p_AssetNO, p_IPAddress, p_LogType, p_Content,  p_ServerTime, p_ClientTime) ;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_OS_Query` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_OS_Query`(in p_AssetNO varchar(32), 
                                                                in p_ComputerName varchar(32), 
                                                                in p_OS varchar(45), 
                                                                 in p_Workgroup varchar(32), 
                                                                 in p_User varchar(255), 
                                                                 in p_IP varchar(255), 
                                                                 in p_ProcessMonitorEnabled tinyint 
                                                                  
                                                                )
BEGIN

###查询系统信息###

declare whereStatement varchar(1024) default ' ';


set whereStatement = CONCAT_WS('', whereStatement, ' AssetNO  like \'%', p_AssetNO, '%\'');


if CHAR_LENGTH(p_ComputerName) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and ComputerName  like \'%', p_ComputerName, '%\'');
end if;

if CHAR_LENGTH(p_OS) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and OSVersion like \'%', p_OS, '%\'');
end if;

if CHAR_LENGTH(p_Workgroup) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and Workgroup like \'%', p_Workgroup, '%\'');
end if;

if CHAR_LENGTH(p_User) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and Users like \'%', p_User, '%\'');
end if;

if CHAR_LENGTH(p_IP) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and IP like \'%', p_IP, '%\'');
end if;

if p_ProcessMonitorEnabled <> -1 then
set whereStatement = CONCAT_WS(' ', whereStatement, ' and ProcessMonitorEnabled=', p_ProcessMonitorEnabled);
end if;


set @fullStatement = CONCAT_WS(' ', ' select * from OS where  ', whereStatement );


SET @sqlQuery=@fullStatement;
PREPARE s1 FROM @sqlQuery;
EXECUTE s1;
DEALLOCATE PREPARE s1;


#select * from OS where 
#AssetNO like p_AssetNO 
#and ComputerName like p_ComputerName 
#and OS like p_OS
#and Workgroup like p_Workgroup
#and Users like p_User
#and IP like p_IP
#and ProcessMonitorEnabled = p_ProcessMonitorEnabled;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_OS_Update` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_OS_Update`(in p_AssetNO varchar(32), 
                                                                in p_ComputerName varchar(32), 
                                                                in p_OS varchar(45), 
                                                                in p_InstallationDate TIMESTAMP ,
                                                                 in p_OSKey varchar(45), 
                                                                 in p_Workgroup varchar(32), 
                                                                 in p_JoinedToDomain tinyint, 
                                                                 in p_Users varchar(255), 
                                                                 in p_Admins varchar(255), 
                                                                 in p_IP varchar(255), 
                                                                 in p_ClientVersion varchar(32)
                                                                  
                                                                  
                                                                )
BEGIN

###新增或更新系统信息###

	set @computerName= null;
	select ComputerName into @computerName from OS where AssetNO=p_AssetNO;
	if @computerName is null then
        insert into OS values(p_AssetNO, p_ComputerName, p_OS, p_InstallationDate,  p_OSKey, p_Workgroup, p_JoinedToDomain, p_Users, p_Admins, p_IP, p_ClientVersion, 0, NULL) ;
        insert into Hardware(AssetNO) values(p_AssetNO);
    else
        update OS set 
        ComputerName=p_ComputerName, OSVersion=p_OS, InstallationDate=p_InstallationDate,  OSKey=p_OSKey, Workgroup=p_Workgroup, JoinedToDomain=p_JoinedToDomain, Users=p_Users, Admins=p_Admins, IP=p_IP, ClientVersion=p_ClientVersion, LastOnlineTime=NULL;
	end if;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_ProcessMonitorSettings_Update` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_ProcessMonitorSettings_Update`(
                                                                in p_AssetNO varchar(32), 
                                                                in p_ProcessMonitorEnabled tinyint, 
                                                                in p_Rules text, 
                                                                in p_PassthroughEnabled tinyint,
                                                                in p_LogAllowedProcess tinyint,
                                                                in p_LogBlockedProcess tinyint,
                                                                in p_UseGlobalRules tinyint,
                                                                in p_Admin varchar(32)
                                                                  
                                                                )
BEGIN

###新增或更新系统信息###

	set @admin= null, @rulesVersion = 0;
	select RulesVersion, Admin into @rulesVersion, @admin from ProcessMonitorSettings where AssetNO=p_AssetNO;
	if @admin is null then
		insert into ProcessMonitorSettings values(NULL, p_AssetNO, p_ProcessMonitorEnabled, p_Rules, p_PassthroughEnabled,  p_LogAllowedProcess, p_LogBlockedProcess, p_UseGlobalRules, p_RulesVersion, p_Admin, NULL) ;
    else
        update ProcessMonitorSettings set 
        AssetNO=p_AssetNO, ProcessMonitorEnabled=p_ProcessMonitorEnabled, Rules=p_Rules,  PassthroughEnabled=p_PassthroughEnabled, LogAllowedProcess=p_LogAllowedProcess, LogBlockedProcess=p_LogBlockedProcess, UseGlobalRules=p_UseGlobalRules, RulesVersion=(@rulesVersion+1),  Admin=p_Admin, UpdateTime=NULL;
	end if;
    
    if p_AssetNO is not null then
        update OS set ProcessMonitorEnabled=p_ProcessMonitorEnabled where AssetNO=p_AssetNO;
    end if;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-04-13 10:06:32
