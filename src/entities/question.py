from .element import Element, ElementYAMLError


class QuestionElement(Element):
    def __init__(self, course, branch, path, meta=None):
        Element.__init__(self, course, branch, path, meta)


class TextQuestionElement(QuestionElement):
    def __init__(self, course, branch, path, meta=None):
        QuestionElement.__init__(self, course, branch, path, meta)


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
