In this folder you find shell scripts that you can use to make periodical
backups of your databases.

WE HIGHLY RECOMMEND YOU TO DO SO.

How does it work?
  1) Copy the scripts to some NON WEBSERVER readable place.
  2) Add your database user and your database password to the doBackup.sh and doFullBackup.sh.
  3) Adapt the backupSelection.sh to your databases that should be backuped.
     We usually backup only the databases of the current semester daily.
     The script automatically backs the full database up every 1st of the month.
  4) Add the backupSelection.sh to your cron to be executed daily (e.g. at 4am).

Then you have daily backups of the DBs you put into the backupSelection.sh
and you have monthly backups of your entire DB.
