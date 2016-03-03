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

    lock_user = ForeignKeyField(User, null=True, related_name='locks')
    lock_time = DateTimeField(null=True)

    meta = CharField(null=True)

    def getLatestContent(self):
        return self.contents.order_by(AnswerContent.time.desc()).get()

    def hasContent(self):
        return self.contents.where(~ AnswerContent.content >> None).count() > 0

    def getLatestCorrection(self):
        return self.contents.order_by(AnswerContent.time.desc()).where(~ AnswerContent.correction >> None).get()

    def hasCorrection(self):
        return self.contents.where(~ AnswerContent.correction >> None).count() > 0

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


class AnswerContent(database.Model):
    answer = ForeignKeyField(Answer, 'contents')
    content = TextField(null=True)
    time = DateTimeField(default=datetime.now)

    correction = TextField(null=True)
    comment = TextField(null=True)
    corrector = ForeignKeyField(User, null=True, related_name='corrections')
