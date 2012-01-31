#!/bin/bash
clear
echo "Welcome to the labsystem installation script."
echo ""
echo "This script will help you with the basic settings to get your labsystem instance running."
echo ""
echo "You will set up the databases and decide if you want to use SSL for login."
echo ""
echo "Press Enter to continue..."
read -s
clear

echo""
echo "---------------------------------------------------------------------------"
echo ">>>> Step 1: The session information is stored file based."
echo "(Where do you want to store the session information?                      )"
echo "(This directory has to be writable by PHP.                                )"
echo ""
setDataBaseField() $sess_save_path ../include/php_session_management.inc

echo""
echo "---------------------------------------------------------------------------"
echo ">>>> Step 1: We set the connection to the database up."
echo "(You can possibly connect to three different database servers.            )"
echo "(You can enter the same user name and password for all three databases.   )"
echo "(The user must have the rights to create and alter tables in the databases)"
echo "(and if you want the php setup script to create the databases for you he  )"
echo "(must also have the right to create databases. Otherwhise the databases   )"
echo "(you enter below must be created by you before running the setup script.  )"
echo ""

# Prompts for the value of field $1 in file $2 (hidden if $3 = -s)
setDataBaseField(){
  value="$(sed -rn "s/\s*$1\s*=\s*\"(\S+)\".*$/\1/p" $2)"
  echo -n "Please provide your $1 (enter for \"$value\"): "

  read $3 newValue
  # Password with silent input? -> Add a CR
  if [ "$3" == "-s" ]
  then
    echo ""
  fi
  if [ -n "$newValue" ]
  then 
    search="$(sed -rn "s/(\s*$1\s*=\s*\"\S*\".*)$/\1/p" $2)"
    replace="$(sed -rn "s/(\s*$1\s*=\s*\")\S*(\".*)$/\1$newValue\2/p" $2)"
    sed -i "s/$search/$replace/" $2
  fi
}

setDataBase(){
  for x in Name Host UserName; do setDataBaseField "$1$x" ../ini/configBase/defaultDatabases.ini; done
  setDataBaseField $1PassWord ../ini/configBase/defaultDatabases.ini -s
}

# make backup as file will be overwritten
cp ../ini/configBase/defaultDatabases.ini ../ini/configBase/defaultDatabases.ini.old
echo ""
echo "--------------------------------------------------------------------------------"
echo "| Working Database:"
echo "|  The working database contains the user answer, rights, etc."
echo "--------------------------------------------------------------------------------"
echo ""
setDataBase WorkingDatabase

echo ""
echo "--------------------------------------------------------------------------------"
echo "| Data Database:"
echo "|  The data database contains the lectures."                                        
echo "--------------------------------------------------------------------------------"                                                                       
echo ""
setDataBase DataDatabase

echo ""
echo "--------------------------------------------------------------------------------"
echo "| User Database:"
echo "|  The user database contains the users data (like user names, names, passwords)."                                                                                                               
echo "--------------------------------------------------------------------------------"
echo ""
setDataBase UserDatabase

echo ""
echo "Step 1/4 is done. Press Enter to continue..."
read -s
clear
echo "---------------------------------------------------------------------------"
echo ">>>> Step 2: Do you want to use SSL for login? (1=yes, 0=no)" 
echo "(It is highly recommended that you set this to 1. You need an SSL webserver)"
echo "(to be running at the same root the non https system does for this.        )"
echo""

cp ../ini/configBase/defaultAuthentication.ini ../ini/configBase/defaultAuthentication.ini.old
setDataBaseField SSLLogin ../ini/configBase/defaultAuthentication.ini

echo ""
echo "Step 2/4 is done. Press Enter to continue..."
read -s
clear
echo ""
echo "---------------------------------------------------------------------------"
echo ">>>> Step 3: We determine the Uid of php."
echo "(We assume a webserver is running on port 80 and try to determine its UID.)"
echo ""

echo "Determining web server UID..."
WWWPID="$(netstat -lpnt | sed -rn 's,^tcp6?\s+\w+\s+\w+\s+\S+:80\s+\S+\s+\w+\s+([0-9]+)/\w+\s*$,\1,p')"
[ $? == 0 ] || exit 1
[ -n "$WWWPID" -a "$WWWPID" -gt 0 ] || exit 2

WWWUID="$(sed -rn 's/^Uid:\s+\w+\s+(\w+)\s+\w+\s+\w+\s*$/\1/p' /proc/$WWWPID/status)"
echo "Uid of service on port 80: $WWWUID"

echo ""
echo "---------------------------------------------------------------------------"
echo ">>>> Step 4: We set some directories to be writable by php."
echo "(Files in these directories will be edited or created from within the php.)"
echo ""

echo "Changing the ownership of directories to the www-user..."
ownDirectoryByConfigVar(){
  ownDirectory $1 "$(sed -rn "s/\s*$1\s*=\s*\"(\S+)\".*$/\1/p" ../ini/configBase/defaultSystemLayout.ini)"
}

ownDirectory() {
  echo "$2 (configured in ini field $1)"
  mkdir -p $2
  chown -R $WWWUID $2
}

for x in UploadDirectory exportImportDir importPictureDir importFilesDir SystemResourcePath; do ownDirectoryByConfigVar "$x"; done
ownDirectory css ../css/

echo ""
echo "Step 4/4 is done. Press Enter to continue..."
read -s
clear
echo ""
echo "---------------------------------------------------------------------------"
echo ">>>> All Done." 
echo ""
echo "You are done now. Please browse to http://[yourHostNameAndPathToTheLabsystem]/setup?config=useradmin next."
echo ""
echo "Enjoy the system!"
echo ""
