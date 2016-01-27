import storage

from .element import Element


class CourseElement(Element):
    def __init__(self, course, branch='master', meta=None):
        Element.__init__(self, course, branch, 'course', meta)

    def delete(self):
        Element.delete(self)

        storage.remoteCourse(self.course)
