from .collection import collection_element_view
from .course import course_element_list, course_element_delete, course_element_view, course_element_create
from .display import display_element_view, display_element_edit, display_element_delete
from .element import element_view, element_edit, element_delete
from .users import user_create, user_view, user_edit, team_create, team_view, team_edit
from .question import question_element_answer
from .language import set_language

__all__ = []

__all__.extend(globals())
