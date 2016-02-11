from peewee import ForeignKeyField, CharField, TextField, DateTimeField
from datetime import datetime

from app import database

from .team import Team
from .user import User


class Answer(database.Model):
    """Contains the answer of a team for a specific question"""
    path = CharField()
    commit = CharField()
    course = CharField()

    team = ForeignKeyField(Team, null=True, related_name='answers')
    user = ForeignKeyField(User, null=True, related_name='answers')
    contents = TextField(null=True)
    correction = TextField(null=True)

    lock_user = ForeignKeyField(User, null=True, related_name='locks')
    lock_time = DateTimeField(null=True)

    class Meta:
        indexes = (
            (('user', 'team', 'course', 'commit', 'path'), True),
        )

    def isLocked(self):
        return self.lock_user is not None

    def lock(self, user):
        if self.lock_user is not None:
            from .question import AlreadyLockedError
            raise AlreadyLockedError(self)

        self.lock_user = user
        self.lock_time = datetime.now()

        self.save()

    def unlock(self, user):
        if self.lock_user.id != user.id:
            from .question import LockError
            raise LockError("Locked by other user")

        self.lock_user = None
        self.lock_time = None

        self.save()
