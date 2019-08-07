#!/bin/bash
fileName="php-markdown-extra-1.2.6.zip"
wget http://littoral.michelf.ca/code/php-markdown/$fileName
unzip -jn $fileName
rm $fileName
