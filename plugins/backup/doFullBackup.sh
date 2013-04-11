#!/bin/bash

qwe() {
        echo "${1:-ERROR}" >&1
        exit 1
}

[[ -n "$1"  ]] || qwe "doFullBackup [Folder]"

DBUSER="root"
DBPASSWD=""
FOLDER="$1"

MONTH="$(date '+%Y-%m')"
mkdir -p "${FOLDER}/FULL" || qwe "cannot create ${FOLDER}/FULL"

echo "Backing up all-databases to ${FOLDER}/FULL/$(date '+%F_%H-%M-%S').sql.gz."
mysqldump -u"$DBUSER" -p"$DBPASSWD" --all-databases | gzip > "${FOLDER}/FULL/$(date '+%F_%H-%M-%S').sql.gz"
