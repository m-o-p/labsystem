from peewee import CharField

from app import database


class User(database.Model):
    name = CharField(unique=True)
