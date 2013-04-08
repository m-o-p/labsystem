CREATE TABLE IF NOT EXISTS `event_log` (
`idx` BIGINT NOT NULL AUTO_INCREMENT ,
`resourceID` VARCHAR( 255 ) NOT NULL ,
`referrerID` VARCHAR( 255 ) NOT NULL ,
`teamNr` INT NOT NULL ,
`userID` VARCHAR( 32 ) NOT NULL ,
`action` INT NOT NULL ,
`timestamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ,
PRIMARY KEY (  `idx` )
) ENGINE = MYISAM COMMENT =  'Saves the event log for the actions within the labsystem.';