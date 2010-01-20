In this directory the session data wil be stored.
It must be writable via php therefore.

Prevent access to this directory from the web.
If you have .htaccess enabled, this is automatically done.
If not set the access policies for this directory respectively.

You can check if your webserver configuration protects the
session files by entering the following URL in your
web browser:

http://[URL of your labsystem instance]/sessiondata/index.html

If you do not get an access forbidden warning 

    YOUR SESSION DATA IS NOT SAVE

People might steal your authentication token and alter data!

You have to change the access settings to the directory!!!