from peewee import ForeignKeyField, CharField, TextField

from app import database

from .team import Team

class Answer(database.Model):
    path = CharField()
    team = ForeignKeyField(Team, related_name='answers')
    contents = TextField()

Answer.create_table(True)
