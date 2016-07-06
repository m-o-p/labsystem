Database
========

The database is used to store information that is specific to an instance.

Configuration
-------------

The database used by default is SQLite, but it can be changed in the configuration. When starting the application, it checks whether all the tables exist and creates them if required. The default data such as the default user is also inserted.

Users
-----

Users are always stored in a database. Each user can be assigned to teams for each individual course.

Permissions are role based. A role can have a number of permissions and is assigned to users. The assignment of roles can be restricted to courses, assignment and by a schedule.

Schedules
---------

Assignments and a users role can be restricted by a schedule. If the start or stop is missing from the schedule it is unlimited in that direction.

Answers
-------

User answers are stored in the database. MultipleChoice questions are automatically corrected and Text questions can be corrected by using the application.

Each *User* has a *Answer* for a specific question. When he answers a new *AnswerContent* is created and the information is stored within. The correction is also stored in *AnswerContent*. Text questions can also have a file attached.

The architecture was designed to allow MultipleChoice questions to be answerable by a team and to keep a history of answers.
