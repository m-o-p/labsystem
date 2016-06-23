from flask import render_template, url_for, redirect, request, g
from wtforms import Form, StringField, validators
from flask_babel import lazy_gettext

from app import app
import storage

from entities import CourseElement, checkPermissionForElement, CourseForm


@app.route("/courses")
def course_element_list():
    """Show the list of courses"""
    courses = map(lambda el: CourseElement(el), storage.listCourses())

    return render_template('elements/course/list.html', courses=courses)


@app.context_processor
def inject_getCourses():
    def getCourses():
        return map(lambda el: CourseElement(el), storage.listCourses())
    return dict(getCourses=getCourses)


@app.route("/courses/<course>/delete")
def course_element_delete(course):
    """Delete a course"""
    course = CourseElement(course)
    checkPermissionForElement(g.user, 'edit', course)

    course.delete()

    return redirect(url_for('course_element_list'))


@app.route("/courses/<course>/branches/<branch>")
def course_element_view(course, branch):
    """Show a course"""
    course = CourseElement(course, branch)
    checkPermissionForElement(g.user, 'view', course)

    return render_template('elements/course/view.html', element=course)


@app.route("/course/create", methods=["GET", "POST"])
def course_element_create():
    form = CourseForm(request.form)

    if request.method == 'POST' and form.validate():
        storage.createCourse(form['name'].data)

        return redirect(url_for('course_element_view', course=form['name'].data, branch='master'))
    else:
        return render_template('elements/course/new.html', form=form)


@app.route("/course/<course>/branches/<branch>/edit", methods=["GET", "POST"])
def course_element_edit(course, branch):
    element = CourseElement(course, branch)
    form = CourseForm(request.form, name=course)

    if request.method == 'POST' and form.validate():
        if form['name'].data != course:
            element.move(form['name'].data)

        return redirect(url_for('course_element_view', course=course, branch=form['name'].data))
    else:

        return render_template('elements/course/edit.html', form=form, element=element, is_dirty=storage.isRepoDirty(course, branch))


@app.route("/courses/<course>/branches/<branch>", methods=["POST"])
def course_element_branch(course, branch):
    """Create a new branch of a course"""
    branchName = request.form['branch']
    storage.createBranch(course, branch, branchName)

    return redirect(url_for('course_element_view', course=course, branch=branchName))


class CourseCommitForm(Form):
    message = StringField(lazy_gettext('Message'), [validators.required()])


@app.route("/course/<course>/branches/<branch>/commit", methods=["GET", "POST"])
def course_commit(course, branch):
    element = CourseElement(course, branch)
    form = CourseCommitForm(request.form, name=course)

    if request.method == 'POST' and form.validate():
        storage.commit(course, branch, form.message.data, g.user)

        return redirect(url_for('course_element_view', course=course, branch=branch))
    else:

        return render_template('elements/course/commit.html', form=form, element=element, is_dirty=storage.isRepoDirty(course, branch))


@app.route("/course/<course>/branches/<branch>/merge", methods=["POST"])
def course_merge(course, branch):
    storage.mergeBranches(course, branch, request.form['source'])

    return redirect(url_for('course_element_view', course=course, branch=branch))
