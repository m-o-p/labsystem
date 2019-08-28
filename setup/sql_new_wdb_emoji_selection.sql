CREATE TABLE IF NOT EXISTS emoji_selections (
  'id' INT(32) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
  'emojiId' VARCHAR(32),
  'elemId' VARCHAR(32),
  'uid' VARCHAR(32),
  'history' VARCHAR(256)
);

