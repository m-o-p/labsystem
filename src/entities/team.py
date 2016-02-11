from peewee import CharField
from playhouse.fields import ManyToManyField
from wtforms import Form, validators
from wtforms import StringField, SelectField
from flask_babel import lazy_gettext

from app import database

from .user import User


class TeamForm(Form):
    name = StringField(lazy_gettext('Name'), [validators.required(), validators.length(min=4, max=20)])
    course = SelectField(lazy_gettext('Course'), [validators.required()])


class Team(database.Model):
    name = CharField(unique=True)
    users = ManyToManyField(User, related_name='teams')
    course = CharField()


TeamUser = Team.users.get_through_model()
