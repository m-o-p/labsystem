#!/bin/bash
#
# Add this to CRON if you want n daily backups for instance.
#
# for a full backup you could use:
# DBUSER="root"
# DBPASSWD=""
# DBHOST="localhost"
# FOLDER="/tmp"
# mkdir -p "${FOLDER}/FULL" || qwe "cannot create ${FOLDER}/FULL"
#
# echo "Backing up all-databases to ${FOLDER}/FULL/$(date '+%F_%H-%M-%S').sql.gz." >&1
# mysqldump -u"$DBUSER" -p"$DBPASSWD" -h"$DBHOST" --all-databases | gzip > "${FOLDER}/FULL/$(date '+%F_%H-%M-%S').sql.gz"
if [ $# -ne 1 ] ; then
	echo "Usage: $0 backupDirectory" >&2
	exit 1
fi
if ! type php5 >/dev/null 2>&1 ; then
	echo "No comman line interface for php5 found."
	exit 1
fi
cd "$( readlink -f "$( dirname "$0" )" )"
while read nextInstance ; do
	if [ -n "$nextInstance" ] ; then
		echo "Backing instance $nextInstance up:"
		( cd ./php/ && php5 cliDoInstanceBackup.php "$nextInstance" "$1" )
	fi
done <backup.txt
