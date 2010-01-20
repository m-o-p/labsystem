CREATE TABLE IF NOT EXISTS `schedules` (
  `idx` bigint(20) NOT NULL auto_increment,
  `id` char(1) NOT NULL default '',
  `num` bigint(20) NOT NULL default '0',
  `start` datetime NOT NULL default '0000-00-00 00:00:00',
  `stop` datetime NOT NULL default '0000-00-00 00:00:00',
  `comment` text NOT NULL,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=3 ;

INSERT INTO `schedules` (`idx`, `id`, `num`, `start`, `stop`, `comment`, `history`) VALUES
(1, 'l', 1, '2005-04-15 00:00:00', '2005-04-15 23:59:59', '', '2007-09-04 18:06:38: Marc-Oliver Pahl'),
(2, 'l', 2, now(), DATE_ADD( NOW(), INTERVAL 3 MONTH ), 'You can delete this demo schedule by clicking on the trash can. You must have scheduling rights to do this...', '2007-09-04 18:06:38: Marc-Oliver Pahl');