from .user import User, UserForm
from .team import Team, TeamUser, TeamForm
from .answer import Answer, AnswerContent
from .permission import Role, UserRole, Permission, RolePermission
from .assignment import Schedule

from .element import Element
from .display import DisplayElement, DisplayHTMLElement, DisplayMarkdownElement, DisplayForm
from .question import QuestionElement, TextQuestionElement, MultipleChoiceQuestionElement, LockError, AlreadyLockedError
from .collection import CollectionElement
from .assignment import AssignmentElement
from .course import CourseElement

from .helpers import load_element, create_element


def register_permissions():
    permissions = ['view', 'update', 'answer', 'correct', 'manage_teams']

    for permission in permissions:
        Permission.create(name=permission)


def create_tables():
    """ Creates tables if they do not exist yet """
    Answer.create_table(True)
    AnswerContent.create_table(True)
    Team.create_table(True)
    TeamUser.create_table(True)
    User.create_table(True)
    Schedule.create_table(True)

    if not Permission.table_exists():
        Permission.create_table()

__all__ = []

__all__.extend(globals())
