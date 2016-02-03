from flask import render_template, request, redirect, url_for

from app import app
from entities import load_element, LockError


@app.route("/courses/<course>/branches/<branch>/element/question/view/<path:path>")
def question_element_view(course, branch, path):
    element = load_element(course, branch, path)

    if element.meta['questionType'] == 'Text':
        return render_template('elements/question/view_text.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/question/edit/<path:path>", methods=["GET", "POST"])
def question_element_edit(course, branch, path):
    element = load_element(course, branch, path)

    pass


@app.route("/courses/<course>/branches/<branch>/element/question/delete/<path:path>")
def question_element_delete(course, branch, path):
    element = load_element(course, branch, path)
    element.delete()

    return redirect(url_for('collection_element_view', course=course, branch=branch, path=element.getParentPath()))


@app.route("/courses/<course>/branches/<branch>/element/question/answer/<path:path>", methods=["POST"])
def question_element_answer(course, branch, path):
    element = load_element(course, branch, path)

    if not element.isLocked():
        raise LockError('Answering without a lock')

    answer = element.getMyAnswer()
    answer.contents = request.form["answer"]
    answer.save()

    element.unlock()

    return redirect(request.args['back'])


@app.route("/courses/<course>/branches/<branch>/element/question/lock/<path:path>", methods=["POST"])
def question_element_lock(course, branch, path):
    element = load_element(course, branch, path)

    element.lock()

    return 'Locked'


@app.route("/courses/<course>/branches/<branch>/element/question/unlock/<path:path>", methods=["POST"])
def question_element_unlock(course, branch, path):
    element = load_element(course, branch, path)

    element.unlock()

    return 'Unlocked'


@app.route("/courses/<course>/branches/<branch>/element/question/correct/<path:path>")
def question_element_correct(course, branch, path):
    element = load_element(course, branch, path)
