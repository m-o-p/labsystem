Configuration
=============

Firs time configuration
-----------------------

The installation requires python3, virtualenv and bower.

bower can be installed by using::

  npm install -g bower

The project is designed to run inside a dedicated environment. A setup script is provided to create it::

  ./setup.sh

The project is now set up. On first run a database is created and the default administrator is inserted with the username *admin* and the password *admin*

Configuration
-------------

The project has some configuration options in *settings.cfg*.

* *DATABASE* controls the database type and location. All tables are automatically generated if missing.
* *SECRET_KEY* must be changes to a unique string that will be used to encrypt cookies.
* *COURSES_DIR* is the location of the courses.
* *FILES_DIR* is the location of the uploaded files.
