from flask import render_template, redirect, url_for, request, session
from playhouse.flask_utils import get_object_or_404

from entities import User, Team, UserForm, TeamForm
from app import app


@app.route("/user/create", methods=["GET", "POST"])
def user_create():
    form = UserForm(request.form)
    form.teams.choices = [(team.id, team.name) for team in Team.select().order_by('name')]

    if request.method == 'POST' and form.validate():
        user = User()
        form.populate_obj(user)
        user.save()
        return redirect(url_for('user_view', user_id=user.id))
    else:
        return render_template('users/new.html', form=form)


@app.route("/users/<int:user_id>", methods=["GET", "POST"])
def user_view(user_id):
    user = get_object_or_404(User.select(), User.id == user_id)

    return render_template('users/view.html', user=user)


@app.route("/users/<int:user_id>/edit", methods=["GET", "POST"])
def user_edit(user_id):
    user = get_object_or_404(User.select(), User.id == user_id)
    form = UserForm(request.form, user)
    form.teams.choices = [(team.id, team.name) for team in Team.select().order_by('name')]

    if request.method == 'POST' and form.validate():
        form.populate_obj(user)
        user.save()
        return redirect(url_for('user_view', user_id=user.id))
    else:
        return render_template('users/edit.html', form=form)


@app.route("/team/create", methods=["GET", "POST"])
def team_create():
    form = TeamForm(request.form)
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

    if request.method == 'POST' and form.validate():
        form.populate_obj(team)
        team.save()
        return redirect(url_for('team_view', team_id=team.id))
    else:
        return render_template('teams/edit.html', form=form)


@app.route("/login/<int:user_id>", methods=["GET", "POST"])
def login(user_id):
    session['user'] = user_id

    return redirect(url_for('user_view', user_id=user_id))
