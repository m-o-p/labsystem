from flask import render_template, redirect, url_for, request
from playhouse.flask_utils import get_object_or_404

from entities import Role, Permission, RoleForm, UserRole, UserRoleForm, User, Schedule
from app import app

import storage


@app.route("/roles/<int:role_id>/edit", methods=["GET", "POST"])
def role_edit(role_id):
    role = get_object_or_404(Role.select(), Role.id == role_id)

    form = RoleForm(request.form, permissions=map(lambda el: el.id, role.permissions), name=role.name)
    form.permissions.choices = [(permission.id, permission.name) for permission in Permission.select()]

    if request.method == 'POST' and form.validate():
        form.populate_obj(role)
        role.save()
        return redirect(url_for('role_edit', role_id=role_id))
    else:
        return render_template('roles/edit.html', form=form)


@app.route("/roles/create", methods=["GET", "POST"])
def role_create():
    form = RoleForm(request.form)
    form.permissions.choices = [(permission.id, permission.name) for permission in Permission.select()]

    if request.method == 'POST' and form.validate():
        role = Role(name='Temporary')
        role.save()
        form.populate_obj(role)
        role.save()
        return redirect(url_for('role_edit', role_id=role.id))
    else:
        return render_template('roles/edit.html', form=form)


@app.route("/userrole/<int:role_id>/remove", methods=["GET", "POST"])
def userrole_remove(role_id):
    role = get_object_or_404(UserRole.select(), UserRole.id == role_id)
    user_id = role.user.id

    role.delete_instance()

    return redirect(url_for('user_view', user_id=user_id))


@app.route("/userrole/create", methods=["GET", "POST"])
def userrole_add():
    user_id = None

    if 'user_id' in request.args:
        user_id = request.args['user_id']

    form = UserRoleForm(request.form, user=user_id)

    form.user.choices = [(user.id, user.name) for user in User.select()]
    form.role.choices = [(role.id, role.name) for role in Role.select()]
    form.schedule.choices = [(schedule.id, schedule.name) for schedule in Schedule.select()]
    form.course.choices = [(course, course) for course in storage.listCourses()]

    form.course.choices.append(('', 'None'))
    form.schedule.choices.append((-1, 'None'))

    if request.method == 'POST' and form.validate():
        role = UserRole()

        if form.schedule.data == -1:
            form.schedule.data = None

        if form.course.data == '':
            form.schedule.data = None

        form.populate_obj(role)
        role.save()
        return redirect(url_for('user_view', user_id=form.user.data))
    else:
        return render_template('roles/edit.html', form=form)
