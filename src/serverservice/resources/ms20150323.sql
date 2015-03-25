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
-- Table structure for table `UsersLogin`
--

DROP TABLE IF EXISTS `UsersLogin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `UsersLogin` (
  `UserID` varchar(20) NOT NULL,
  `_Date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `PCName` varchar(20) DEFAULT NULL,
  `Remark` varchar(800) DEFAULT NULL,
  `IP` varchar(60) DEFAULT NULL
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
-- Table structure for table `hardware`
--

DROP TABLE IF EXISTS `hardware`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `hardware` (
  `ComputerName` varchar(32) NOT NULL DEFAULT '',
  `CPU` varchar(128) DEFAULT NULL,
  `Motherboard` varchar(255) DEFAULT NULL,
  `Memory` varchar(64) DEFAULT NULL,
  `Video` varchar(128) DEFAULT NULL,
  `Monitor` varchar(96) DEFAULT NULL,
  `Audio` varchar(128) DEFAULT NULL,
  `Storage` varchar(256) DEFAULT NULL,
  `NIC` varchar(512) DEFAULT NULL,
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ComputerName`) USING BTREE,
  CONSTRAINT `FK_detailedinfo_ComputerName_summaryinfo` FOREIGN KEY (`ComputerName`) REFERENCES `os` (`ComputerName`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `hardware`
--

LOCK TABLES `hardware` WRITE;
/*!40000 ALTER TABLE `hardware` DISABLE KEYS */;
/*!40000 ALTER TABLE `hardware` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `installedsoftware`
--

DROP TABLE IF EXISTS `installedsoftware`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `installedsoftware` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ComputerName` varchar(32) NOT NULL,
  `SoftwareName` varchar(255) NOT NULL,
  `SoftwareVersion` varchar(45) DEFAULT NULL,
  `InstallationDate` varchar(45) DEFAULT NULL,
  `Publisher` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_installedsoftware_ComputerName` (`ComputerName`),
  CONSTRAINT `FK_installedsoftware_ComputerName` FOREIGN KEY (`ComputerName`) REFERENCES `os` (`ComputerName`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `installedsoftware`
--

LOCK TABLES `installedsoftware` WRITE;
/*!40000 ALTER TABLE `installedsoftware` DISABLE KEYS */;
/*!40000 ALTER TABLE `installedsoftware` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `logs`
--

DROP TABLE IF EXISTS `logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `logs` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ComputerName` varchar(16) NOT NULL,
  `Users` varchar(255) DEFAULT NULL,
  `IPAddress` varchar(16) NOT NULL,
  `Type` tinyint(3) unsigned DEFAULT NULL,
  `Content` varchar(512) NOT NULL,
  `ClientTime` datetime DEFAULT NULL,
  `ServerTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `logs`
--

LOCK TABLES `logs` WRITE;
/*!40000 ALTER TABLE `logs` DISABLE KEYS */;
/*!40000 ALTER TABLE `logs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `os`
--

DROP TABLE IF EXISTS `os`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `os` (
  `ComputerName` varchar(32) NOT NULL,
  `OS` varchar(45) NOT NULL,
  `InstallationDate` timestamp NULL DEFAULT NULL,
  `OSKey` varchar(45) DEFAULT NULL,
  `Workgroup` varchar(32) NOT NULL,
  `JoinedToDomain` tinyint(3) unsigned DEFAULT '0',
  `Users` varchar(255) DEFAULT NULL,
  `Administrators` varchar(255) DEFAULT NULL,
  `IP` varchar(255) NOT NULL DEFAULT '',
  `ClientVersion` varchar(32) DEFAULT NULL,
  `LastOnlineTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ComputerName`) USING BTREE,
  UNIQUE KEY `Index_ComputerName` (`ComputerName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `os`
--

LOCK TABLES `os` WRITE;
/*!40000 ALTER TABLE `os` DISABLE KEYS */;
/*!40000 ALTER TABLE `os` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `software`
--

DROP TABLE IF EXISTS `software`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `software` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `InternalName` varchar(64) NOT NULL,
  `Author` varchar(45) DEFAULT NULL,
  `LegalCopyright` varchar(45) DEFAULT NULL,
  `Comments` varchar(64) DEFAULT NULL,
  `LatestInternalVersion` varchar(45) NOT NULL,
  `UpdatePath` varchar(255) NOT NULL,
  `Framework` varchar(45) NOT NULL,
  `ExpirationDate` date NOT NULL,
  `NewFileName` varchar(64) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `Index_2` (`InternalName`,`Framework`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `software`
--

LOCK TABLES `software` WRITE;
/*!40000 ALTER TABLE `software` DISABLE KEYS */;
/*!40000 ALTER TABLE `software` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `systemadministrators`
--

DROP TABLE IF EXISTS `systemadministrators`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `systemadministrators` (
  `UserID` varchar(24) NOT NULL,
  `UserName` varchar(24) NOT NULL,
  `BusinessAddress` varchar(16) NOT NULL,
  `PassWD` varchar(56) NOT NULL,
  `LastLoginTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `Remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `systemadministrators`
--

LOCK TABLES `systemadministrators` WRITE;
/*!40000 ALTER TABLE `systemadministrators` DISABLE KEYS */;
INSERT INTO `systemadministrators` VALUES ('admindg','','DG','c8HdVaYFOAXZ2oYjM4L6gqTIiFk=','0000-00-00 00:00:00',''),('adminhk','','HK','ImosDlUaBd8YyNUrX7pfHTuuI3A=','0000-00-00 00:00:00',''),('adminyd','','YD','MXuRPpxRE1xTPP/X4zO1bNkgy/0=','0000-00-00 00:00:00',''),('hehui','','DG','KlcsSsfmfp6B3ya+LliE2bHO2uc=','2015-03-12 10:17:01',''),('king','','DG','apJt8QFtRPAZaUawJkZZTVFEiOo=','0000-00-00 00:00:00',''),('lhc','','DG','es9NRQZZf2kbdGvMp/GrkV0+OV0=','0000-00-00 00:00:00',''),('lj','','DG','v+7twKPCWLNfk9pWrsDVB/CEQ1c=','0000-00-00 00:00:00',''),('ljf','','DG','RvhSIcEL1/AeICOvyeoCNMvXh3g=','0000-00-00 00:00:00',''),('zk','','DG','usJrQRUF6lLB0kUaoWJco3bFht8=','0000-00-00 00:00:00','');
/*!40000 ALTER TABLE `systemadministrators` ENABLE KEYS */;
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
-- Table structure for table `windowsadminpassword`
--

DROP TABLE IF EXISTS `windowsadminpassword`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `windowsadminpassword` (
  `Department` varchar(16) NOT NULL,
  `Password` varchar(45) NOT NULL DEFAULT 'trousetrouse',
  `OldPassword` varchar(45) NOT NULL DEFAULT 'computermisdg',
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Admin` varchar(45) NOT NULL DEFAULT 'HeHui',
  PRIMARY KEY (`Department`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `windowsadminpassword`
--

LOCK TABLES `windowsadminpassword` WRITE;
/*!40000 ALTER TABLE `windowsadminpassword` DISABLE KEYS */;
/*!40000 ALTER TABLE `windowsadminpassword` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping routines for database 'sitoycomputers'
--
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-03-23 19:10:06
