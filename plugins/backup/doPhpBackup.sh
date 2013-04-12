#!/bin/bash
while read nextInstance ; do
	if [ -n "$nextInstance" ] ; then
		echo "$nextInstance"
		#php5 ../../php/cliDoInstanceBackup.php "$nextInstance" "$1"
	fi
done <backupTheseInstances.txt
