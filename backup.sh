#!/bin/bash
#
# Add this to CRON if you want n daily backups for instance.
#
if ! type php5 >/dev/null 2>&1 ; then
	echo "No comman line interface for php5 found."
	exit 1
fi
cd "$( readlink -f "$( dirname "$0" )" )"
(
read directory || echo "The first line of backup.txt must contain the backup directory." >&2
while read nextInstance ; do
	if [ -n "$nextInstance" ] ; then
		echo "Backing instance $nextInstance up:"
		( cd ./php/ && php5 cliDoInstanceBackup.php "$nextInstance" "$directory" )
	fi
done
)<backup.txt
