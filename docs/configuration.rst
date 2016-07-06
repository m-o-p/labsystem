Configuration
=============

Firs time configuration
-----------------------

The project is designed to run inside an virtualenv environment. First we set up that environment::

  virtualenv env

Depencies are manged using pip::

  source ./env.sh
  pip install -r requirements.txt

The project is now set up. On first run a database is created and the default administrator is inserted with the username *admin* and the password *123456*

Configuration
-------------

The project has some configuration options in *settings.cfg*.

* *DATABASE* controls the database type and location. All tables are automatically generated if missing.
* *SECRET_KEY* must be changes to a unique string that will be used to encrypt cookies.
* *COURSES_DIR* is the location of the courses.
* *FILES_DIR* is the location of the uploaded files.
