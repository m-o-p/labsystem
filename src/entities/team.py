from peewee import CharField
from playhouse.fields import ManyToManyField
from wtforms import Form, validators
from wtforms import StringField

from app import database

from .user import User


class TeamForm(Form):
    name = StringField('Name', [validators.required(), validators.length(min=4, max=20)])


class Team(database.Model):
    name = CharField(unique=True)
    users = ManyToManyField(User, related_name='teams')


TeamUser = Team.users.get_through_model()
