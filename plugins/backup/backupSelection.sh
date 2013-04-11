#!/bin/sh

#  to be started by cron every night
#  #  
#
DESTINATIONFOLDER="data"
echo "Backup all necessary DBs..." >&2

if [ "$(date '+%d')" = 11 ]
then
        echo "Doing a full backup." >&2
	./doFullBackup.sh "$DESTINATIONFOLDER"
else
        echo "Doing a partial backup."

	# iLab1
	./doBackup.sh ilab_2013ss_data "$DESTINATIONFOLDER"
	./doBackup.sh ilab_2013ss_work "$DESTINATIONFOLDER"

	# iLab2
	./doBackup.sh ilab2_2013ss_data "$DESTINATIONFOLDER"
	./doBackup.sh ilab2_2013ss_work "$DESTINATIONFOLDER"

	# development instance
	./doBackup.sh ilab2_2013dev_data "$DESTINATIONFOLDER"
	./doBackup.sh ilab_2013_dev_data "$DESTINATIONFOLDER"

	# user database
	./doBackup.sh userdb "$DESTINATIONFOLDER"
fi
