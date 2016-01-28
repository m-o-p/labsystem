from flask import render_template, url_for, redirect, request

from app import app
import storage
from entities import CourseElement, create_element


@app.route("/courses")
def course_element_list():
    courses = map(lambda el: CourseElement(el), storage.listCourses())

    return render_template('elements/course/list.html', courses=courses)


@app.route("/courses/<course>/delete")
def course_element_delete(course):
    course = CourseElement(course)

    course.delete()

    return redirect(url_for('course_element_list'))


@app.route("/courses/<course>/branches/<branch>")
def course_element_view(course, branch):
    course = CourseElement(course, branch)

    return render_template('elements/course/view.html', course=course)


@app.route("/course/create")
def course_element_create():
    courseName = request.form['course']
    storage.createCourse(courseName)

    create_element(courseName, 'master', 'course', {'type': 'Course'})

    return redirect(url_for('course_element_view', course=courseName, branch='master'))
