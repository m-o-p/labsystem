CREATE TABLE IF NOT EXISTS `multiple_choices` (
  `idx` bigint(20) NOT NULL AUTO_INCREMENT,
  `question` text,
  `answer_array` text,
  `reaction_array` text,
  `answer_explanation` text,
  `correct_mask` bigint(20) DEFAULT '0',
  `possible_credits` int(11) NOT NULL DEFAULT '-1',
  `do_not_shuffle_answers` tinyint(1) DEFAULT '0',
  `only_one_answer_possible` tinyint(1) DEFAULT '0',
  `requires_n_correct` smallint(6) DEFAULT NULL COMMENT 'How many of the correct answers have to be clicked to grade this Q as correct?',
  `reactions` tinyint(1) DEFAULT '0',
  `show_hint_for_clicked` tinyint(1) DEFAULT '0',
  `reshow_last_hint` tinyint(1) DEFAULT '0',
  `detailed_grading` tinyint(1) DEFAULT '0',
  `horizontal_layout` tinyint(1) DEFAULT '0',
  `visible_for` bigint(20) DEFAULT '0',
  `visible_only_in_collection` tinyint(1) DEFAULT '0',
  `history` text,
  PRIMARY KEY (`idx`)
) AUTO_INCREMENT=3 ;

INSERT INTO `multiple_choices` (`idx`, `question`, `answer_array`, `answer_explanation`, `correct_mask`, `visible_for`, `visible_only_in_collection`, `history`) VALUES
(1, 'Your question...\r\nYou will always have more inputs than answers below... (if all are filled press save and some more will appear).', 'a:3:{i:0;s:17:"answer1 (correct)";i:1;s:69:"[HTML]You can also use <b>html formattings <u>here</u>...</b> (wrong)";i:2;s:17:"answer3 (correct)";}', 'This will appear to the user after the question is (correctly or wrong) answered...', 5, 1, 1, '2006-03-11 19:10:58: Marc-Oliver Pahl'),
(2, 'This is a multiple choice question. Click on "give answers" to set the check marks. Don''t forget to "save"...', 'a:3:{i:0;s:46:"[HTML]This answer is <strong>correct</strong>.";i:1;s:21:"This answer is wrong.";i:2;s:46:"[HTML]This answer is <strong>correct</strong>.";}', 'Here you might find some remarks why the solution is correct.', 5, 1, 1, '2007-09-04 17:56:05: Marc-Oliver Pahl\n2007-09-03 20:04:37: Marc-Oliver Pahl\r\n2006-03-11 19:10:58: Marc-Oliver Pahl');

CREATE TABLE IF NOT EXISTS `bak_multiple_choices` (
  `idx` bigint(20) NOT NULL AUTO_INCREMENT,
  `question` text,
  `answer_array` text,
  `reaction_array` text,
  `answer_explanation` text,
  `correct_mask` bigint(20) DEFAULT '0',
  `possible_credits` int(11) NOT NULL DEFAULT '-1',
  `do_not_shuffle_answers` tinyint(1) DEFAULT '0',
  `only_one_answer_possible` tinyint(1) DEFAULT '0',
  `requires_n_correct` smallint(6) DEFAULT NULL COMMENT 'How many of the correct answers have to be clicked to grade this Q as correct?',
  `reactions` tinyint(1) DEFAULT '0',
  `show_hint_for_clicked` tinyint(1) DEFAULT '0',
  `reshow_last_hint` tinyint(1) DEFAULT '0',
  `detailed_grading` tinyint(1) DEFAULT '0',
  `horizontal_layout` tinyint(1) DEFAULT '0',
  `visible_for` bigint(20) DEFAULT '0',
  `visible_only_in_collection` tinyint(1) DEFAULT '0',
  `history` text,
  PRIMARY KEY (`idx`)
);