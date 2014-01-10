CREATE TABLE IF NOT EXISTS `input_answers` (
  `idx` bigint(20) NOT NULL auto_increment,
  `i_idx` bigint(20) default NULL,
  `team` int(9) default NULL,
  `answer` text,
  `comment` text,
  `given_credits` int(11) default NULL,
  `closed` tinyint(1) default '0',
  `corrected` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
) AUTO_INCREMENT=3 ;

INSERT INTO `input_answers` (`idx`, `i_idx`, `team`, `answer`, `comment`, `given_credits`, `closed`, `corrected`, `history`) VALUES
(1, 3, 55555, 'As corrector you can write a comment below.\r\nYou can give points.\r\nAnd you can leave the "isCorrected" field unchecked. So the Participant can not see the correction but your corrector collegues...', '', 0, 1, 0, '2007-09-04 18:09:15: The Tester: - closes input -\r\n2007-09-04 18:07:58: The Tester\r\n2007-09-04 18:06:52: The Tester: - created -\r\n'),
(2, 2, 55555, 'This is the answer of the Participant...\r\n\r\nShe can put in lots of things...\r\n\r\n10: br0: <BROADCAST,MULTICAST,UP,10000> mtu 1500 qdisc noqueue\r\n    link/ether 00:ff:65:77:89:94 brd ff:ff:ff:ff:ff:ff\r\n    inet 192.168.97.1/24 brd 192.168.97.255 scope global br0\r\n    inet6 2001:6f8:108c:97::1/64 scope global\r\n       valid_lft forever preferred_lft forever\r\n    inet6 fe80::2ff:65ff:fe77:8994/64 scope link\r\n       valid_lft forever preferred_lft forever\r\n11: tap2: <BROADCAST,MULTICAST,UP,10000> mtu 1500 qdisc pfifo_fast qlen 100\r\n    link/ether 00:ff:5b:19:8d:0c brd ff:ff:ff:ff:ff:ff\r\n    inet 192.168.99.5/24 brd 192.168.99.255 scope global tap2\r\n    inet6 2001:6f8:108c:99::5/64 scope global\r\n       valid_lft forever preferred_lft forever\r\n    inet6 fe80::2ff:5bff:fe19:8d0c/64 scope link\r\n       valid_lft forever preferred_lft forever\r\n', '', 0, 1, 0, '2007-09-04 18:09:15: The Tester: - closes input -\r\n2007-09-04 18:09:04: The Tester\r\n2007-09-04 18:08:19: The Tester: - created -\r\n');

CREATE TABLE IF NOT EXISTS `bak_input_answers` (
  `idx` bigint(20) NOT NULL auto_increment,
  `i_idx` bigint(20) default NULL,
  `team` int(9) default NULL,
  `answer` text,
  `comment` text,
  `given_credits` int(11) default NULL,
  `closed` tinyint(1) default '0',
  `corrected` tinyint(1) default '0',
  `history` text NOT NULL,
  PRIMARY KEY  (`idx`)
);