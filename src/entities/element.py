import yaml
import os

import storage


class ElementYAMLError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


class Element:
    def __init__(self, course, branch, path, meta=None):
        self.course = course
        self.branch = branch
        self.path = path

        if meta is not None:
            self.meta = meta
        else:
            self.meta = yaml.load(storage.read(self.course, self.branch, self.metaPath()))

    def metaPath(self):
        return os.path.join('content', self.path + '.meta')

    def save(self):
        yaml.dump(self.meta, storage.write(self.course, self.branch, self.metaPath()))

    def delete(self):
        storage.delete(self.course, self.branch, self.metaPath())

        (parent, me) = os.path.split(self.path)

        if parent is not None:
            load_element(self.course, self.branch, parent).removeChild(me)

    def move(self, new):
        storage.rename(self.course, self.branch, self.metaPath(), os.path.join('content', self.getParentPath(), new + '.meta'))

    def getTitle(self):
        (parent, me) = os.path.split(self.path)

        return me

    def getParentPath(self):
        (parent, me) = os.path.split(self.path)

        return parent

    def getParent(self):
        (parent, me) = os.path.split(self.path)

        if parent is not None:
            load_element(self.course, self.branch, parent).removeChild(me)

from .display import load_display_element
from .question import load_question_element
from .collection import CollectionElement


def load_element(course, branch, path):
    meta = yaml.load(storage.read(course, branch, os.path.join('content', path + '.meta')))

    if 'isRedirect' in meta and meta['isRedirect']:
        return load_element(course, branch, meta.path)
    else:
        if meta['type'] == 'Display':
            return load_display_element(course, branch, path, meta)
        elif meta['type'] == 'Question':
            return load_question_element(course, branch, path, meta)
        elif meta['type'] == 'Collection':
            return CollectionElement(course, branch, path, meta)
        else:
            raise ElementYAMLError('Invalid type')


def create_element(course, branch, path, meta):
    yaml.dump(meta, storage.write(course, branch, os.path.join('content', path + '.meta')))

    (parent, me) = os.path.split(path)

    return load_element(course, branch, parent).addChild(me)
