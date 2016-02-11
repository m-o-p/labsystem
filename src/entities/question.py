import os
import yaml

from flask import render_template

import storage
from .element import Element, ElementYAMLError
from .answer import Answer


class LockError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


class AlreadyLockedError(LockError):
    def __init__(self, answer):
        self.answer = answer
        LockError.__init__(self, repr(self.answer))

    def getLocker(self):
        return self.lock_user

    def getLockTime(self):
        return self.lock_time


class QuestionElement(Element):
    def __init__(self, course, branch, path, meta=None):
        Element.__init__(self, course, branch, path, meta)

    def getCorrection(self):
        return yaml.load(storage.read(self.course, self.branch, os.path.join('secret', self.path + '.meta')))

    def getQuestionDisplayElement(self):
        from .element import load_element
        return load_element(self.course, self.branch, os.path.join(self.getParentPath(), self.meta['display']))

    def getDisplayElement(self, name):
        from .element import load_element
        return load_element(self.course, self.branch, os.path.join(self.getParentPath(), name))

    def getTeamAnswer(self, team):
        answer, created = Answer.get_or_create(
            team=team,
            course=self.course,
            commit=self.getCommit(),
            path=self.path)

        return answer

    def getUserAnswer(self, user):
        answer, created = Answer.get_or_create(
            user=user,
            course=self.course,
            commit=self.getCommit(),
            path=self.path)

        return answer

    def getAnswer(self, user):
        if self.needTeamAnswer():
            return self.getTeamAnswer(user.getTeamForCourse(self.course))
        else:
            return self.getUserAnswer(user)

    def needTeamAnswer(self):
        return True


class TextQuestionElement(QuestionElement):
    def __init__(self, course, branch, path, meta=None):
        QuestionElement.__init__(self, course, branch, path, meta)

    def render(self, context):
        if context == 'collection':
            return render_template('elements/question/text_render.html', element=self)
        elif context == 'correct':
            return render_template('elements/question/text_correct.html', element=self)
        else:
            return render_template('elements/question/text_render.html', element=self)


class MultipleChoiceQuestionElement(QuestionElement):
    def __init__(self, course, branch, path, meta=None):
        QuestionElement.__init__(self, course, branch, path, meta)


def load_question_element(course, branch, path, meta):
    if meta['questionType'] == 'Text':
        return TextQuestionElement(course, branch, path, meta)
    elif meta['questionType'] == 'MultipleChoice':
        return MultipleChoiceQuestionElement(course, branch, path, meta)
    else:
        raise ElementYAMLError('Invalid questionType')
