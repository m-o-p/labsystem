from peewee import CharField
from wtforms import Form, validators
from wtforms import StringField, PasswordField, SelectMultipleField

from app import database


class UserForm(Form):
    name = StringField('Full Name', [validators.required(), validators.length(min=4, max=20)])
    email = StringField('Email Address', [validators.Length(min=6, max=35)])
    password = PasswordField('New Password', [
        validators.length(min=6),
        validators.EqualTo('confirm', message='Passwords must match')
    ])
    confirm = PasswordField('Repeat Password')
    teams = SelectMultipleField('Teams', coerce=int)


class User(database.Model):
    name = CharField(unique=True)
    password = CharField(unique=True)

    def getTeamForCourse(self, course):
        return self.teams[0]
