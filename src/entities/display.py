import os

import markdown
from wtforms import Form, StringField, TextAreaField, validators
from flask_babel import lazy_gettext

import storage

from .element import Element, ElementYAMLError


class DisplayElement(Element):
    def __init__(self, course, branch, path, meta=None):
        Element.__init__(self, course, branch, path, meta)


class DisplayHTMLElement(DisplayElement):
    def __init__(self, course, branch, path, meta=None):
        DisplayElement.__init__(self, course, branch, path, meta)

    def getRaw(self):
        return storage.read(self.course, self.branch, os.path.join('content', self.path + '.html')).read().decode()

    def save(self, content):
        Element.save(self)

        stream = storage.write(self.course, self.branch, os.path.join('content', self.path + '.html'))
        stream.write(content)
        stream.close()

    def delete(self):
        DisplayElement.delete(self)

        return storage.read(self.course, self.branch, os.path.join('content', self.path + '.html')).delete()

    def render(self, mode):
        return self.getRaw()


class DisplayMarkdownElement(DisplayElement):
    def __init__(self, course, branch, path, meta=None):
        DisplayElement.__init__(self, course, branch, path, meta)

    def getRaw(self):
        return storage.read(self.course, self.branch, os.path.join('content', self.path + '.md')).read().decode()

    def save(self, data):
        Element.save(self)

        stream = storage.write(self.course, self.branch, os.path.join('content', self.path + '.md'))
        stream.write(data)
        stream.close()

    def delete(self):
        DisplayElement.delete(self)

        return storage.read(self.course, self.branch, os.path.join('content', self.path + '.md')).delete()

    def render(self, mode):
        return markdown.markdown(self.getRaw())


def load_display_element(course, branch, path, meta):
    if meta['displayType'] == 'HTML':
        return DisplayHTMLElement(course, branch, path, meta)
    elif meta['displayType'] == 'Markdown':
        return DisplayMarkdownElement(course, branch, path, meta)
    else:
        raise ElementYAMLError('Invalid displayType')


class DisplayForm(Form):
    path = StringField(lazy_gettext('Path'), [validators.required()])
    content = TextAreaField(lazy_gettext('Content'), [validators.required()])
