from peewee import CharField, ForeignKeyField
from wtforms import BooleanField, validators
from flask_babel import lazy_gettext

from app import database

from .collection import CollectionElement, CollectionForm
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

    def create(self):
        self.meta['teamwork'] = False
        CollectionElement.create(self)

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
        asignments = filter(lambda el: el.meta['type'] == 'Asignment', self.getRecursiveChildren())
        return sum(lambda el: el.getAssignments(), asignments)

    def getCredits(self, questions=None):
        if questions is None:
            questions = self.getQuestions()

        credits = sum(map(lambda el: el.getCredits(), questions))

        return credits

    def getUserCredits(self, user, questions=None):
        if questions is None:
            questions = self.getQuestions()

        credits = map(lambda el: el.getUserCredits(user), questions)

        sum = 0
        for el in credits:
            if el is None:
                return
            else:
                sum += el

        return sum

    def getQuestions(self):
        return filter(lambda el: el.meta['type'] == 'Question', self.getRecursiveChildren())

    def getAssignment(self):
        return self


class AssignmentForm(CollectionForm):
    teamwork = BooleanField(lazy_gettext('Requires teamwork'))
