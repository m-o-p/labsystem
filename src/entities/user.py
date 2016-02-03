from peewee import CharField
from wtforms import Form, validators
from wtforms import StringField, PasswordField, SelectMultipleField
from flask_babel import lazy_gettext

from app import database


class UserForm(Form):
    username = StringField(lazy_gettext('Username'), [validators.required(), validators.length(min=4, max=20)])
    name = StringField(lazy_gettext('Full Name'), [validators.required(), validators.length(min=4, max=80)])
    email = StringField(lazy_gettext('Email Address'), [validators.required(), validators.Length(min=6, max=35)])
    password = PasswordField(lazy_gettext('Password'), [
        validators.length(min=6),
        validators.EqualTo('confirm', message=lazy_gettext('Passwords must match'))
    ])
    confirm = PasswordField(lazy_gettext('Repeat Password'))
    teams = SelectMultipleField(lazy_gettext('Teams'), coerce=int)


class User(database.Model):
    name = CharField(unique=True)
    username = CharField(unique=True)
    password = CharField(unique=True)
    email = CharField(unique=True)

    def getTeamForCourse(self, course):
        return self.teams[0]
