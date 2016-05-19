import os
from string import Template

import markdown
from wtforms import Form, StringField, TextAreaField, SelectField, validators
from flask_babel import lazy_gettext
from flask import escape, url_for

import storage

from .element import Element


class DisplayElement(Element):
    def __init__(self, course, branch, path, meta=None, isSecret=False):
        Element.__init__(self, course, branch, path, meta, isSecret=isSecret)

    def getRawPath(self):
        if self.isSecret:
            root = 'secret'
        else:
            root = 'content'

        return os.path.join(root, self.path + '.data')

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

    def getTemplateParams(self):
        return {
            'title': self.getTitle(),
            'fileroot': url_for('file_view', course=self.course, branch=self.branch, path='')
        }


class DisplayHTMLElement(DisplayElement):
    def __init__(self, course, branch, path, meta=None, isSecret=False):
        DisplayElement.__init__(self, course, branch, path, meta, isSecret=isSecret)

    def render(self, mode):
        if mode == 'headers':
            return ''

        t = Template(self.getRaw())

        return t.substitute(self.getTemplateParams())


class DisplayTextElement(DisplayElement):
    def __init__(self, course, branch, path, meta=None, isSecret=False):
        DisplayElement.__init__(self, course, branch, path, meta, isSecret=isSecret)

    def render(self, mode):
        if mode == 'headers':
            return ''

        t = Template(self.getRaw())

        return escape(t.substitute(self.getTemplateParams()))


class DisplayMarkdownElement(DisplayElement):
    def __init__(self, course, branch, path, meta=None, isSecret=False):
        DisplayElement.__init__(self, course, branch, path, meta, isSecret=isSecret)

    def render(self, mode):
        if mode == 'headers':
            return ''

        t = Template(self.getRaw())

        return markdown.markdown(t.substitute(self.getTemplateParams()))


def load_display_element(course, branch, path, meta, isSecret=False):
    if meta['displayType'] == 'HTML':
        return DisplayHTMLElement(course, branch, path, meta, isSecret=isSecret)
    elif meta['displayType'] == 'Markdown':
        return DisplayMarkdownElement(course, branch, path, meta, isSecret=isSecret)
    elif meta['displayType'] == 'Text':
        return DisplayTextElement(course, branch, path, meta, isSecret=isSecret)
    else:
        from .helpers import ElementYAMLError
        raise ElementYAMLError('Invalid displayType')


class DisplayForm(Form):
    type = SelectField(lazy_gettext('Type'), [validators.required()], choices=[('HTML', lazy_gettext('HTML')), ('Markdown', lazy_gettext('Markdown'))])
    path = StringField(lazy_gettext('Path'), [validators.required()])
    content = TextAreaField(lazy_gettext('Content'), [validators.required()])
