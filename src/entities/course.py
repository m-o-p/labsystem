import storage

from .element import Element


class CourseElement(Element):
    def __init__(self, course, branch='master', meta=None):
        Element.__init__(self, course, branch, 'course', meta)

    def metaPath(self):
        return self.path + '.meta'

    def delete(self):
        Element.delete(self)

        storage.remoteCourse(self.course)

    def getChildren(self):
        from .helpers import load_element

        return map(lambda el: load_element(self.course, self.branch, el), self.meta['children'])

    def removeChild(self, child):
        self.meta['children'].remove(child)
        self.save()

    def addChild(self, child):
        self.meta['children'].append(child)

    def getCheckedOutBranches(self):
        return storage.listCheckedOutBranches(self.course)

    def getAvailableBranches(self):
        return storage.listAvailableBranches(self.course)

    def getUncheckedOutBranches(self):
        return list(set(self.getAvailableBranches() - set(self.getAvailableBranches())))

    def history(self, offset=0, limit=10):
        return storage.getHistory(self.course, self.branch, "", offset, limit)

    def getAssignments(self):
        return sum(lambda el: el.getAssignments(), self.getChildren())
