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


def load_element(course, branch, path, meta=None):
    """Get a specific element"""
    if meta is None:
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
        elif meta['type'] == 'Assignment':
            return AssignmentElement(course, branch, path, meta)
        else:
            raise ElementYAMLError('Invalid type')


def create_element(course, branch, path, meta, **kwargs):
    """Create a new element"""
    element = load_element(course, branch, path, meta)

    element.create(**kwargs)

    return element
