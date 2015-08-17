#!/bin/bash
#
# Add this to CRON if you want n daily backups for instance.
#

BASENAME="$(readlink -f "$0")"
BASEDIR="$(dirname "$BASENAME")"
BASENAME="$(basename "$BASENAME")"

if ! type php5 >/dev/null 2>&1 ; then
	echo "No comman line interface for php5 found." >&2
	exit 1
fi

mapfile -t -u3 3<"${BASEDIR}/backup.txt"
if ! [ -n "${MAPFILE[0]}" -a -d  "${MAPFILE[0]}" ]; then
	echo  "The first line of backup.txt must contain the backup directory." >&2
	exit 1
fi
backupdir="${MAPFILE[0]}"
for ((i=1;i<${#MAPFILE[@]};i++)); do
	[ -n "${MAPFILE[i]}" ] || continue
	echo "Backing instance ${MAPFILE[i]} up:" >&2
	(
		if cd "${BASEDIR}/php"; then
			php5 "cliDoInstanceBackup.php" \
				"${MAPFILE[i]}" "$backupdir"
		else
			echo "ERROR: could not cd to ${BASEDIR}/php" >&2
		fi
	)
done
