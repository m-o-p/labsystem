from flask import render_template, redirect, url_for, request
from playhouse.flask_utils import get_object_or_404
from wtforms import Form, validators
from wtforms import StringField, PasswordField
from flask_babel import lazy_gettext, gettext

from app import app

from entities import Schedule, ScheduleForm


@app.route("/schedules/<int:schedule_id>", methods=["GET"])
def schedule_view(schedule_id):
    schedule = get_object_or_404(Schedule.select(), Schedule.id == schedule_id)

    return render_template('schedule/view.html', schedule=schedule)


@app.route("/schedules/<int:schedule_id>/edit", methods=["GET", "POST"])
def schedule_edit(schedule_id):
    schedule = get_object_or_404(Schedule.select(), Schedule.id == schedule_id)
    form = ScheduleForm(request.form, schedule)

    if request.method == 'POST' and form.validate():
        form.populate_obj(schedule)
        schedule.save()
        return redirect(url_for('schedule_view', schedule_id=schedule_id.id))
    else:
        return render_template('schedule/edit.html', form=form)


@app.route("/schedules/create", methods=["GET", "POST"])
def schedule_create():
    form = ScheduleForm(request.form)

    if request.method == 'POST' and form.validate():
        schedule = Schedule()
        form.populate_obj(schedule)
        schedule.save()
        return redirect(url_for('schedule_view', schedule_id=schedule.id))
    else:
        return render_template('schedule/new.html', form=form)
