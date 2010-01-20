-- phpMyAdmin SQL Dump
-- version 2.9.0.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Erstellungszeit: 04. September 2007 um 18:28
-- Server Version: 4.0.24
-- PHP-Version: 5.2.0
-- 
-- Datenbank: `web10_db8`
-- 

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `input_answers`
-- 

DROP TABLE IF EXISTS `input_answers`;
CREATE TABLE `input_answers` (
  `idx` bigint(20) NOT NULL auto_increment,
  `i_idx` bigint(20) default NULL,
  `team` int(9) default NULL,
  `answer` text,
  `comment` text,
  `given_credits` int(11) default NULL,
  `closed` tinyint(1) default '0',
  `corrected` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) TYPE=MyISAM AUTO_INCREMENT=3 ;

-- 
-- Daten für Tabelle `input_answers`
-- 

INSERT INTO `input_answers` (`idx`, `i_idx`, `team`, `answer`, `comment`, `given_credits`, `closed`, `corrected`, `history`) VALUES 
(1, 3, 55555, 'As corrector you can write a comment below.\r\nYou can give points.\r\nAnd you can leave the "isCorrected" field unchecked. So the Participant can not see the correction but your corrector collegues...', '', 0, 1, 0, '2007-09-04 18:09:15: The Tester: - closes input -\r\n2007-09-04 18:07:58: The Tester\r\n2007-09-04 18:06:52: The Tester: - created -\r\n'),
(2, 2, 55555, 'This is the answer of the Participant...\r\n\r\nShe can put in lots of things...\r\n\r\n10: br0: <BROADCAST,MULTICAST,UP,10000> mtu 1500 qdisc noqueue\r\n    link/ether 00:ff:65:77:89:94 brd ff:ff:ff:ff:ff:ff\r\n    inet 192.168.97.1/24 brd 192.168.97.255 scope global br0\r\n    inet6 2001:6f8:108c:97::1/64 scope global\r\n       valid_lft forever preferred_lft forever\r\n    inet6 fe80::2ff:65ff:fe77:8994/64 scope link\r\n       valid_lft forever preferred_lft forever\r\n11: tap2: <BROADCAST,MULTICAST,UP,10000> mtu 1500 qdisc pfifo_fast qlen 100\r\n    link/ether 00:ff:5b:19:8d:0c brd ff:ff:ff:ff:ff:ff\r\n    inet 192.168.99.5/24 brd 192.168.99.255 scope global tap2\r\n    inet6 2001:6f8:108c:99::5/64 scope global\r\n       valid_lft forever preferred_lft forever\r\n    inet6 fe80::2ff:5bff:fe19:8d0c/64 scope link\r\n       valid_lft forever preferred_lft forever\r\n', '', 0, 1, 0, '2007-09-04 18:09:15: The Tester: - closes input -\r\n2007-09-04 18:09:04: The Tester\r\n2007-09-04 18:08:19: The Tester: - created -\r\n');

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `input_answers_locks`
-- 

DROP TABLE IF EXISTS `input_answers_locks`;
CREATE TABLE `input_answers_locks` (
  `idx` bigint(20) NOT NULL auto_increment,
  `team` int(9) default NULL,
  `lock_on_idx` bigint(20) default NULL,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) TYPE=MyISAM AUTO_INCREMENT=3 ;

-- 
-- Daten für Tabelle `input_answers_locks`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `input_answers_uid_team`
-- 

DROP TABLE IF EXISTS `input_answers_uid_team`;
CREATE TABLE `input_answers_uid_team` (
  `idx` bigint(20) NOT NULL auto_increment,
  `i_idx` bigint(20) default NULL,
  `uid` varchar(32) default NULL,
  `team` int(9) default NULL,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) TYPE=MyISAM AUTO_INCREMENT=3 ;

-- 
-- Daten für Tabelle `input_answers_uid_team`
-- 

INSERT INTO `input_answers_uid_team` (`idx`, `i_idx`, `uid`, `team`, `history`) VALUES 
(1, 3, 'participant', 55555, '2007-09-04 18:06:52: The Tester\r\n'),
(2, 2, 'participant', 55555, '2007-09-04 18:08:19: The Tester: - user reMapped -\r\n');

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `lab_uid_status`
-- 

DROP TABLE IF EXISTS `lab_uid_status`;
CREATE TABLE `lab_uid_status` (
  `idx` bigint(20) NOT NULL auto_increment,
  `l_idx` bigint(20) default NULL,
  `uid` varchar(32) default NULL,
  `current_team` int(9) NOT NULL default '0',
  `prelab_finished` tinyint(1) default '0',
  `prelab_given_credits` int(11) default '0',
  `prelab_possible_credits` int(11) default '0',
  `prelab_all_teammembers_finished` tinyint(1) default '0',
  `lab_closed` tinyint(1) default '0',
  `lab_corrected` tinyint(1) default '0',
  `which_corrected` varchar(255) NOT NULL default '',
  `lab_given_credits` int(11) default '0',
  `lab_possible_credits` int(11) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) TYPE=MyISAM AUTO_INCREMENT=2 ;

-- 
-- Daten für Tabelle `lab_uid_status`
-- 

INSERT INTO `lab_uid_status` (`idx`, `l_idx`, `uid`, `current_team`, `prelab_finished`, `prelab_given_credits`, `prelab_possible_credits`, `prelab_all_teammembers_finished`, `lab_closed`, `lab_corrected`, `which_corrected`, `lab_given_credits`, `lab_possible_credits`, `history`) VALUES 
(1, 2, 'participant', 55555, 1, 100, 300, 1, 1, 0, '', 0, 0, '2007-09-04 18:09:15: The Tester: - lab closed -\r\n2007-09-04 18:08:19: - all teammates finished prelab -\r\n2007-09-04 18:08:19: The Tester: - user finished prelab -\r\n');

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `multiple_choice_answers`
-- 

DROP TABLE IF EXISTS `multiple_choice_answers`;
CREATE TABLE `multiple_choice_answers` (
  `idx` bigint(20) NOT NULL auto_increment,
  `mc_idx` bigint(20) default NULL,
  `uid` varchar(32) default NULL,
  `answer_array` text,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) TYPE=MyISAM AUTO_INCREMENT=2 ;

-- 
-- Daten für Tabelle `multiple_choice_answers`
-- 

INSERT INTO `multiple_choice_answers` (`idx`, `mc_idx`, `uid`, `answer_array`, `history`) VALUES 
(1, 2, 'participant', 'a:5:{i:0;a:3:{i:0;i:1;i:1;i:2;i:2;i:0;}i:1;i:6;i:2;i:7;i:3;i:5;i:4;s:1:"0";}', '2007-09-04 18:08:19: The Tester: -check-\r\n2007-09-04 18:08:18: The Tester\r\n2007-09-04 18:08:12: The Tester: -check-\r\n2007-09-04 18:08:11: The Tester\r\n2007-09-04 18:08:05: The Tester: -check-\r\n2007-09-04 18:06:49: The Tester\r\n2007-09-04 18:06:41: The Tester: - created -\r\n');

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `schedules`
-- 

DROP TABLE IF EXISTS `schedules`;
CREATE TABLE `schedules` (
  `idx` bigint(20) NOT NULL auto_increment,
  `id` char(1) NOT NULL default '',
  `num` bigint(20) NOT NULL default '0',
  `start` datetime NOT NULL default '0000-00-00 00:00:00',
  `stop` datetime NOT NULL default '0000-00-00 00:00:00',
  `comment` text NOT NULL,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) TYPE=MyISAM AUTO_INCREMENT=3 ;

-- 
-- Daten für Tabelle `schedules`
-- 

INSERT INTO `schedules` (`idx`, `id`, `num`, `start`, `stop`, `comment`, `history`) VALUES 
(1, 'l', 1, '2005-04-15 00:00:00', '2005-04-15 23:59:59', '', '2007-09-04 18:06:38: Marc-Oliver Pahl'),
(2, 'l', 2, now(), DATE_ADD( NOW(), INTERVAL 1 MONTH ), 'You can delete this demo schedule by clicking on the trash can. You must have scheduling rights to do this...', '2007-09-04 18:06:38: Marc-Oliver Pahl');

-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `user_rights`
-- 

DROP TABLE IF EXISTS `user_rights`;
CREATE TABLE `user_rights` (
  `idx` bigint(20) NOT NULL auto_increment,
  `uid` varchar(32) NOT NULL default '',
  `rights` bigint(20) NOT NULL default '0',
  `currentTeam` int(9) NOT NULL default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`),
  KEY `uid` (`uid`)
) TYPE=MyISAM AUTO_INCREMENT=3 ;

-- 
-- Daten für Tabelle `user_rights`
-- 

INSERT INTO `user_rights` (`idx`, `uid`, `rights`, `currentTeam`, `history`) VALUES 
(1, 'participant', 49, 55555, ''),
(2, 'demoUser', 49, 1234, '');
