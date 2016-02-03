from peewee import ForeignKeyField, CharField, TextField, DateTimeField

from app import database

from .team import Team
from .user import User


class Answer(database.Model):
    """Contains the answer of a team for a specific question"""
    path = CharField()
    commit = CharField()
    course = CharField()

    team = ForeignKeyField(Team, related_name='answers')
    contents = TextField(null=True)
    correction = TextField(null=True)

    lock_user = ForeignKeyField(User, null=True)
    lock_time = DateTimeField(null=True)

    class Meta:
        indexes = (
            (('team', 'course', 'commit', 'path'), True),
        )
