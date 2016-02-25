from peewee import CharField, DateTimeField
from datetime import datetime

from app import database

from .collection import CollectionElement


class Schedule(database.Model):
    course = CharField()
    assignment = CharField()

    start_date = DateTimeField(default=datetime.now)
    end_time = DateTimeField(default=datetime.now)
    correct_time = DateTimeField(default=datetime.now)

    class Meta:
        indexes = (
            (('course', 'assignment'), True),
        )


class AssignmentElement(CollectionElement):
    def getAssignment(self):
        return self

    def getSchedule(self):
        return Schedule.get_or_create(
            course=self.course,
            assignment=self.path)
