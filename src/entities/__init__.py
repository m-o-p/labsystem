from .user import User, UserForm
from .team import Team, TeamUser, TeamForm
from .answer import Answer, AnswerContent, File
from .permission import Role, RoleForm, UserRole, UserRoleForm, Permission, RolePermission, checkPermissionForElement, checkPermissionForCourse, checkPermissionForSystem, PermissionDeniedError, InactiveAssignementError

from .element import Element
from .display import DisplayElement, DisplayHTMLElement, DisplayMarkdownElement, DisplayForm
from .question import QuestionElement, TextQuestionElement, MultipleChoiceQuestionElement, LockError, AlreadyLockedError, TextQuestionForm, MultipleChoiceQuestionForm
from .collection import CollectionElement, CollectionForm
from .assignment import AssignmentElement, Assignment, AssignmentForm
from .course import CourseElement, CourseForm
from .schedule import Schedule, ScheduleForm

from .helpers import load_element, create_element


def register_permissions():
    permissions = ['view', 'update', 'delete', 'schedule', 'correct', 'correct_open', 'view_secret', 'answer', 'view_outside_schedule',  # per assignment
                   'publish', 'branch', 'user_rights', 'view_scores', 'edit_scores',  # per course
                   'user_administration', 'create_course', 'clone_course', 'edit_schedule', 'admin']  # systemwide

    for permission in permissions:
        row = Permission(name=permission)
        row.save()


def register_default_user():
    user = User(name='admin', username='admin', password='admin', email='admin@localhost')
    user.save()

    role = Role(name='Admin')
    role.save()

    permission = Permission.get(Permission.name == 'admin').id

    RolePermission(role=role, permission=permission).save()

    UserRole(user=user, role=role).save()


def create_tables():
    """ Creates tables if they do not exist yet """
    Answer.create_table(True)
    AnswerContent.create_table(True)
    Team.create_table(True)
    TeamUser.create_table(True)
    Schedule.create_table(True)
    Assignment.create_table(True)
    Schedule.create_table(True)
    RolePermission.create_table(True)
    UserRole.create_table(True)
    Role.create_table(True)
    File.create_table(True)

    if not Permission.table_exists():
        Permission.create_table()
        register_permissions()

    if not User.table_exists():
        User.create_table()
        register_default_user()

AllDbEntities = [User, Team, TeamUser, Answer, AnswerContent, Role, UserRole, Permission, RolePermission, Schedule]

__all__ = []

__all__.extend(globals())
