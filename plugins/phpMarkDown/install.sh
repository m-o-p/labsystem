#!/bin/bash
fileName="php-markdown-extra-1.2.8.zip"
wget http://littoral.michelf.ca/code/php-markdown/$fileName
unzip -jn $fileName
rm $fileName
patch -p1 < patch_php_8_2026-03-31.diff
