from datetime import datetime

from peewee import ForeignKeyField, CharField
from playhouse.fields import ManyToManyField
from wtforms import Form, validators
from wtforms import StringField, SelectMultipleField, SelectField
from flask_babel import lazy_gettext

from app import database

from .user import User
from .schedule import Schedule


class RoleForm(Form):
    name = StringField(lazy_gettext('Name'), [validators.required(), validators.length(min=4, max=20)])
    permissions = SelectMultipleField(lazy_gettext('Permissions'), coerce=int)


class Role(database.Model):
    name = CharField(unique=True)

    def can(self, permission):
        return self.permissions.where(Permission.name == permission).count() > 0


class Permission(database.Model):
    name = CharField(unique=True)

    roles = ManyToManyField(Role, related_name='permissions')


RolePermission = Permission.roles.get_through_model()


class UserRoleForm(Form):
    user = SelectField(lazy_gettext('User'), coerce=int)
    role = SelectField(lazy_gettext('Role'), coerce=int)

    course = SelectField(lazy_gettext('Course'))
    assignment = StringField(lazy_gettext('Assigment'))
    schedule = SelectField(lazy_gettext('Schedule'), coerce=int)


class UserRole(database.Model):
    role = ForeignKeyField(Role, related_name='users')
    user = ForeignKeyField(User, related_name='roles')

    course = CharField(null=True)
    assignment = CharField(null=True)
    schedule = ForeignKeyField(Schedule, related_name='roles', null=True)

    def can(self, permission):
        if self.schedule is not None:
            now = datetime.now()
            if self.schedule.start is not None:
                if self.schedule.start > now:
                    return False
            if self.schedule.end is not None:
                if self.schedule.end < now:
                    return False

        return self.role.can(permission)


class PermissionDeniedError(Exception):
    def __init__(self, user, permission, element=None, course=None):
        self.user = user
        self.permission = permission
        self.element = element
        self.course = course

    def __str__(self):
        return repr(self.permission)


class InactiveAssignementError(Exception):
    def __init__(self, user, permission, element=None, course=None):
        self.user = user
        self.permission = permission
        self.element = element
        self.course = course

    def __str__(self):
        return repr(self.permission)


def hasPermissionForSystem(user, permission):
    return user.can(permission)


def checkPermissionForSystem(user, permission):
    if not user.can(permission):
        raise PermissionDeniedError(user=user, permission=permission)


def hasPermissionForCourse(user, permission, course):
    return user.can(permission, course=course)


def checkPermissionForCourse(user, permission, course):
    if hasPermissionForSystem(user, 'admin'):
        return

    if not user.can(permission, course=course):
        raise PermissionDeniedError(user=user, permission=permission, course=course)


def checkPermissionForElement(user, permission, element):
    if hasPermissionForSystem(user, 'admin') or hasPermissionForCourse(user, 'admin', element.course):
        return

    assignment = element.getAssignment()

    if permission == 'view':
        if not assignment.canView():
            raise InactiveAssignementError(user=user, permission=permission, element=element)

    if permission == 'answer':
        if not assignment.canAnswer():
            raise InactiveAssignementError(user=user, permission=permission, element=element)

    if permission == 'correct':
        if not assignment.canCorrect():
            raise InactiveAssignementError(user=user, permission=permission, element=element)

    if not user.can(permission, course=element.course, assignment=assignment.path):
        raise PermissionDeniedError(user=user, permission=permission, element=element)
