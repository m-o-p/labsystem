CREATE TABLE IF NOT EXISTS `lab_uid_status` (
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
) AUTO_INCREMENT=2 ;

INSERT INTO `lab_uid_status` (`idx`, `l_idx`, `uid`, `current_team`, `prelab_finished`, `prelab_given_credits`, `prelab_possible_credits`, `prelab_all_teammembers_finished`, `lab_closed`, `lab_corrected`, `which_corrected`, `lab_given_credits`, `lab_possible_credits`, `history`) VALUES
(1, 2, 'participant', 55555, 1, 100, 300, 1, 1, 0, '', 0, 0, '2007-09-04 18:09:15: The Tester: - lab closed -\r\n2007-09-04 18:08:19: - all teammates finished prelab -\r\n2007-09-04 18:08:19: The Tester: - user finished prelab -\r\n');
