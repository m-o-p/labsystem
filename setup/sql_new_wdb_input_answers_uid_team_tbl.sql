CREATE TABLE IF NOT EXISTS `input_answers_uid_team` (
  `idx` bigint(20) NOT NULL auto_increment,
  `i_idx` bigint(20) default NULL,
  `uid` varchar(32) default NULL,
  `team` int(9) default NULL,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=3 ;

INSERT INTO `input_answers_uid_team` (`idx`, `i_idx`, `uid`, `team`, `history`) VALUES
(1, 3, 'participant', 55555, '2007-09-04 18:06:52: The Tester\r\n'),
(2, 2, 'participant', 55555, '2007-09-04 18:08:19: The Tester: - user reMapped -\r\n');
