import os

from flask import render_template

from .element import Element


class CollectionElement(Element):
    def __init__(self, course, branch, path, meta=None):
        Element.__init__(self, course, branch, path, meta)

    def render(self, mode):
        if mode == 'self':
            return render_template("elements/collection/view.html", element=self)
        elif mode == 'course':
            return ''
        else:
            raise('Invalid rendering mode')

    def getChildren(self):
        from .element import load_element

        return map(lambda el: load_element(self.course, self.branch, os.path.join(self.path, el)), self.meta['children'])

    def removeChild(self, child):
        self.meta['children'].remove(child)
        self.save()

    def addChild(self, child):
        self.meta['children'].append(child)
