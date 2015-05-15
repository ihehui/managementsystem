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
-- Table structure for table `Alarms`
--

DROP TABLE IF EXISTS `Alarms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Alarms` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `AssetNO` varchar(32) NOT NULL,
  `AlarmType` tinyint(1) unsigned NOT NULL COMMENT 'ALARM_UNKNOWN=0,\nALARM_HARDWARECHANGE=1,\nALARM_PROCESSMONITOR=2',
  `Message` varchar(255) NOT NULL,
  `UpdateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Acknowledged` tinyint(1) unsigned DEFAULT '0',
  `AcknowledgedBy` varchar(32) DEFAULT NULL,
  `AcknowledgedTime` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Alarms`
--

LOCK TABLES `Alarms` WRITE;
/*!40000 ALTER TABLE `Alarms` DISABLE KEYS */;
INSERT INTO `Alarms` VALUES (1,'china1111',1,'A1','2015-04-22 18:34:48',0,'',NULL),(2,'china1111',2,'A2','2015-04-22 15:35:22',0,'',NULL),(3,'china1111',2,'A3','2011-04-21 15:35:44',0,'',NULL),(4,'china1111',1,'A3','2014-04-23 15:36:00',1,' hehui ','2015-04-28 09:50:11'),(5,'dgpc02419',1,'A5','2015-03-23 15:36:15',1,' hehui ','2015-04-28 09:44:48'),(6,'dgpc02419',1,'A7','2015-04-19 18:36:35',1,'guest','2015-04-23 07:35:22'),(7,'dgpc02419',2,'A7','2015-04-23 05:36:40',1,' hehui ','2015-04-28 09:44:48');
/*!40000 ALTER TABLE `Alarms` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `AnnouncementReplyies`
--

DROP TABLE IF EXISTS `AnnouncementReplyies`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `AnnouncementReplyies` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Announcement` int(10) unsigned NOT NULL,
  `Sender` varchar(32) NOT NULL,
  `SendersAssetNO` varchar(32) DEFAULT NULL,
  `Receiver` varchar(32) DEFAULT NULL,
  `ReceiversAssetNO` varchar(32) DEFAULT NULL,
  `Message` text NOT NULL,
  `PublishTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`),
  KEY `Announcement` (`Announcement`),
  CONSTRAINT `FK_AnnouncementReplys_Announcement` FOREIGN KEY (`Announcement`) REFERENCES `Announcements` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `AnnouncementReplyies`
--

LOCK TABLES `AnnouncementReplyies` WRITE;
/*!40000 ALTER TABLE `AnnouncementReplyies` DISABLE KEYS */;
/*!40000 ALTER TABLE `AnnouncementReplyies` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `AnnouncementTargets`
--

DROP TABLE IF EXISTS `AnnouncementTargets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `AnnouncementTargets` (
  `ID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `Announcement` int(10) unsigned NOT NULL,
  `AssetNO` varchar(32) DEFAULT NULL,
  `UserName` varchar(32) DEFAULT NULL,
  `Acknowledged` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `ACKTime` datetime DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_AnnouncementTargets_Announcement` (`Announcement`),
  KEY `FK_AnnouncementTargets_AssetNO` (`AssetNO`),
  CONSTRAINT `FK_AnnouncementTargets_Announcement` FOREIGN KEY (`Announcement`) REFERENCES `Announcements` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `FK_AnnouncementTargets_AssetNO` FOREIGN KEY (`AssetNO`) REFERENCES `OS` (`AssetNO`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `AnnouncementTargets`
--

LOCK TABLES `AnnouncementTargets` WRITE;
/*!40000 ALTER TABLE `AnnouncementTargets` DISABLE KEYS */;
INSERT INTO `AnnouncementTargets` VALUES (1,3,'china1111','',0,NULL),(2,5,'china1111','User1',0,NULL),(3,4,'china1111','User3',0,NULL),(4,3,'PC1',NULL,0,NULL),(5,1,'china1111',NULL,0,NULL),(6,2,'china1111','User2',0,NULL),(7,12,'china1111','',0,NULL),(8,14,'PC1','',0,NULL),(9,14,'PC2','',0,NULL),(10,14,'PC3','User3',0,NULL);
/*!40000 ALTER TABLE `AnnouncementTargets` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Announcements`
--

DROP TABLE IF EXISTS `Announcements`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Announcements` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `AnnouncementType` tinyint(1) unsigned NOT NULL COMMENT '1:Normal Message\n2:Critical Message',
  `Content` text NOT NULL,
  `ACKRequired` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `Admin` varchar(32) NOT NULL,
  `PublishDate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ValidityPeriod` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'How long the bulletin will expires, in minutes\n0:Never Expires',
  `TargetType` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '0:Everyone\n1:Specific',
  `DisplayTimes` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '0:Always Display',
  `Active` tinyint(1) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`),
  KEY `FK_Announcements_Admin` (`Admin`),
  CONSTRAINT `FK_Announcements_Admin` FOREIGN KEY (`Admin`) REFERENCES `SystemAdministrators` (`UserID`) ON DELETE NO ACTION ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=27 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Announcements`
--

LOCK TABLES `Announcements` WRITE;
/*!40000 ALTER TABLE `Announcements` DISABLE KEYS */;
INSERT INTO `Announcements` VALUES (1,1,'Test1',1,'hehui','2015-05-13 06:19:00',900,1,10,1),(2,2,'Test2',0,'hehui','2015-05-03 06:19:00',0,1,10,1),(3,2,'Test3',1,'hehui','2015-05-13 06:19:00',600,2,100,0),(4,1,'QQQQQQQQQQQQQQQQQQQ',1,'hehui','2015-05-06 11:18:13',30,1,1,1),(5,1,'QQQQQQQQQQQQQ',1,'hehui','2015-05-13 11:29:57',300000,1,1,1),(6,1,'QQQQQQQQQQQ',1,'hehui','2015-05-06 11:34:56',30,1,1,1),(7,1,'EEEEEEEEEE',1,'hehui','2015-05-06 11:38:24',30,1,1,1),(8,1,'EEEEEEEEEE',1,'hehui','2015-05-06 12:05:27',30,1,1,1),(9,1,'TTTTBBBB',0,'hehui','2015-05-13 00:34:14',600,0,2,1),(10,1,'2222222',1,'hehui','2015-05-12 08:24:53',30,1,1,1),(11,1,'2222222222222',1,'hehui','2015-05-12 08:56:24',30,1,1,1),(12,1,'44444444444444',1,'hehui','2015-05-13 06:58:01',3000,1,1,1),(13,1,'sdddd',1,'hehui','2015-05-13 07:12:06',30,1,1,1),(14,1,'555555555555',1,'hehui','2015-05-13 07:22:00',30,1,1,1),(15,1,'1111111',1,'hehui','2015-05-14 09:32:31',30,0,1,1),(16,1,'2222222222222222',1,'hehui','2015-05-14 09:36:44',30,0,1,1),(17,1,'222222222222',1,'hehui','2015-05-14 09:41:06',3000,0,1,1),(18,1,'33333333333333333',1,'hehui','2015-05-14 09:49:31',30,0,1,1),(19,1,'4444444444444',1,'hehui','2015-05-14 09:54:00',30,0,1,1),(20,1,'4444444444444444',1,'hehui','2015-05-14 09:55:48',30,0,1,1),(21,1,'11111111111111111',1,'hehui','2015-05-14 10:03:38',30,0,1,1),(22,1,'222222222222222222222',1,'hehui','2015-05-14 10:06:06',30,0,1,1),(23,1,'QQQQQQQQQQQQQQQQQQQ',1,'hehui','2015-05-14 11:12:29',30,0,1,1),(24,1,'555555555555555555',1,'hehui','2015-05-15 06:21:48',30,0,1,1),(25,1,'44444444444444',1,'hehui','2015-05-15 07:24:28',30,0,1,1),(26,1,'fffffffff',1,'hehui','2015-05-15 07:27:25',30,0,1,1);
/*!40000 ALTER TABLE `Announcements` ENABLE KEYS */;
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
INSERT INTO `Hardware` VALUES ('china1111','Intel Pentium III Xeon 处理器 CPU 0',NULL,'(1GB,)','QEMU HARDDISK(29GB)',NULL,NULL,NULL,'Realtek RTL8139 Family PCI Fast Ethernet NIC(52:54:00:6D:43:EA)','2015-05-15 07:38:11',NULL),('dgpc02419','Intel(R) Core(TM) i3-3220 CPU @ 3.30GHz LGA1155','ASUSTeK COMPUTER INC. P8H61-M LX3 PLUS R2.0','Kingston(4GB,1333)','ST3500413AS ATA Device(465GB);ST500DM002-1BC142 ATA Device(465GB)','DELF03D','GeForce 9400 GT','VIA High Definition Audio','Realtek PCIe GBE Family Controller(08:60:6E:C4:36:29);VirtualBox Host-Only Ethernet Adapter(08:00:27:00:C0:CB);VirtualBox Bridged Networking Driver Miniport(08:60:6E:C4:36:29)','2015-04-17 15:42:45',NULL);
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
) ENGINE=InnoDB AUTO_INCREMENT=2111 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `InstalledSoftware`
--

LOCK TABLES `InstalledSoftware` WRITE;
/*!40000 ALTER TABLE `InstalledSoftware` DISABLE KEYS */;
INSERT INTO `InstalledSoftware` VALUES (1840,'dgpc02419','Adobe Flash Player 17 ActiveX','17.0.0.169','','Adobe Systems Incorporated'),(1841,'dgpc02419','Adobe Flash Player 17 NPAPI','17.0.0.169','','Adobe Systems Incorporated'),(1842,'dgpc02419','支付宝安全控件 5.3.0.3807','5.3.0.3807','','Alipay.com Co., Ltd.'),(1843,'dgpc02419','百度安全控件 1.0.3.0','','',''),(1844,'dgpc02419','招行专业版','','',''),(1845,'dgpc02419','Colasoft Capsa 7 Enterprise','7.3','20150202','Colasoft'),(1846,'dgpc02419','Git version 1.9.5-preview20150319','1.9.5-preview20150319','20150416','The Git Development Community'),(1847,'dgpc02419','GNS3 1.2','1.2','',''),(1848,'dgpc02419','VIA 平台设备管理员','1.39','20140729','VIA Technologies, Inc.'),(1849,'dgpc02419','金山毒霸','2015.1.2','','Kingsoft Internet Security'),(1850,'dgpc02419','Microsoft Windows Driver Kit 7.1.0.7600','7.1.0.7600','','Microsoft Corporation'),(1851,'dgpc02419','快盘','4.12.16.17','','ShenZhen Xunlei Networking Technologies Ltd.'),(1852,'dgpc02419','猎豹安全浏览器','5.2.91.9718','','猎豹工作室'),(1853,'dgpc02419','Microsoft Document Explorer 2008','','','Microsoft Corporation'),(1854,'dgpc02419','向日葵 7.3 正式版','7.3.38.998','','上海贝锐信息科技有限公司'),(1855,'dgpc02419','TeamViewer 10','10.0.40798','','TeamViewer'),(1856,'dgpc02419','VisualDDK','1.0','','Bazis'),(1857,'dgpc02419','豌豆荚','2.80.0.7144','','豌豆实验室'),(1858,'dgpc02419','WinPcap 4.1.2','4.1.0.2001','','CACE Technologies'),(1859,'dgpc02419','Wireshark 1.12.1 (64-bit)','1.12.1','','The Wireshark developer community, http://www.wireshark.org'),(1860,'dgpc02419','Microsoft Visual C++ 2008 ATL Update kb973924 - x86 9.0.30729.4148','9.0.30729.4148','20140919','Microsoft Corporation'),(1861,'dgpc02419','sensorsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1862,'dgpc02419','vistalibs_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1863,'dgpc02419','wdftools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1864,'dgpc02419','VMware vSphere Client 5.1','5.1.0.1557','20140804','VMware, Inc.'),(1865,'dgpc02419','networklibraries_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1866,'dgpc02419','toastermetadatapackagesample','1.1.6001.0','20150404','Microsoft Corporation'),(1867,'dgpc02419','infsample_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1868,'dgpc02419','networklibraries_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1869,'dgpc02419','Microsoft Visual C++ 2013 Redistributable (x86) - 12.0.21005','12.0.21005.1','','Microsoft Corporation'),(1870,'dgpc02419','dsfsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1871,'dgpc02419','setupsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1872,'dgpc02419','Microsoft Visual C++ 2013 x86 Minimum Runtime - 12.0.21005','12.0.21005','20140729','Microsoft Corporation'),(1873,'dgpc02419','setuptools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1874,'dgpc02419','imagingtools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1875,'dgpc02419','pnptools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1876,'dgpc02419','powermanagement_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1877,'dgpc02419','Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729.4148','9.0.30729.4148','20140804','Microsoft Corporation'),(1878,'dgpc02419','headers','1.1.6001.0','20150404','Microsoft Corporation'),(1879,'dgpc02419','Platform','1.39','20140729','VIA Technologies, Inc.'),(1880,'dgpc02419','Java 8 Update 31','8.0.310','20150213','Oracle Corporation'),(1881,'dgpc02419','bluetoothsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1882,'dgpc02419','audiosamples','1.1.6001.0','20150404','Microsoft Corporation'),(1883,'dgpc02419','Debugging Tools for Windows (x86)','6.11.1.404','20141016','Microsoft Corporation'),(1884,'dgpc02419','generaltools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1885,'dgpc02419','vistalibs_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1886,'dgpc02419','buildtools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1887,'dgpc02419','offreg_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1888,'dgpc02419','MSXML 4.0 SP3 Parser (KB2721691)','4.30.2114.0','20141112','Microsoft Corporation'),(1889,'dgpc02419','Microsoft Visual C++  Compilers 2010 Standard - enu - x86','10.0.30319','20141121','Microsoft Corporation'),(1890,'dgpc02419','tracingtool_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1891,'dgpc02419','Microsoft Visual C++ 2013 Redistributable (x64) - 12.0.21005','12.0.21005.1','','Microsoft Corporation'),(1892,'dgpc02419','tools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1893,'dgpc02419','printtools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1894,'dgpc02419','printtools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1895,'dgpc02419','toolindex','1.1.6001.0','20150404','Microsoft Corporation'),(1896,'dgpc02419','tracingtool_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1897,'dgpc02419','debugfiles_win7','1.1.6001.0','20150404','Microsoft Corporation'),(1898,'dgpc02419','tracingtool_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1899,'dgpc02419','generalsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1900,'dgpc02419','toastersample','1.1.6001.0','20150404','Microsoft Corporation'),(1901,'dgpc02419','fireflysample','1.1.6001.0','20150404','Microsoft Corporation'),(1902,'dgpc02419','eventsample','1.1.6001.0','20150404','Microsoft Corporation'),(1903,'dgpc02419','biometricsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1904,'dgpc02419','Java Auto Updater','2.8.31.13','20150213','Oracle Corporation'),(1905,'dgpc02419','libs_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1906,'dgpc02419','imagingtools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1907,'dgpc02419','VMware vSphere Client 5.5','5.5.0.3838','20140815','VMware, Inc.'),(1908,'dgpc02419','pfd_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1909,'dgpc02419','wnetlibs_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1910,'dgpc02419','cancelsample','1.1.6001.0','20150404','Microsoft Corporation'),(1911,'dgpc02419','bluetoothtools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1912,'dgpc02419','chkinftool_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1913,'dgpc02419','wdftools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1914,'dgpc02419','sideshowsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1915,'dgpc02419','readme','1.1.6001.0','20150404','Microsoft Corporation'),(1916,'dgpc02419','networksamples','1.1.6001.0','20150404','Microsoft Corporation'),(1917,'dgpc02419','SolarWinds Response Time Viewer','1.0.0.162','20141025','SolarWinds'),(1918,'dgpc02419','portiosample','1.1.6001.0','20150404','Microsoft Corporation'),(1919,'dgpc02419','biometrictools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1920,'dgpc02419','storagesamples','1.1.6001.0','20150404','Microsoft Corporation'),(1921,'dgpc02419','bussamples','1.1.6001.0','20150404','Microsoft Corporation'),(1922,'dgpc02419','smartcardsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1923,'dgpc02419','avstreamtools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1924,'dgpc02419','wnetlibs_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1925,'dgpc02419','irsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1926,'dgpc02419','pnptools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1927,'dgpc02419','Microsoft Document Explorer 2008','9.0.21022','20150404','Microsoft Corporation'),(1928,'dgpc02419','Hotfix for Microsoft Document Explorer 2008 (KB953196)','1','','Microsoft Corporation'),(1929,'dgpc02419','streammediasamples','1.1.6001.0','20150404','Microsoft Corporation'),(1930,'dgpc02419','offreg_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1931,'dgpc02419','Microsoft Visual C++ 2005 Redistributable','8.0.61001','20140729','Microsoft Corporation'),(1932,'dgpc02419','usbsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1933,'dgpc02419','drvtools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1934,'dgpc02419','infsample_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1935,'dgpc02419','Microsoft Windows Driver Kit Documentation 7600.091201','6.0.7600.0','20150404','Microsoft'),(1936,'dgpc02419','dfx_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1937,'dgpc02419','Microsoft Visual C++ 2013 Redistributable (x64) - 12.0.21005','12.0.21005.1','','Microsoft Corporation'),(1938,'dgpc02419','evntdrvsample','1.1.6001.0','20150404','Microsoft Corporation'),(1939,'dgpc02419','pfd_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1940,'dgpc02419','Microsoft Visual C++ 2005 Redistributable','8.0.59193','20140919','Microsoft Corporation'),(1941,'dgpc02419','wpdtools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1942,'dgpc02419','Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729','9.0.30729','20140919','Microsoft Corporation'),(1943,'dgpc02419','dfx_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1944,'dgpc02419','Realtek Ethernet Controller Driver','7.86.508.2014','20140729','Realtek'),(1945,'dgpc02419','setuptools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1946,'dgpc02419','tools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1947,'dgpc02419','drvtools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1948,'dgpc02419','hid_inputsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1949,'dgpc02419','ioctlsample','1.1.6001.0','20150404','Microsoft Corporation'),(1950,'dgpc02419','Security Update for Microsoft .NET Framework 4.5.2 (KB2972216)','1','','Microsoft Corporation'),(1951,'dgpc02419','hidsampleinput','1.1.6001.0','20150404','Microsoft Corporation'),(1952,'dgpc02419','dfx_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1953,'dgpc02419','powermanagement_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1954,'dgpc02419','buildsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1955,'dgpc02419','Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729.17','9.0.30729','20140912','Microsoft Corporation'),(1956,'dgpc02419','Microsoft Visual C++ 2012 Redistributable (x64) - 11.0.61030','11.0.61030.0','','Microsoft Corporation'),(1957,'dgpc02419','Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729.6161','9.0.30729.6161','20140729','Microsoft Corporation'),(1958,'dgpc02419','wxplibs_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1959,'dgpc02419','generaltools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1960,'dgpc02419','驱动精灵2014','8.1.108.1323','20150131','驱动精灵2014'),(1961,'dgpc02419','umdfsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1962,'dgpc02419','displaysamples','1.1.6001.0','20150404','Microsoft Corporation'),(1963,'dgpc02419','Microsoft Visual C++ 2012 Redistributable (x86) - 11.0.61030','11.0.61030.0','','Microsoft Corporation'),(1964,'dgpc02419','infsample_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1965,'dgpc02419','bluetoothtools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1966,'dgpc02419','imagingtools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1967,'dgpc02419','wsdtool_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1968,'dgpc02419','drvtools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1969,'dgpc02419','wpdsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1970,'dgpc02419','Microsoft Visual C++ 2012 x86 Additional Runtime - 11.0.61030','11.0.61030','20140729','Microsoft Corporation'),(1971,'dgpc02419','setuptools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1972,'dgpc02419','powermanagement_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1973,'dgpc02419','printtools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1974,'dgpc02419','libs_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1975,'dgpc02419','wmisamples','1.1.6001.0','20150404','Microsoft Corporation'),(1976,'dgpc02419','avstreamtools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1977,'dgpc02419','avstreamtools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1978,'dgpc02419','Microsoft Visual C++ 2012 x86 Minimum Runtime - 11.0.61030','11.0.61030','20140729','Microsoft Corporation'),(1979,'dgpc02419','pnpportssample','1.1.6001.0','20150404','Microsoft Corporation'),(1980,'dgpc02419','DSF-KitSetup','1.1.6001.0','20150404','Microsoft Corporation'),(1981,'dgpc02419','ifssamples','1.1.6001.0','20150404','Microsoft Corporation'),(1982,'dgpc02419','swtuner','1.1.6001.0','20150404','Microsoft Corporation'),(1983,'dgpc02419','hidsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1984,'dgpc02419','oacr_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1985,'dgpc02419','tools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1986,'dgpc02419','Microsoft Visual C++ 2013 Redistributable (x86) - 12.0.21005','12.0.21005.1','','Microsoft Corporation'),(1987,'dgpc02419','generaltools_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1988,'dgpc02419','buildtools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1989,'dgpc02419','libs_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1990,'dgpc02419','sdv','1.1.6001.0','20150404','Microsoft Corporation'),(1991,'dgpc02419','wnetlibs_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(1992,'dgpc02419','ManageEngine Firewall Analyzer 8','7','20150307','ZOHO Corp'),(1993,'dgpc02419','pfd_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1994,'dgpc02419','biometrictools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1995,'dgpc02419','pnptools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1996,'dgpc02419','printsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1997,'dgpc02419','avstreamsamples','1.1.6001.0','20150404','Microsoft Corporation'),(1998,'dgpc02419','buildtools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(1999,'dgpc02419','installhelp','1.1.6001.0','20150404','Microsoft Corporation'),(2000,'dgpc02419','modemtools','1.1.6001.0','20150404','Microsoft Corporation'),(2001,'dgpc02419','bluetoothtools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2002,'dgpc02419','offreg_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2003,'dgpc02419','wpdtools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2004,'dgpc02419','wcoinstallers','1.1.6001.0','20150404','Microsoft Corporation'),(2005,'dgpc02419','wsdtool_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2006,'dgpc02419','wsdtool_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2007,'dgpc02419','Microsoft Visual C++ 2010  x86 Redistributable - 10.0.40219','10.0.40219','20150102','Microsoft Corporation'),(2008,'dgpc02419','networklibraries_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2009,'dgpc02419','wdftools_x64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2010,'dgpc02419','Microsoft Visual C++ 2013 x86 Additional Runtime - 12.0.21005','12.0.21005','20140729','Microsoft Corporation'),(2011,'dgpc02419','Foxit Reader Pro 7.0.8','7.0.8.1216','20150131','三千院雨'),(2012,'dgpc02419','vistalibs_x86fre','1.1.6001.0','20150404','Microsoft Corporation'),(2013,'dgpc02419','wpdtools_ia64fre','1.1.6001.0','20150404','Microsoft Corporation'),(2014,'dgpc02419','pcidrvsample','1.1.6001.0','20150404','Microsoft Corporation'),(2015,'dgpc02419','Microsoft Visual C++ 2008 Redistributable - x86 9.0.21022','9.0.21022','20141027','Microsoft Corporation'),(2016,'dgpc02419','tools-windows','8.8.2.703057','20150406','VMware, Inc.'),(2017,'dgpc02419','天威诚信数字证书助手','2.6.0.1','','iTrusChina'),(2018,'dgpc02419','百度云管家','5.2.0','','百度在线网络技术（北京）有限公司'),(2019,'dgpc02419','金山卫士4.7正式版','4.7.0.4207 正式版','','金山卫士'),(2020,'dgpc02419','阿里旺旺2014Beta1','','20140926','阿里巴巴（中国）有限公司'),(2021,'dgpc02419','Microsoft Visual J# 2.0 Redistributable Package - SE (x64)','','','Microsoft Corporation'),(2022,'dgpc02419','WinRAR 4.10 (64-bit)','4.10.0','','win.rar GmbH'),(2023,'dgpc02419','TortoiseGit 1.8.14.0 (64 bit)','1.8.14.0','20150415','TortoiseGit'),(2024,'dgpc02419','Microsoft .NET Framework 4.5.2 (CHS)','4.5.51209','20140730','Microsoft Corporation'),(2025,'dgpc02419','Microsoft Visual C++ 2010  x64 Redistributable - 10.0.40219','10.0.40219','20140729','Microsoft Corporation'),(2026,'dgpc02419','Microsoft .NET Framework 4.5.2','4.5.51209','20140916','Microsoft Corporation'),(2027,'dgpc02419','Java 7 Update 45 (64-bit)','7.0.450','20140811','Oracle'),(2028,'dgpc02419','Java 8 (64-bit)','8.0.0','20140811','Oracle Corporation'),(2029,'dgpc02419','VmciSockets','9.1.54.1','20150406','VMware, Inc.'),(2030,'dgpc02419','Microsoft Visual C++ 2012 x64 Additional Runtime - 11.0.61030','11.0.61030','20140729','Microsoft Corporation'),(2031,'dgpc02419','MSRedists64','2.00.0000','20140925','Ingres Corporation'),(2032,'dgpc02419','Microsoft Visual C++ 2008 Redistributable - x64 9.0.30729.4148','9.0.30729.4148','20140804','Microsoft Corporation'),(2033,'dgpc02419','Oracle VM VirtualBox 4.3.26','4.3.26','20150406','Oracle Corporation'),(2034,'dgpc02419','Microsoft Visual C++ 2008 Redistributable - x64 9.0.30729.6161','9.0.30729.6161','20140729','Microsoft Corporation'),(2035,'dgpc02419','Microsoft Visual C++  Compilers 2010 Standard - enu - x64','10.0.30319','20141121','Microsoft Corporation'),(2036,'dgpc02419','Microsoft Visual C++ 2013 x64 Additional Runtime - 12.0.21005','12.0.21005','20140729','Microsoft Corporation'),(2037,'dgpc02419','Microsoft Visual C++ 2013 x64 Minimum Runtime - 12.0.21005','12.0.21005','20140729','Microsoft Corporation'),(2038,'dgpc02419','Microsoft Visual C++ 2005 Redistributable (x64)','8.0.61000','20140729','Microsoft Corporation'),(2039,'dgpc02419','Microsoft Visual J# 2.0 Redistributable Package - SE (x64)','2.0.50728','20140804','Microsoft Corporation'),(2040,'dgpc02419','Microsoft SQL Server 2008 Native Client','10.0.1600.22','20141024','Microsoft Corporation'),(2041,'dgpc02419','Microsoft Visual C++ 2012 x64 Minimum Runtime - 11.0.61030','11.0.61030','20140729','Microsoft Corporation'),(2042,'dgpc02419','TightVNC','2.7.10.0','20140812','GlavSoft LLC.'),(2043,'dgpc02419','Debugging Tools for Windows (x64)','6.12.2.633','20150404','Microsoft Corporation'),(2044,'dgpc02419','True Launch Bar','6.1','','Tordex'),(2109,'china1111','Internet Explorer 7 (200806)','20070813.185237','20150426','Microsoft Corporation'),(2110,'china1111','WebFldrs XP','9.50.7523','20150417','Microsoft Corporation');
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
  `OSVersion` varchar(128) NOT NULL,
  `InstallationDate` date DEFAULT NULL,
  `OSKey` varchar(45) DEFAULT NULL,
  `Workgroup` varchar(32) NOT NULL,
  `JoinedToDomain` tinyint(1) unsigned DEFAULT '0',
  `Users` varchar(255) DEFAULT NULL,
  `Administrators` varchar(255) DEFAULT NULL,
  `IP` varchar(255) NOT NULL DEFAULT '',
  `ClientVersion` varchar(32) DEFAULT NULL,
  `ProcessMonitorEnabled` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '0:Disabled\n1:Enabled',
  `USB` tinyint(1) unsigned NOT NULL DEFAULT '1' COMMENT 'USBSTOR_Disabled = 0, USBSTOR_ReadWrite = 1,\nUSBSTOR_ReadOnly = 2,\nUSBSTOR_Unknown = 3',
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
INSERT INTO `OS` VALUES ('china1111','china-74d290c0d','Microsoft Windows XP Professional Service Pac','2015-04-17','MRX3F-47B9T-2487J-KWKMF-RPWBY','WORKGROUP',0,'hehui','administrator;hehui','200.200.200.106','2015.5.4.1',0,0,'2015-05-15 07:38:10'),('dgpc02419','dgpc02419','Microsoft Windows 7 旗舰版  Service Pack 1 64-bi','2014-07-29','FJGCP-4DFJD-GJY49-VJBQ7-HYRR2','WORKGROUP',0,'hehui','administrator;hehui','','2014.12.23.1',0,0,'2015-04-17 15:42:45'),('pc1','pc1','Microsoft Windows XP Professional Service Pac','2015-04-17','MRX3F-47B9T-2487J-KWKMF-RPWBY','WORKGROUP',0,'hehui','administrator;hehui','200.200.200.106','2014.12.23.1',0,0,'2015-04-28 09:44:02'),('pc2','pc2','Microsoft Windows XP Professional Service Pac','2015-04-17','MRX3F-47B9T-2487J-KWKMF-RPWBY','WORKGROUP',0,'hehui','administrator;hehui','200.200.200.106','2014.12.23.1',0,0,'2015-04-28 09:44:02'),('pc3','pc3','Microsoft Windows XP Professional Service Pac','2015-04-17','MRX3F-47B9T-2487J-KWKMF-RPWBY','WORKGROUP',0,'hehui','administrator;hehui','200.200.200.106','2014.12.23.1',0,0,'2015-04-28 09:44:02');
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
  `UserName` varchar(24) DEFAULT NULL,
  `UserPassword` varchar(64) NOT NULL COMMENT '密码两次MD5，小写',
  `BusinessAddress` varchar(64) DEFAULT NULL,
  `LastLoginIP` varchar(64) DEFAULT NULL,
  `LastLoginPC` varchar(32) DEFAULT NULL,
  `LastLoginTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `Readonly` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `Active` tinyint(1) unsigned NOT NULL DEFAULT '1',
  `Remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SystemAdministrators`
--

LOCK TABLES `SystemAdministrators` WRITE;
/*!40000 ALTER TABLE `SystemAdministrators` DISABLE KEYS */;
INSERT INTO `SystemAdministrators` VALUES ('guest','Guest','46fe47e8b67cad6074a7dfd508be9dd9','','dgpc02419','200.200.200.17','2015-04-17 16:33:26',1,1,NULL),('hehui','He Hui','46fe47e8b67cad6074a7dfd508be9dd9','','dgpc02419','200.200.200.17','2015-05-15 07:27:11',0,1,NULL),('test','NNNN111','ddf63c1a8f2492aa4e41b34e93a9d54e','AAA111','dgpc02419','200.200.200.17','2015-04-24 10:25:37',1,0,'RRRRRRR111');
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
/*!50003 DROP PROCEDURE IF EXISTS `sp_Admins_Query` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Admins_Query`()
BEGIN

###t查询所有管理员信息###

    select * from SystemAdministrators;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Admin_Delete` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Admin_Delete`( in p_adminID varchar(32) )
BEGIN

###删除管理员###

    delete from SystemAdministrators where UserID=p_adminID;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Admin_Login` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Admin_Login`(in p_adminID varchar(32), 
                                                                in p_adminPassword varchar(64), 
                                                                in p_adminIP varchar(64), 
                                                                in p_adminPC varchar(32) 

                                                                )
BEGIN

###t管理员登陆###

	set @pswd= null;
	select UserPassword into @pswd from SystemAdministrators where UserID=p_adminID;
	if @pswd = p_adminPassword then
        update SystemAdministrators set LastLoginIP=p_adminIP, LastLoginPC=p_adminPC, LastLoginTime=NULL where UserID=p_adminID;
        select Readonly from SystemAdministrators where UserID=p_adminID;
	end if;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Admin_Update_Info` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Admin_Update_Info`(in p_UserID varchar(32), 
                                                                in p_UserName varchar(32), 
                                                                in p_UserPassword varchar(64), 
                                                                in p_BusinessAddress varchar(64), 
                                                                in p_Readonly tinyint, 
                                                                in p_Active tinyint,
                                                                in p_Remark varchar(255)

                                                                )
BEGIN

###更新管理员信息###

	set @pswd= null;
	select UserPassword into @pswd from SystemAdministrators where UserID=p_UserID;
	if @pswd is null then
        insert into SystemAdministrators(UserID, UserName, UserPassword, BusinessAddress, Readonly, Active, Remark) 
        values(p_UserID, p_UserName, p_UserPassword, p_BusinessAddress,  p_Readonly, p_Active, p_Remark) ;
    else
        update SystemAdministrators set
        UserName=p_UserName, 
        UserPassword=p_UserPassword, 
        BusinessAddress=p_BusinessAddress ,
        Readonly=p_Readonly,
        Active=p_Active,
        Remark=p_Remark
        where UserID=p_UserID;
	end if;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Admin_Update_Login_Info` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Admin_Update_Login_Info`(in p_adminID varchar(32), 
                                                                in p_adminIP varchar(64), 
                                                                in p_adminPC varchar(32) 

                                                                )
BEGIN

###t管理员登陆###

        update SystemAdministrators set LastLoginIP=p_adminIP, LastLoginPC=p_adminPC, LastLoginTime=NULL where UserID=p_adminID;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Alarms_Acknowledge` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Alarms_Acknowledge`(
 in p_adminID varchar(32),
 in p_Alarms varchar(255),
 in p_deleteAlarms tinyint
                                                                                    
)
BEGIN

###确认或删除报警信息###


declare whereStatement varchar(1024) default ' ';
set whereStatement = CONCAT_WS('', whereStatement, ' where ID in (', p_Alarms, ')');

if p_deleteAlarms <> 0 then
    set @sqlQuery = CONCAT_WS(' ', ' delete from Alarms  ', whereStatement );
else
    set @sqlQuery = CONCAT_WS(' ', ' update Alarms set Acknowledged=1, AcknowledgedBy=\'', p_adminID, '\', AcknowledgedTime=now()   ', whereStatement );
end if;

PREPARE s1 FROM @sqlQuery;
EXECUTE s1;
DEALLOCATE PREPARE s1;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Alarms_GetCount` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Alarms_GetCount`()
BEGIN

###查询报警信息数量###

SET @TotalCount=0, @Unacknowledged=0;
SELECT count(ID) into @TotalCount FROM `Alarms` ;
SELECT count(ID) into @Unacknowledged FROM Alarms WHERE Acknowledged=0  ;

SELECT @TotalCount, @Unacknowledged;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Alarms_Insert` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Alarms_Insert`(
 in p_AssetNO varchar(32),
 in p_AlarmType tinyint,
 in p_Message varchar(255)
                                                                                    
)
BEGIN

###新增报警信息###

insert into Alarms(AssetNO, AlarmType, Message) values(p_AssetNO, p_AlarmType, p_Message);

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Alarms_Query` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Alarms_Query`(
 in p_AssetNO varchar(32),
 in p_AlarmType tinyint,
 in p_Acknowledged tinyint,
 in p_startTime DateTime,
 in p_endTime DateTime
                                                                                    
)
BEGIN

###查询报警信息###

declare whereStatement varchar(1024) default ' ';

set whereStatement = CONCAT_WS(' ',  '  (UpdateTime  between \' ', p_startTime, ' \'and \'',p_endTime, '\' ) ');

if CHAR_LENGTH(p_AssetNO) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and AssetNO  like \'%', p_AssetNO, '%\'');
end if;

if p_AlarmType >= 0 then
set whereStatement = CONCAT_WS(' ', whereStatement, ' and AlarmType=', p_AlarmType);
end if;

if p_Acknowledged >= 0 then
set whereStatement = CONCAT_WS(' ', whereStatement, ' and Acknowledged=', p_Acknowledged);
end if;

set @fullStatement = CONCAT_WS(' ', ' select * from Alarms where  ', whereStatement );

#SET @sqlQuery=@fullStatement;
PREPARE s1 FROM @fullStatement;
EXECUTE s1;
DEALLOCATE PREPARE s1;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_AnnouncementReplies_Insert` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_AnnouncementReplies_Insert`(
 out p_ID int unsigned,
 in p_AnnouncementID int unsigned,
 in p_Sender varchar(32),
  in p_SendersAssetNO varchar(32),
 in p_Receiver varchar(32),
  in p_ReceiversAssetNO varchar(32),
 in p_Message text

)
BEGIN

###新增公告信息###

insert into AnnouncementReplys(Announcement, Sender, SendersAssetNO, Receiver, ReceiversAssetNO, Message) 
values(p_AnnouncementID, p_Sender, p_SendersAssetNO, p_Receiver, p_ReceiversAssetNO, p_Message);

select last_insert_id() into p_ID;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_AnnouncementReplies_Query` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_AnnouncementReplies_Query`(
 in p_AnnouncementID int unsigned,
 in p_Receiver varchar(32)

)
BEGIN

###查询公告回复信息###

if CHAR_LENGTH(p_Receiver) <> 0 then
    SELECT * FROM AnnouncementReplys WHERE Announcement=p_AnnouncementID and Receiver=p_Receiver;
else
    SELECT * FROM AnnouncementReplys WHERE Announcement=p_AnnouncementID;
end if;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Announcements_Insert` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Announcements_Insert`(
 out p_ID int unsigned,
 in p_Type tinyint,
 in p_Content text,
 in p_ACKRequired tinyint,
 in p_Admin varchar(32),
 in p_ValidityPeriod int unsigned,
 in p_TargetType tinyint,
 in p_DisplayTimes int unsigned

)
BEGIN

###新增公告信息###

insert into Announcements(AnnouncementType, Content, ACKRequired, Admin, ValidityPeriod, TargetType, DisplayTimes) 
values(p_Type, p_Content, p_ACKRequired, p_Admin, p_ValidityPeriod, p_TargetType, p_DisplayTimes);

select last_insert_id() into p_ID;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Announcements_Query` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Announcements_Query`(
 in p_IDs varchar(255),
 in p_Keyword varchar(32),
 in p_Validity tinyint,
 in p_AssetNO varchar(32),
 in p_UserName varchar(32),
 in p_TargetType tinyint,
 in p_startTime DateTime,
 in p_endTime DateTime
                                                                                    
)
BEGIN

###查询公告信息###

declare whereStatement varchar(1024) default ' ';

set whereStatement = CONCAT_WS(' ',  '  (PublishDate  between \' ', p_startTime, ' \'and \'',p_endTime, '\' ) ');

if CHAR_LENGTH(p_IDs) <> 0 then
#逗号分隔的ID列表
set whereStatement = CONCAT_WS('', whereStatement, ' and ID in (', p_IDs, ')');
end if;

if CHAR_LENGTH(p_Keyword) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and Content  like \'%', p_Keyword, '%\'');
end if;


if p_Validity >= 0 then

if p_Validity = 0 then
#Already Expire
set whereStatement = CONCAT_WS(' ', whereStatement, ' and (select ( PublishDate + INTERVAL ValidityPeriod  MINUTE)  < now()  ) ');
elseif p_Validity = 1 then
#Not Expired
set whereStatement = CONCAT_WS(' ', whereStatement, ' and (select ( PublishDate + INTERVAL ValidityPeriod  MINUTE)  > now()   or ValidityPeriod=0 )  ');
end if;

end if;


if CHAR_LENGTH(p_AssetNO) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and tb_ann.ID in (select t.Announcement from AnnouncementTargets t where t.AssetNO  like \'%', p_AssetNO, '%\' )');
end if;

if CHAR_LENGTH(p_UserName) <> 0 then
set whereStatement = CONCAT_WS('', whereStatement, ' and tb_ann.ID in (select t.Announcement from AnnouncementTargets t where UserName  like \'%', p_UserName, '%\' )');
end if;

if p_TargetType >= 0 then
set whereStatement = CONCAT_WS(' ', whereStatement, ' and TargetType=', p_TargetType);
end if;


set @fullStatement = CONCAT_WS(' ', ' select * from Announcements tb_ann where  ', whereStatement );
#select  @fullStatement ;


#SET @sqlQuery=@fullStatement;
PREPARE s1 FROM @fullStatement;
EXECUTE s1;
DEALLOCATE PREPARE s1;


END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Announcements_Update` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Announcements_Update`(
 in p_ID int unsigned,
 in p_Active tinyint,
 out p_TargetType tinyint

)
BEGIN

###更新公告信息###

update Announcements set Active=p_Active where ID=p_ID;
select TargetType into p_TargetType from Announcements where ID=p_ID;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_AnnouncementTargets_Insert` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_AnnouncementTargets_Insert`(
 in p_AnnouncementID int unsigned,
 in p_AssetNO varchar(32),
 in p_UserName varchar(32)

)
BEGIN

###新增公告目标###

INSERT INTO AnnouncementTargets (Announcement, AssetNO, UserName)
VALUES(p_AnnouncementID, p_AssetNO, p_UserName);

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_AnnouncementTargets_Query` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_AnnouncementTargets_Query`(
 in p_AnnouncementID int unsigned

)
BEGIN

###新增公告目标###

SELECT * FROM AnnouncementTargets WHERE Announcement=p_AnnouncementID;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_CurrentUTCTimestamp` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_CurrentUTCTimestamp`()
BEGIN
###查询当前的UTC时间
select utc_timestamp();
END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Hardware_Query` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Hardware_Query`(in p_AssetNO varchar(32)

                                                                )
BEGIN

###查询硬件信息###

select * from Hardware where AssetNO  like p_AssetNO;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
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
/*!50003 DROP PROCEDURE IF EXISTS `sp_OS_AssetNO_Update` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_OS_AssetNO_Update`(in p_newAssetNO varchar(32), 
                                                                in p_oldAssetNO varchar(32)
                                                                  
                                                                )
BEGIN

###更新资产编号###

        update OS set AssetNO=p_newAssetNO where AssetNO=p_oldAssetNO;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_Test` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_Test`(
 in p_Validity tinyint,
 in p_AssetNO varchar(32),
 in p_UserName varchar(32)

                                                                                    
)
BEGIN

###查询公告信息###






select * from Announcements where  (select ( PublishDate + INTERVAL ValidityPeriod  MINUTE)  < now() );




END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `sp_AnnouncementTargets_Update` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50020 DEFINER=`root`@`200.200.200.17`*/ /*!50003 PROCEDURE `sp_AnnouncementTargets_Update`(
 in p_AnnouncementID int unsigned,
  in p_addedTargets text,
 in p_deletedTargets text

)
BEGIN

###更新公告目标###
#p_addedTargets Format: 'PC1','User1';'PC2','User2';'','User3'
#p_deletedTargets Format: Announcement1,Announcement2,Announcement3

declare statement varchar(1024) default ' ';

set @announcementTargetType=0;
select TargetType into @announcementTargetType from Announcements where ID=p_AnnouncementID;
if @announcementTargetType=1 then

    if CHAR_LENGTH(p_deletedTargets) <> 0 then
        set statement = CONCAT_WS(' ',  ' delete from AnnouncementTargets where ID in( ', p_deletedTargets, ' ) ;');
    end if;
    
set @tempstr = concat("),(", p_AnnouncementID, ",");
set @statement = concat(statement, concat("insert into AnnouncementTargets(Announcement,AssetNO,UserName) values(", p_AnnouncementID, ",", replace(p_addedTargets,';',@tempstr)), ");" );
#select @statement;

PREPARE s1 FROM @statement;
EXECUTE s1;
DEALLOCATE PREPARE s1;

else
    delete from AnnouncementTargets where ID=p_ID;
end if;

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
                                                                 in p_ProcessMonitorEnabled tinyint ,
                                                                 in p_USB tinyint
                                                                  
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

if p_ProcessMonitorEnabled >= 0 then
set whereStatement = CONCAT_WS(' ', whereStatement, ' and ProcessMonitorEnabled=', p_ProcessMonitorEnabled);
end if;

if p_USB >= 0 then
set whereStatement = CONCAT_WS(' ', whereStatement, ' and USB=', p_USB);
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
                                                                 in p_ClientVersion varchar(32),
                                                                  in p_USB tinyint
                                                                  
                                                                )
BEGIN

###新增或更新系统信息###

	set @computerName= null;
	select ComputerName into @computerName from OS where AssetNO=p_AssetNO;
	if @computerName is null then
        insert into OS values(p_AssetNO, p_ComputerName, p_OS, p_InstallationDate,  p_OSKey, p_Workgroup, p_JoinedToDomain, p_Users, p_Admins, p_IP, p_ClientVersion, 0, 1,NULL) ;
        insert into Hardware(AssetNO) values(p_AssetNO);
    else
        update OS set 
        ComputerName=p_ComputerName, OSVersion=p_OS, InstallationDate=p_InstallationDate,  OSKey=p_OSKey, Workgroup=p_Workgroup, JoinedToDomain=p_JoinedToDomain, Users=p_Users, Administrators=p_Admins, IP=p_IP, ClientVersion=p_ClientVersion, USB=p_USB, LastOnlineTime=NULL
        where AssetNO=p_AssetNO;
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

###新增或更新进程监控信息###

	set @admin= null, @rulesVersion = 0;
	select RulesVersion, Admin into @rulesVersion, @admin from ProcessMonitorSettings where AssetNO=p_AssetNO;
	if @admin is null then
		insert into ProcessMonitorSettings values(NULL, p_AssetNO, p_ProcessMonitorEnabled, p_Rules, p_PassthroughEnabled,  p_LogAllowedProcess, p_LogBlockedProcess, p_UseGlobalRules, p_RulesVersion, p_Admin, NULL) ;
    else
        update ProcessMonitorSettings set 
        ProcessMonitorEnabled=p_ProcessMonitorEnabled, Rules=p_Rules,  PassthroughEnabled=p_PassthroughEnabled, LogAllowedProcess=p_LogAllowedProcess, LogBlockedProcess=p_LogBlockedProcess, UseGlobalRules=p_UseGlobalRules, RulesVersion=(@rulesVersion+1),  Admin=p_Admin, UpdateTime=NULL
        where AssetNO=p_AssetNO;
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

-- Dump completed on 2015-05-15 15:52:18
