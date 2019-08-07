#!/bin/bash
fileName="tinymce_3.5.8.zip"
wget http://github.com/downloads/tinymce/tinymce/$fileName
unzip -jn $fileName
rm $fileName
