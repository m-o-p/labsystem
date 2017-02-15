CREATE TABLE IF NOT EXISTS `user_rights` (
  `idx` bigint(20) NOT NULL auto_increment,
  `uid` varchar(32) NOT NULL DEFAULT '',
  `rights` bigint(20) NOT NULL DEFAULT '0' COMMENT 'The rights the user can maximally have.',
  `enabledRights` bigint(20) DEFAULT NULL COMMENT 'The rights the user has currently enabled.',
  `currentTeam` int(9) NOT NULL DEFAULT '0',
  `history` text NOT NULL,
  PRIMARY KEY (`idx`),
  KEY `uid` (`uid`)
) AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
