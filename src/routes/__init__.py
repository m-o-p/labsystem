from .collection import collection_element_view
from .course import course_element_list, course_element_delete, course_element_view, course_element_create
from .display import display_element_view, display_element_edit, display_element_delete
from .element import element_view, element_edit, element_delete

__all__ = []

__all__.extend(globals())
