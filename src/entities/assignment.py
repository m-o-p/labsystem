from peewee import CharField, ForeignKeyField

from app import database

from .collection import CollectionElement
from .schedule import Schedule


class Assignment(database.Model):
    course = CharField()
    assignment = CharField()

    view_schedule = ForeignKeyField(Schedule, related_name="assigment_views", null=True)
    answer_schedule = ForeignKeyField(Schedule, related_name="assigment_answers", null=True)
    correct_schedule = ForeignKeyField(Schedule, related_name="assigment_corrects", null=True)

    class Meta:
        indexes = (
            (('course', 'assignment'), True),
        )


class AssignmentElement(CollectionElement):
    def __init__(self, course, branch, path, meta=None, **kwargs):
        CollectionElement.__init__(self, course, branch, path, meta)

    def getDBData(self):
        assignment, created = Assignment.get_or_create(course=self.course, assignment=self.path)

        return assignment

    def canView(self):
        if self.getDBData().view_schedule is None:
            return True

        return self.view_schedule.isActive()

    def canAnswer(self):
        if self.getDBData().answer_schedule is None:
            return True

        return self.answer_schedule.isActive()

    def canCorrect(self):
        if self.getDBData().correct_schedule is None:
            return True

        return self.correct_schedule.isActive()

    def getAssignments(self):
        asignments = filter(lambda el: el.meta['type'] == 'Asignment', self.getChildren())
        return sum(lambda el: el.getAssignments(), asignments)

    def getAssignment(self):
        return self
