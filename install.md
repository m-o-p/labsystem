# How to INSTALL the labsystem.m-o-p.de

1) Download the archive.

2) `unzip labsystem.zip` in the webserver directory you want to have it
 	or copy the files from the archive on your webserver if you do not have
 	shell access.  
 	`unzip labsystem.zip -d webroot/`

3) Install [NPM](https://www.npmjs.com/) and [Composer](https://getcomposer.org/)

4) In the labsystem root directory run:  
   * `npm install`
   * `php composer.phar install` (requires php-cli to be installed)
   
5) To update JS and CSS files, run **after every** Labsytem update in the labsystem root directory:   
    `npx webpack`  
    The Labsystem uses [Webpack](https://webpack.js.org/) for bundling/merging static resources
   
## Editing file permissions
 	
3) Make the files
 	  * ini/menu_en_demo.ini
 	  * ini/menu_en_useradmin.ini
 	  * css/labsys_user_style.css
 	  * css/labsys_mop_ua_user_style.css
 	  
 	 writable by php.
 	 
 	 Even though it is not necessary for running the system it allows you to
 	 edit these files from inside the interface in a comfortable way.  
 	 Determine apache user: `cat /etc/passwd | grep www`  
 	 `chown wwwrun.www ini/en_menu_demo.ini`
 	 
4) Protect the following directories from web access  
 	(If your webserver allows the override of access permissions via .htaccess
 	files (apache configuration statement: `AllowOverride Limit`) you can SKIP THIS step)
 	* ini/
 	* sessiondata/
 	* include/
 	
5) Set the directory for the sessiondata appropriately in by editing the following
 	 line in *includes/php_session_management.inc*  
 	`$sess_save_path = "/tmp";`
 	
 	Set `/tmp` to `[your web dir]/sessiondata`.  
 	It might also work with /tmp but this is not recommended.
 	(e.g. `$sess_save_path = "/webdir/webroot/sessiondata";`)
 	MAKE SURE THIS PATH IS WRITABLE BY PHP!


## Preparing the Database

If you are the only one using the machine, you may leave the users
as they are, so 'root' without a password if you did not set a password for your mysql's root user.
Then you can skip this step.

If you want to create users for the databases continue reading this step.

The system uses three databases:
1) UserDatabase   
    Contains the users that will be able to logon your system.
    See *include/classes/DBInterfaceUser.inc* if you want to
    administer your users not within the database but use your
    LDAP+Kerberos for instance.
                        
2) DataDatabase  
    Contains the data that is supposed to be persistent for a longer time, like the instructions.
                        
3) WorkingDatabase  
    Contains the data that changes for each course like the user answers, credits etc.
                        
You can set up a different user for each database. This might be useful if you
want to control the read/ write permissions. You can do this by setting your
users access rights in mysql.

To enable the labsystem to use the tables and to create the necessary table 
structure, make sure, the users have the appropriate rights.

Set the account credentials (for the THREE dbs) in the config files:
* ini/config_demo.ini
* ini/config_useradmin.ini


## Runing the install routines
If you open the URL before setting the databases up, you get:  
`[Fatal error] SELECT * FROM pages WHERE idx='3' --> No database selected`
 	  
Open the install routines on your webserver by issuing the following URLs in your browser:
* [your system url and path]/setup?config=useradmin  	
    Scroll down and have a short look if everything went o.k.
    At the bottom you find a link to
* [your system url and path]/setup?config=demo  	
    Run the second setup.


## Email setup
The labsystem uses the php mail function to send e-mails.  It does not
(try to) set a specific envelope sender, so your sendmail is responsible
for doing that. The default will usually be something like www-data@FQDN.

For postfix, an easy way to set the envelope sender to what you want is to
add the following line to the main.cf:  
`sender_canonical_maps = hash:/etc/postfix/canonical`

and */etc/postfix/cannonical* should then contain a mapping to the desired
email address, e.g.:  
`www-data	envelope-sender@yourdomain.tld`



--------------------------------------------------------------------------

<p style="text-align:center;">DONE</p>

--------------------------------------------------------------------------


## How to GET STARTED with the labsystem.m-o-p.de

Go to your labsystem.m-o-p.de's instance now by following the link at the
bottom of the installation or typing your URL into the browser.

The start page tells you the installation runs.

1) Click on "log in". The first thing we do is changing the admin password.
 	Follow the link under the login to the useradmin configuration (red page).
 	
 	Log in at the user administration page. If you did a fresh installation
 	the login is "admin". The password is the same.
 	
 	Here You can change your contact details. The mail address provided here
 	will be used by the system to send you mails. Your students can do that
 	via the interface for instance.
 	
 	Provide your details. If you change the userName update the
 	* ini/config_demo.ini
 	* ini/config_useradmin.ini
 	  
 	to your new username by changing the following line in BOTH files:  
 	`RightsAdminUsername 	= "admin" ; admin -> yourNewLogin`
 	
 	The RightsAdmin user can automatically set the other user's rights.
 	As you get default access rights as set in the ini files  
 	`DefaultAuthUserRights 	  = "33"`
 	  
 	no one else can set rights at the beginning.
 	
 	Now as we changed our account we can log on the blue page again.
 	
2) Log in on the blue page with your new credentials.
 	If you do not see "user properties" in the menu now, you did not change
 	the "RightsAdminUsername" correctly.
 	
 	If you see "user properties" n the menu you can continue with the
 	instructions on the after login page...
 	
 	By clicking on the "First steps" menu you can always get back to this
 	instruction.
 	
 	Enjoy!


 	
## How to ADD USERS to labsystem.m-o-p.de
To add users log on the red page with your "RightsAdminUsername".

Go to "user properties" and give yourself the right "is able to administrate 
users at the uai" by clicking at the checkbox and saving.

Go to "myRights" in the menu then and apply all rights.

Then the menu "create users" appears. There you can add users by putting
a list of mail addresses in the field (one per row!).

Check the course IDs the users should be able to log on BEFORE clicking on
"Create users" (otherwise you have to add the courses at the "manage users"
page.

They must be able to log on "_ua" if they should be able to change their
credentials and details (recommended!).

Now the new users have the permission to log on the courses you enabled 
them. They will automatically get the rights and team specified in the
ini-file for the course:
```
; The following field specifies which rights logged in users have that are not specified otherwise.
 DefaultAuthUserRights 	 = "33"  ; IS_USER+IS_ROUNDMAIL_RECEIVER
                                 ; See ../include/user_roles.inc for possible values (sum up all those you want).
 DefaultAuthUserTeam 	 = "999" ; New users are in that team.
``` 
When your course starts you should make a proper team assignment as the
assignment affects the behaviour of the system (only if ALL team members
finished the prelab they see the lab, so as all are in the same team at
the beginning... also the team answers will be valid for all like this.)

Next you should send an email to all users via your course interface and
ask your participants to request a password on the user administration 
page, to log on there and change their credentials and AFTER that logging
on at the course page.


## How to CREATE NEW CONFIG on BASE OF an OLD one with labsystem.m-o-p.de

We assume your current config is *ini/config_2009ws.ini*  
with the menu *ini/menu_en_2009ws.ini*  
and the databases *ilab_2009ws_work* and *ilab_2009ws_data*.

You want to create a new instance for the upcoming semester 2010ss:

1) `copy ini/config_2009ws.ini ini/config_2010ss.ini`
2) `copy ini/menu_en_2009ws.ini ini/menu_en_2010ss.ini`  
 	change owner of ini/menu_en_2010ss.ini to [www-user].[www-user-group] (to make it editable via frontend)

3) edit ini/config_2010ss.ini  
   changes:
   ```
 	 SystemTitle                         = "ilab ws0910"            -> "ilab ss10"
 	 SystemMenuFile                      = "menu_en_2009ws.ini"     -> "menu_en_2010ss.ini"
 	 (OPTIONAL) UserStyleSheet           = "../css/labsys_user_style_ss06.css"
 	 WorkingDatabaseName                 = "ilab_2009ws_work"       -> "ilab_2010ss_work"
 	 DataDatabaseName                    = "ilab_2009ws_data"       -> "ilab_2010ss_data"
 	 User_courseID                       = "_ilab_2009ws"           -> "_ilab_2010ss"
 	 (OPTIONAL) RightsAdminUsername      = "mop"                    -> "newFirstAdmin"
   ```

4) (OPTIONAL) Change /index.php to get routed to the new course per default:  
 	`[line6] else $config = '2009ws'; 	 	 	 	 	  -> '2010ss'` 	 

5) If you want to replicate the data database (to do versioning):
 	(in mysql) `copy "ilab_2009ws_data" -> "ilab_2010ss_data"`
 	
6) Open [your system url and path]/setup?config=2010ss

7) Done.

You can log on the new instance now and edit the pages (e.g. start page).
Do not forget to give you the edit rights on the "user properties" page
first to make you an editor.

You can ADD USERS (see above) to the new course now.
