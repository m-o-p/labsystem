CREATE TABLE IF NOT EXISTS `labs` (
  `idx` bigint(20) NOT NULL auto_increment,
  `title` varchar(255) default NULL,
  `comment` text,
  `prelab_collection_idx` bigint(20) default NULL,
  `lab_collection_idx` bigint(20) default NULL,
  `matching_menu` varchar(255) default NULL,
  `visible_only_in_collection` tinyint(1) default '0',
  `visible_before_first_sched` tinyint(1) default '0',
  `visible_during_sched` tinyint(1) default '0',
  `visible_after_first_sched` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=3 ;

INSERT INTO `labs` (`idx`, `title`, `comment`, `prelab_collection_idx`, `lab_collection_idx`, `matching_menu`, `visible_only_in_collection`, `visible_before_first_sched`, `visible_during_sched`, `visible_after_first_sched`, `history`) VALUES
(1, 'prototype of a lab element', 'author', 1, 1, '', 0, 0, 1, 1, '2006-03-11 19:10:58: Marc-Oliver Pahl'),
(2, 'Demonstration lab', '', 6, 5, '', 0, 0, 1, 1, '2006-03-11 19:10:58: Marc-Oliver Pahl');
