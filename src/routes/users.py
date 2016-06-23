from flask import render_template, redirect, url_for, request, session, flash
from playhouse.flask_utils import get_object_or_404
from wtforms import Form, validators
from wtforms import StringField, PasswordField
from flask_babel import lazy_gettext, gettext

from entities import User, Team, UserForm, TeamForm
from app import app
import storage


@app.context_processor
def inject_user():
    def getUser(id):
        return get_object_or_404(User.select(), User.id == id)
    return dict(getUser=getUser)


@app.route("/user/create", methods=["GET", "POST"])
def user_create():
    form = UserForm(request.form)
    del form.teams

    if request.method == 'POST' and form.validate():
        user = User()
        form.populate_obj(user)
        user.save()
        return redirect(url_for('user_view', user_id=user.id))
    else:
        return render_template('users/new.html', form=form)


@app.route("/users")
def user_list():
    users = User.select()

    return render_template('users/list.html', users=users)


@app.route("/users/<int:user_id>", methods=["GET", "POST"])
def user_view(user_id):
    user = get_object_or_404(User.select(), User.id == user_id)

    return render_template('users/view.html', user=user)


@app.route("/users/<int:user_id>/edit", methods=["GET", "POST"])
def user_edit(user_id):
    user = get_object_or_404(User.select(), User.id == user_id)
    form = UserForm(request.form, user)
    form.teams.choices = [(team.id, team.course + ' - ' + team.name) for team in Team.select().order_by('name')]

    if request.method == 'POST' and form.validate():
        form.populate_obj(user)
        user.save()
        return redirect(url_for('user_view', user_id=user.id))
    else:
        return render_template('users/edit.html', form=form)


@app.route("/team/create", methods=["GET", "POST"])
def team_create():
    form = TeamForm(request.form)
    form.course.choices = [(course, course) for course in storage.listCourses()]

    if request.method == 'POST' and form.validate():
        team = Team()
        form.populate_obj(team)
        team.save()
        return redirect(url_for('team_view', team_id=team.id))
    else:
        return render_template('teams/new.html', form=form)


@app.route("/teams/<int:team_id>", methods=["GET", "POST"])
def team_view(team_id):
    team = get_object_or_404(Team.select(), Team.id == team_id)

    return render_template('teams/view.html', team=team)


@app.route("/teams/<int:team_id>/edit", methods=["GET", "POST"])
def team_edit(team_id):
    team = get_object_or_404(Team.select(), Team.id == team_id)
    form = TeamForm(request.form, team)
    form.course.choices = [(course, course) for course in storage.listCourses()]

    if request.method == 'POST' and form.validate():
        form.populate_obj(team)
        team.save()
        return redirect(url_for('team_view', team_id=team.id))
    else:
        return render_template('teams/edit.html', form=form)


class LoginForm(Form):
    username = StringField(lazy_gettext('Username'), [validators.required(), validators.length(min=4, max=20)])
    password = PasswordField(lazy_gettext('Password'), [validators.required(), validators.length(min=4, max=20)])


@app.route("/login", methods=["GET", "POST"])
def login():
    form = LoginForm(request.form)

    if request.method == 'POST' and form.validate():
        user = User.select().where(User.username == form.username.data)

        try:
            user = user.get()
        except:
            flash(gettext('Invalid login'))
            return render_template('login.html', form=form)

        if user.password != form.password.data:
            flash(gettext('Invalid login'))
            return render_template('login.html', form=form)

        session['user'] = user.id

        return redirect(url_for('user_view', user_id=user.id))
    else:
        return render_template('login.html', form=form)


@app.route("/logout", methods=["POST"])
def logout():
    session.pop('user', None)

    return redirect(url_for('login'))
