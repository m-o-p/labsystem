In this directory the session data wil be stored.
It must be writable via php therefore.

Prevent access to this directory from the web.
If you have .htaccess enabled, this is automatically done.
If not set the access policies for this directory respectively.

You can check if your webserver configuration protects the
configuration files by entering the following URL in your
web browser:

  http://[URL of your labsystem instance]/ini/config_demo.ini

If you see the config file YOUR PASSWORDS ARE VISIBLE to evvery-
one and you have to change the access settings to the directory!!!