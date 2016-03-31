from datetime import datetime

from peewee import DateTimeField, CharField
from wtforms import Form, validators
import wtforms
from flask_babel import lazy_gettext

from app import database


class ScheduleForm(Form):
    name = wtforms.StringField(lazy_gettext('Name'), [validators.required(), validators.length(min=4, max=20)])
    start = wtforms.DateTimeField(lazy_gettext('Start'), validators=[validators.Optional()])
    end = wtforms.DateTimeField(lazy_gettext('End'), validators=[validators.Optional()])


class Schedule(database.Model):
    name = CharField(unique=True)
    start = DateTimeField(null=True)
    end = DateTimeField(null=True)

    def isActive(self):
        now = datetime.now()

        if self.start is not None:
            if now < self.start:
                return False

        if self.end is not None:
            if now > self.end:
                return False

        return True
