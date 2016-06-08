import os

from flask import render_template

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
