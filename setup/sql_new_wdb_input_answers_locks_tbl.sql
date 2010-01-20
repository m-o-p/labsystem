CREATE TABLE IF NOT EXISTS `input_answers_locks` (
  `idx` bigint(20) NOT NULL auto_increment,
  `team` int(9) default NULL,
  `lock_on_idx` bigint(20) default NULL,
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=1 ;