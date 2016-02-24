from .user import User, UserForm
from .team import Team, TeamUser, TeamForm
from .answer import Answer, AnswerContent

from .element import Element, load_element, create_element
from .display import DisplayElement, DisplayHTMLElement, DisplayMarkdownElement, DisplayForm
from .question import QuestionElement, TextQuestionElement, MultipleChoiceQuestionElement, LockError, AlreadyLockedError
from .collection import CollectionElement
from .course import CourseElement


def create_tables():
    """ Creates tables if they do not exist yet """
    Answer.create_table(True)
    AnswerContent.create_table(True)
    Team.create_table(True)
    TeamUser.create_table(True)
    User.create_table(True)

__all__ = []

__all__.extend(globals())
