CREATE TABLE IF NOT EXISTS `multiple_choice_answers` (
  `idx` bigint(20) NOT NULL auto_increment,
  `mc_idx` bigint(20) default NULL,
  `uid` varchar(32) default NULL,
  `answer_array` text,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=2 ;

INSERT INTO `multiple_choice_answers` (`idx`, `mc_idx`, `uid`, `answer_array`, `history`) VALUES 
(1, 2, 'participant', 'a:5:{i:0;a:3:{i:0;i:1;i:1;i:2;i:2;i:0;}i:1;i:6;i:2;i:7;i:3;i:5;i:4;s:1:"0";}', '2007-09-04 18:08:19: The Tester: -check-\r\n2007-09-04 18:08:18: The Tester\r\n2007-09-04 18:08:12: The Tester: -check-\r\n2007-09-04 18:08:11: The Tester\r\n2007-09-04 18:08:05: The Tester: -check-\r\n2007-09-04 18:06:49: The Tester\r\n2007-09-04 18:06:41: The Tester: - created -\r\n');
