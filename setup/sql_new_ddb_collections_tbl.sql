CREATE TABLE IF NOT EXISTS `collections` (
  `idx` bigint(20) NOT NULL auto_increment,
  `title` varchar(255) default NULL,
  `contents` text,
  `matching_menu` varchar(255) default NULL,
  `visible_only_in_collection` tinyint(1) default '0',
  `visible_before_first_sched` tinyint(1) default '0',
  `visible_during_sched` tinyint(1) default '0',
  `visible_after_first_sched` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=7 ;

INSERT INTO `collections` (`idx`, `title`, `contents`, `matching_menu`, `visible_only_in_collection`, `visible_before_first_sched`, `visible_during_sched`, `visible_after_first_sched`, `history`) VALUES
(1, 'prototype of a collection element', '', '', 1, 0, 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' )),
(2, 'Demonstration PreLab content', 'p4 m2 p8', '', 1, 0, 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' )),
(3, 'Demonstration Lab content...', 'p7 i2', '', 1, 0, 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' )),
(4, 'Suggestions/ complaints', 'p9 i3 p12', '', 1, 0, 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' )),
(5, 'Demonstration Lab', 'c3 c4', '', 1, 0, 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' )),
(6, 'Demonstration PreLab', 'c2 c4', '', 1, 0, 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' ));

CREATE TABLE IF NOT EXISTS `bak_collections` (
  `idx` bigint(20) NOT NULL auto_increment,
  `title` varchar(255) default NULL,
  `contents` text,
  `matching_menu` varchar(255) default NULL,
  `visible_only_in_collection` tinyint(1) default '0',
  `visible_before_first_sched` tinyint(1) default '0',
  `visible_during_sched` tinyint(1) default '0',
  `visible_after_first_sched` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
);