from peewee import ForeignKeyField, CharField

from app import database

from .user import User


class Role(database.Model):
    name = CharField(unique=True)

    def can(self, permission):
        return self.permissions.where(RolePermission.permission == Permission.get(Permission.name == permission)).count() > 0


class Permission(database.Model):
    name = CharField(unique=True)


class RolePermission(database.Model):
    role = ForeignKeyField(Role, related_name='permissions')
    permission = ForeignKeyField(Permission, related_name='roles')


class UserRole(database.Model):
    role = ForeignKeyField(Role, related_name='users')
    user = ForeignKeyField(User, related_name='roles')

    course = CharField(null=True)
    assignment = CharField(null=True)
