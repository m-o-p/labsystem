from peewee import ForeignKeyField, CharField, TextField

from app import database

from .team import Team


class Answer(database.Model):
    """Contains the answer of a team for a specific question"""
    path = CharField()
    commit = CharField()
    course = CharField()
    team = ForeignKeyField(Team, related_name='answers')
    contents = TextField()
