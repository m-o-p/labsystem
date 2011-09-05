CREATE TABLE IF NOT EXISTS `inputs` (
  `idx` bigint(20) NOT NULL auto_increment,
  `question` text,
  `example_solution` text,
  `hasFileUpload` tinyint(1) DEFAULT '0',
  `possible_credits` int(11) default NULL,
  `visible_for` bigint(20) default NULL,
  `visible_only_in_collection` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=4 ;

INSERT INTO `inputs` (`idx`, `question`, `example_solution`, `hasFileUpload`, `possible_credits`, `visible_for`, `visible_only_in_collection`, `history`) VALUES
(1, 'Your question...', 'Well here will be what you should have answered...', 0, 1375, 1, 1, '2006-03-11 19:10:58: Marc-Oliver Pahl'),
(2, '...and here will be an interesting question to the experiment.\r\nYou can answer this demo question by clicking on "give answer" below.\r\nMaybe there are some answers yet. How come? Well probably another teammate inserted something yet...', 'Well here will be what you should have answered...', 0, 0, 1, 1, '2006-03-11 19:10:58: Marc-Oliver Pahl'),
(3, '[HTML]<b>Suggestions/ complaints</b>:<br />\r\nWhat did you (dis-)like about this lab? Any improvements you would suggest? Found any errors?', 'This question is very important because the lab is done for those who do it and noone can find errors or make suggestions better than them...\r\nThe C r e d i t should be given if you can see that they thought about it at least for a second.', 0, 0, 1, 1, '2006-03-11 19:10:58: Marc-Oliver Pahl');
