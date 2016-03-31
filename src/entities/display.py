import os

import markdown
from wtforms import Form, StringField, TextAreaField, SelectField, validators
from flask_babel import lazy_gettext

import storage

from .element import Element


class DisplayElement(Element):
    def __init__(self, course, branch, path, meta=None):
        Element.__init__(self, course, branch, path, meta)

    def getRawPath(self):
        return os.path.join('content', self.path + '.data')

    def getRaw(self):
        return storage.read(self.course, self.branch, self.getRawPath()).read().decode()

    def save(self, content):
        Element.save(self)

        if content is not None:
            stream = storage.write(self.course, self.branch, self.getRawPath())
            stream.write(content)
            stream.close()

    def delete(self):
        DisplayElement.delete(self)

        return storage.delete(self.course, self.branch, self.getRawPath())

    def create(self, content=None):
        self.save(content)


class DisplayHTMLElement(DisplayElement):
    def __init__(self, course, branch, path, meta=None):
        DisplayElement.__init__(self, course, branch, path, meta)

    def render(self, mode):
        return self.getRaw()


class DisplayMarkdownElement(DisplayElement):
    def __init__(self, course, branch, path, meta=None):
        DisplayElement.__init__(self, course, branch, path, meta)

    def render(self, mode):
        return markdown.markdown(self.getRaw())


def load_display_element(course, branch, path, meta):
    if meta['displayType'] == 'HTML':
        return DisplayHTMLElement(course, branch, path, meta)
    elif meta['displayType'] == 'Markdown':
        return DisplayMarkdownElement(course, branch, path, meta)
    else:
        from .helpers import ElementYAMLError
        raise ElementYAMLError('Invalid displayType')


class DisplayForm(Form):
    type = SelectField(lazy_gettext('Type'), [validators.required()], choices=[('HTML', lazy_gettext('HTML')), ('Markdown', lazy_gettext('Markdown'))])
    path = StringField(lazy_gettext('Path'), [validators.required()])
    content = TextAreaField(lazy_gettext('Content'), [validators.required()])
