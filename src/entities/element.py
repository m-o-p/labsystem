import yaml
import os

import storage
from escape import unescapePath


class Element:
    """Base class for all elements"""
    def __init__(self, course, branch, path, meta=None, isSecret=False):
        self.course = course
        self.branch = branch
        self.path = path
        self.isSecret = isSecret

        if meta is not None:
            self.meta = meta
        else:
            self.meta = yaml.load(storage.read(self.course, self.branch, self.metaPath()))

    def metaPath(self):
        if self.isSecret:
            root = 'secret'
        else:
            root = 'content'

        return os.path.join(root, self.path + '.meta')

    def save(self):
        yaml.dump(self.meta, storage.write(self.course, self.branch, self.metaPath()))

    def delete(self):
        self.getParent().removeChild(self.getName())

        storage.delete(self.course, self.branch, self.metaPath())

    def move(self, new):
        if self.isSecret:
            root = 'secret'
        else:
            root = 'content'

        storage.rename(self.course, self.branch, self.metaPath(), os.path.join(root, new + '.meta'))

        self.getParent().removeChild(self.getName())
        self.path = new
        self.getParent().addChild(self.getName())

    def getTitle(self):
        (parent, me) = os.path.split(self.path)

        return unescapePath(me)

    def getParentPath(self):
        (parent, me) = os.path.split(self.path)

        return parent

    def getParent(self):
        (parent, me) = os.path.split(self.path)

        if parent is not None and parent is not '':
            from .helpers import load_element
            return load_element(self.course, self.branch, parent)
        else:
            from .course import CourseElement
            return CourseElement(self.course, self.branch)

    def getParentList(self):
        return self.path.split('/')

    def getName(self):
        (parent, me) = os.path.split(self.path)

        return me

    def getCommit(self):
        return str(next(storage.getHistory(self.course, self.branch, "", 0, 1)))

    def getAssignment(self):
        return self.getParent().getAssignment()

    def getId(self):
        (parent, me) = os.path.split(self.path)

        if parent is not None and parent != '':
            from .helpers import load_element
            parent = load_element(self.course, self.branch, parent)

            for index, child in enumerate(parent.meta['children']):
                if me == child:
                    return parent.getId() + str(index + 1) + '.'
        else:
            from .course import CourseElement
            for index, child in enumerate(CourseElement(self.course, self.branch).meta['children']):
                if me == child:
                    return str(index + 1) + '.'
