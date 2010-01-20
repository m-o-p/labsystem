CREATE TABLE IF NOT EXISTS `user_rights` (
  `idx` bigint(20) NOT NULL auto_increment,
  `uid` varchar(32) NOT NULL default '',
  `rights` bigint(20) NOT NULL default '0',
  `currentTeam` int(9) NOT NULL default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`),
  KEY `uid` (`uid`)
) AUTO_INCREMENT=1;