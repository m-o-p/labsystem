CREATE TABLE IF NOT EXISTS `multiple_choices` (
  `idx` bigint(20) NOT NULL auto_increment,
  `question` text,
  `answer_array` text,
  `answer_explanation` text,
  `correct_mask` bigint(20) default '0',
  `visible_for` bigint(20) default '0',
  `visible_only_in_collection` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=3 ;

INSERT INTO `multiple_choices` (`idx`, `question`, `answer_array`, `answer_explanation`, `correct_mask`, `visible_for`, `visible_only_in_collection`, `history`) VALUES
(1, 'Your question...\r\nYou will always have more inputs than answers below... (if all are filled press save and some more will appear).', 'a:3:{i:0;s:17:"answer1 (correct)";i:1;s:69:"[HTML]You can also use <b>html formattings <u>here</u>...</b> (wrong)";i:2;s:17:"answer3 (correct)";}', 'This will appear to the user after the question is (correctly or wrong) answered...', 5, 1, 1, '2006-03-11 19:10:58: Marc-Oliver Pahl'),
(2, 'This is a multiple choice question. Click on "give answers" to set the check marks. Don''t forget to "save"...', 'a:3:{i:0;s:46:"[HTML]This answer is <strong>correct</strong>.";i:1;s:21:"This answer is wrong.";i:2;s:46:"[HTML]This answer is <strong>correct</strong>.";}', 'Here you might find some remarks why the solution is correct.', 5, 1, 1, '2007-09-04 17:56:05: Marc-Oliver Pahl\n2007-09-03 20:04:37: Marc-Oliver Pahl\r\n2006-03-11 19:10:58: Marc-Oliver Pahl');

CREATE TABLE IF NOT EXISTS `bak_multiple_choices` (
  `idx` bigint(20) NOT NULL auto_increment,
  `question` text,
  `answer_array` text,
  `answer_explanation` text,
  `correct_mask` bigint(20) default '0',
  `visible_for` bigint(20) default '0',
  `visible_only_in_collection` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
);