import os
import yaml

from flask import render_template

import storage
from .element import Element, ElementYAMLError


class QuestionElement(Element):
    def __init__(self, course, branch, path, meta=None):
        Element.__init__(self, course, branch, path, meta)

    def loadCorrection(self):
        self.correction = yaml.load(storage.read(self.course, self.branch, os.path.join('secret', self.path + '.meta')))


class TextQuestionElement(QuestionElement):
    def __init__(self, course, branch, path, meta=None):
        QuestionElement.__init__(self, course, branch, path, meta)

    def render(self, context):
        if context == 'collection':
            return render_template('elements/question/text_view.html')
        elif context == 'correct':
            return render_template('elements/question/text_correct.html')
        else:
            raise 'Invalid rendering context'


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
