from flask import render_template, request, redirect, url_for

from app import app
from entities import load_element


@app.route("/courses/<course>/branches/<branch>/element/question/view/<path:path>")
def question_element_view(course, branch, path):
    element = load_element(course, branch, path)

    return render_template('elements/display/view.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/question/edit/<path:path>", methods=["GET", "POST"])
def question_element_edit(course, branch, path):
    element = load_element(course, branch, path)

#   TODO checks

    if request.method == 'POST':
        element.save(request.form['content'])

#        TODO change title

        return redirect(url_for('display_element_view', course=course, branch=branch, path=path))
    else:
        return render_template('elements/display/edit.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/question/delete/<path:path>")
def question_element_delete(course, branch, path):
    element = load_element(course, branch, path)
    element.delete()

    return redirect(url_for('collection_element_view', course=course, branch=branch, path=element.getParentPath()))


@app.route("/courses/<course>/branches/<branch>/element/question/answer/<path:path>")
def question_element_answer(course, branch, path):
    element = load_element(course, branch, path)


@app.route("/courses/<course>/branches/<branch>/element/question/correct/<path:path>")
def question_element_correct(course, branch, path):
    element = load_element(course, branch, path)
