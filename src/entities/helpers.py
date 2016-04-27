import yaml
import os

import storage

from .display import load_display_element
from .question import load_question_element
from .collection import CollectionElement
from .assignment import AssignmentElement


class ElementYAMLError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


def load_element(course, branch, path, meta=None, isSecret=False):
    """Get a specific element"""
    if isSecret:
        root = 'secret'
    else:
        root = 'content'

    if meta is None:
        meta = yaml.load(storage.read(course, branch, os.path.join(root, path + '.meta')))

    if 'isRedirect' in meta and meta['isRedirect']:
        return load_element(course, branch, meta.path, isSecret=isSecret)
    else:
        if meta['type'] == 'Display':
            return load_display_element(course, branch, path, meta, isSecret=isSecret)
        elif meta['type'] == 'Question':
            return load_question_element(course, branch, path, meta)
        elif meta['type'] == 'Collection':
            return CollectionElement(course, branch, path, meta)
        elif meta['type'] == 'Assignment':
            return AssignmentElement(course, branch, path, meta)
        else:
            raise ElementYAMLError('Invalid type')


def create_element(course, branch, path, meta, **kwargs):
    """Create a new element"""
    element = load_element(course, branch, path, meta)

    element.create(**kwargs)

    return element
