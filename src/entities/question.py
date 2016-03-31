import os
import yaml

from flask import render_template
from wtforms import Form, validators, TextAreaField, FormField, FieldList, SelectField, IntegerField, BooleanField
from wtforms.widgets import ListWidget
from flask_babel import lazy_gettext

import storage
from app import app

from .element import Element
from .answer import Answer
from .widgets import InputGroupWidget


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

    def getSecret(self):
        return yaml.load(storage.read(self.course, self.branch, os.path.join('secret', self.path + '.meta')))

    def saveSecret(self, data):
        yaml.dump(data, storage.write(self.course, self.branch, os.path.join('secret', self.path + '.meta')))

    def getQuestionDisplayElement(self):
        return self.getDisplayElement('Display')

    def getHintElement(self):
        return self.getDisplayElement('Hint')

    def getSectionContentElement(self, index):
        return self.getDisplayElement('Section-Content-' + str(index))

    def getSectionSecretElement(self, index):
        return self.getDisplayElement('Section-Secret-' + str(index))

    def getSectionElements(self):
        return [dict(content=self.getSectionContentElement(i), secret=self.getSectionSecretElement(i)) for i in range(self.meta['sectionCount'])]

    def getDisplayElement(self, name):
        from .helpers import load_element
        return load_element(self.course, self.branch, self.path + '-' + name)

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
        return self.getAssignment().meta['teamwork']


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

    def create(self):
        from .helpers import create_element
        displayElement = create_element(self.course, self.branch, self.path + '-Display', {'type': 'Display', 'displayType': 'HTML'})
        displayElement.save('Question')
        hintElement = create_element(self.course, self.branch, self.path + '-Hint', {'type': 'Display', 'displayType': 'HTML'})
        hintElement.save('Hint')

        self.meta['sectionCount'] = 0
        self.save()

        self.saveSecret({'credits': 0, 'sections': []})


class MiniDisplayForm(Form):
    content = TextAreaField(lazy_gettext('Content'), [validators.required()])
    displayType = SelectField(lazy_gettext('Type'), [validators.required()], choices=[('HTML', lazy_gettext('HTML')), ('Markdown', lazy_gettext('Markdown'))])


class SectionForm(Form):
    content = FormField(MiniDisplayForm, widget=InputGroupWidget)
    secret = FormField(MiniDisplayForm, widget=InputGroupWidget)
    credits = IntegerField(lazy_gettext('Credits'))
    remove = BooleanField(lazy_gettext('Remove'))


class TextQuestionForm(Form):
    display = FormField(MiniDisplayForm, widget=InputGroupWidget)
    hint = FormField(MiniDisplayForm, widget=InputGroupWidget)
    credits = IntegerField(lazy_gettext('Credits'))

    sections = FieldList(FormField(SectionForm, widget=ListWidget()), widget=ListWidget())


@app.context_processor
def register_shuffle_helper():
    def do_shuffle(data, permutation):
        return [data[idx] for idx in permutation]

    return dict(do_shuffle=do_shuffle)


class MultipleChoiceQuestionElement(QuestionElement):
    def __init__(self, course, branch, path, meta=None):
        QuestionElement.__init__(self, course, branch, path, meta)

    def render(self, context):
        from controllers import MultipleChoiceQuestionController
        controller = MultipleChoiceQuestionController(self)
        return render_template('elements/question/mc_render.html', **controller.renderParams())

    def getOptions(self):
        return [dict(content=self.getDisplayElement('Option-' + str(i)), hint=self.getDisplayElement('Option-Hint-' + str(i)), correctHint=self.getDisplayElement('Option-Correct-' + str(i))) for i in range(self.meta['optionCount'])]


class OptionForm(Form):
    content = FormField(MiniDisplayForm)
    hint = FormField(MiniDisplayForm)
    correctHint = FormField(MiniDisplayForm)
    remove = BooleanField(lazy_gettext('Remove'))
    isCorrect = BooleanField(lazy_gettext('Correct'))


class MultipleChoiceQuestionForm(Form):
    display = FormField(MiniDisplayForm)
    credits = IntegerField(lazy_gettext('Credits'))

    options = FieldList(FormField(OptionForm))

    shuffle = BooleanField(lazy_gettext('Shuffle'))
    shuffleHints = BooleanField(lazy_gettext('Shuffle hints'))
    singleChoice = BooleanField(lazy_gettext('Single Choice'))
    maxAllowedMistakes = IntegerField(lazy_gettext('Maximum allowed mistakes'))


def load_question_element(course, branch, path, meta):
    if meta['questionType'] == 'Text':
        return TextQuestionElement(course, branch, path, meta)
    elif meta['questionType'] == 'MultipleChoice':
        return MultipleChoiceQuestionElement(course, branch, path, meta)
    else:
        from .helpers import ElementYAMLError
        raise ElementYAMLError('Invalid questionType')
