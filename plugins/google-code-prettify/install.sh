#!/bin/bash
svn --force export http://google-code-prettify.googlecode.com/svn/trunk/ tmp/
mv tmp/src/*.* .
rm -rf tmp
