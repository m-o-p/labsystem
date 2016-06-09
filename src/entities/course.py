import storage

from wtforms import Form, StringField, validators
from flask_babel import lazy_gettext

from .element import Element


class CourseElement(Element):
    def __init__(self, course, branch='master', meta=None):
        Element.__init__(self, course, branch, 'course', meta)

    def metaPath(self):
        return self.path + '.meta'

    def delete(self):
        storage.deleteCourse(self.course)

    def move(self, newpath):
        storage.renameCourse(self.course, self.newpath)

    def getChildren(self):
        from .helpers import load_element

        return map(lambda el: load_element(self.course, self.branch, el), self.meta['children'])

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

    def getCheckedOutBranches(self):
        return storage.listCheckedOutBranches(self.course)

    def getAvailableBranches(self):
        return storage.listAvailableBranches(self.course)

    def getUncheckedOutBranches(self):
        return list(set(self.getAvailableBranches() - set(self.getAvailableBranches())))

    def history(self, offset=0, limit=10):
        return storage.getHistory(self.course, self.branch, "", offset, limit)

    def getTitle(self):
        return self.course

    def getId(self):
        return ''


class CourseForm(Form):
    name = StringField(lazy_gettext('Name'), [validators.required()])
