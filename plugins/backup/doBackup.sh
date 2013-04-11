#!/bin/bash

qwe() {
                echo "${1:-ERROR}" >&2
                exit 1  
        }       

[[ -n "$2"  ]] || qwe "doBackup [DBName] [Folder]"

DBNAME="$1"
FOLDER="$2"
DBUSER="root"
DBPASSWD=""

MONTH="$(date '+%Y-%m')"
mkdir -p "${FOLDER}/${MONTH}" || qwe "cannot create ${FOLDER}/${MONTH}"

echo "Backing up $DBNAME to ${FOLDER}/${MONTH}/$(date '+%F_%H-%M-%S')_${DBNAME}.sql.gz."
mysqldump -u"$DBUSER" -p"$DBPASSWD" "$DBNAME" | gzip > "${FOLDER}/${MONTH}/$(date '+%F_%H-%M-%S')_${DBNAME}.sql.gz"
