from flask import render_template, request, redirect, url_for, g
import yaml

from app import app
from entities import load_element, LockError, AnswerContent


@app.route("/courses/<course>/branches/<branch>/element/question/view/<path:path>")
def question_element_view(course, branch, path):
    element = load_element(course, branch, path)
    answer = element.getAnswer(g.user)

    if element.meta['questionType'] == 'Text':
        return render_template('elements/question/text_view.html', element=element, answer=answer)
    elif element.meta['questionType'] == 'MultipleChoice':
        element.setupShuffle(g.user)
        return render_template('elements/question/mc_view.html', element=element, answer=answer)


@app.route("/courses/<course>/branches/<branch>/element/question/edit/<path:path>", methods=["GET", "POST"])
def question_element_edit(course, branch, path):
    pass


@app.route("/courses/<course>/branches/<branch>/element/question/delete/<path:path>")
def question_element_delete(course, branch, path):
    element = load_element(course, branch, path)
    element.delete()

    return redirect(url_for('collection_element_view', course=course, branch=branch, path=element.getParentPath()))


@app.route("/courses/<course>/branches/<branch>/element/question/answer_text/<path:path>", methods=["POST"])
def text_question_element_answer(course, branch, path):
    element = load_element(course, branch, path)
    answer = element.getAnswer(g.user)

    if not answer.isLocked():
        raise LockError('Answering without a lock')

    answercontent = AnswerContent(answer=answer, content=request.form["answer"])
    answercontent.save()

    answer.unlock(g.user)

    return redirect(request.args['back'])


@app.route("/courses/<course>/branches/<branch>/element/question/answer_mc/<path:path>", methods=["POST"])
def mc_question_element_answer(course, branch, path):
    element = load_element(course, branch, path)
    answer = element.getAnswer(g.user)
    correction = element.getCorrection()

    answercontent = answer.getLatestCorrection()

    if answercontent.content is not None:
        content = yaml.load(answercontent.content)
    else:
        content = []

    content.append(["checkbox_" + str(i) in request.form for i in range(0, len(correction['options']))])

    answercontent.content = yaml.dump(content)
    answercontent.save()

    return redirect(request.args['back'])


@app.route("/courses/<course>/branches/<branch>/element/question/lock/<path:path>", methods=["POST"])
def question_element_lock(course, branch, path):
    element = load_element(course, branch, path)
    answer = element.getAnswer(g.user)

    answer.lock(g.user)

    return 'Locked'


@app.route("/courses/<course>/branches/<branch>/element/question/unlock/<path:path>", methods=["POST"])
def question_element_unlock(course, branch, path):
    element = load_element(course, branch, path)
    answer = element.getAnswer(g.user)

    answer.unlock(g.user)

    return 'Unlocked'


@app.route("/courses/<course>/branches/<branch>/element/question/correct/<path:path>", methods=["POST"])
def question_element_correct(course, branch, path):
    element = load_element(course, branch, path)
    answer = element.getAnswer(g.user)
    correction = element.getCorrection()
    answercontent = answer.getLatestContent()

    data = {
        'text': request.form['text'],
        'comment': request.form['comment'],
        'credits': int(request.form['credits']),
        'freeCredits': int(request.form['credits'])
    }

    for id, section in enumerate(correction['sections']):
        data['text-' + str(id)] = request.form['text-' + str(id)]
        data['check-' + str(id)] = 'check-' + str(id) in request.form

        if data['check-' + str(id)]:
            data['credits'] += section['credits']

    answercontent.correction = yaml.dump(data)
    answercontent.corrector = g.user

    answercontent.save()

    return redirect(request.args['back'])
