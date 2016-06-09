import os

from flask import render_template
from wtforms import Form, StringField, validators, BooleanField
from flask_babel import lazy_gettext

import storage

from .element import Element


class CollectionElement(Element):
    def __init__(self, course, branch, path, meta=None):
        Element.__init__(self, course, branch, path, meta)

    def render(self, mode):
        if mode == 'self':
            if self.meta['showOnlyHeaders']:
                return render_template("elements/collection/children.html", element=self)
            else:
                return render_template("elements/collection/headers.html", element=self)
        elif mode == 'course':
            return ''
        elif mode == 'collection':
            return render_template("elements/collection/headers.html", element=self)
        elif mode == 'headers':
            return render_template("elements/collection/headers.html", element=self)
        else:
            raise('Invalid rendering mode')

    def create(self):
        storage.createDirectory(self.course, self.branch, os.path.join('content', self.path))
        storage.createDirectory(self.course, self.branch, os.path.join('secret', self.path))

        self.meta['showOnlyHeaders'] = False

        self.save()

    def move(self, new):
        oldpath = self.path
        Element.move(self, new)
        newpath = self.path

        storage.rename(self.course, self.branch, oldpath, newpath)

    def getChildren(self):
        from .helpers import load_element

        return map(lambda el: load_element(self.course, self.branch, os.path.join(self.path, el)), self.meta['children'])

    def getRecursiveChildren(self):
        def process(el):
            if 'children' in el.meta:
                return el.getRecursiveChildren() + [el]
            else:
                return [el]

        tmp = map(process, self.getChildren())
        return [el for lst in tmp for el in lst]

    def removeChild(self, child):
        self.meta['children'].remove(child)
        self.save()

    def addChild(self, child):
        self.meta['children'].append(child)
        self.save()

    def moveChild(self, index, direction):
        if direction == "up":
            self.meta['children'][index - 1], self.meta['children'][index] = self.meta['children'][index], self.meta['children'][index - 1]
        elif direction == "down":
            self.meta['children'][index + 1], self.meta['children'][index] = self.meta['children'][index], self.meta['children'][index + 1]
        else:
            pass

        self.save()


class CollectionForm(Form):
    path = StringField(lazy_gettext('Path'), [validators.required()])
    showOnlyHeaders = BooleanField(lazy_gettext('Show Only Headers of Children'))
