from peewee import ForeignKeyField, CharField

from app import database

from .user import User


class Team(database.Model):
    name = CharField(unique=True)


class TeamUser(database.Model):
    team = ForeignKeyField(Team, related_name='users')
    user = ForeignKeyField(User, related_name='teams')

Team.create_table(True)
TeamUser.create_table(True)
