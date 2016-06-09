from flask import render_template, request, redirect, url_for, g, make_response
from playhouse.flask_utils import get_object_or_404
import yaml

from app import app
from entities import load_element, LockError, AnswerContent, checkPermissionForElement, TextQuestionForm, create_element, MultipleChoiceQuestionForm, File
from controllers import MultipleChoiceQuestionController


@app.route("/courses/<course>/branches/<branch>/element/question/view/<path:path>")
def question_element_view(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'view', element)

    if element.meta['questionType'] == 'Text':
        answer = element.getAnswer(g.user)
        return render_template('elements/question/text_view.html', element=element, answer=answer)
    elif element.meta['questionType'] == 'MultipleChoice':
        return render_template('elements/question/mc_view.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/question/text/edit/<path:path>", methods=["GET", "POST"])
def question_element_edit(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'edit', element)
    secret = element.getSecret()

    sections = element.getSectionElements()

    for index, section in enumerate(sections):
        section['content'] = dict(content=section['content'].getRaw(), displayType=section['content'].meta['displayType'])
        section['secret'] = dict(content=section['secret'].getRaw(), displayType=section['secret'].meta['displayType'])
        section['credits'] = secret['sections'][index]

    questionElement = element.getQuestionDisplayElement()
    hintElement = element.getHintElement()

    form = TextQuestionForm(request.form, element,
                            credits=secret['credits'],
                            display=dict(content=questionElement.getRaw(), displayType=questionElement.meta['displayType']),
                            hint=dict(content=hintElement.getRaw(), displayType=hintElement.meta['displayType']),
                            sections=sections)

    form.sections.append_entry(dict(remove=True, credits=0, secret=dict(displayType='HTML', content='-'), content=dict(displayType='HTML', content='-')))

    if request.method == 'POST' and form.validate():
        hintElement.meta['displayType'] = form.hint.displayType.data
        hintElement.save(form.hint.content.data)

        questionElement.meta['displayType'] = form.display.displayType.data
        questionElement.save(form.display.content.data)

        secret['credits'] = form.credits.data

        index = 0
        for entry in form.sections.entries:
            if entry.remove.data is True:
                continue

            entryContentElement = create_element(course, branch, path + '-Section-Content-' + str(index), {'type': 'Display', 'displayType': entry.content.displayType.data})
            entryContentElement.save(entry.content.content.data)

            entrySecretElement = create_element(course, branch, path + '-Section-Secret-' + str(index), {'type': 'Display', 'displayType': entry.secret.displayType.data})
            entrySecretElement.save(entry.secret.content.data)

            if index >= len(secret['sections']):
                secret['sections'].append(entry.credits.data)
            else:
                secret['sections'][index] = entry.credits.data

            index = index + 1

        element.saveSecret(secret)

        element.meta['sectionCount'] = index
        element.save()

        return redirect(url_for('question_element_view', course=course, branch=branch, path=path))
    else:
        return render_template('elements/question/text_edit.html', form=form, element=element)


@app.route("/courses/<course>/branches/<branch>/element/question/mc/edit/<path:path>", methods=["GET", "POST"])
def mc_element_edit(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'edit', element)
    secret = element.getSecret()

    options = element.getOptions()

    for index, option in enumerate(options):
        option['content'] = dict(content=option['content'].getRaw(), displayType=option['content'].meta['displayType'])
        option['hint'] = dict(content=option['hint'].getRaw(), displayType=option['hint'].meta['displayType'])
        option['correctHint'] = dict(content=option['correctHint'].getRaw(), displayType=option['correctHint'].meta['displayType'])
        option['isCorrect'] = secret['options'][index]
        option['remove'] = False

    roundHints = element.getRoundHints()

    questionElement = element.getQuestionDisplayElement()

    form = MultipleChoiceQuestionForm(request.form, element,
                                      credits=secret['credits'],
                                      display=dict(content=questionElement.getRaw(), displayType=questionElement.meta['displayType']),
                                      shuffle=element.meta['shuffle'],
                                      shuffleHints=element.meta['shuffleHints'],
                                      singleChoice=element.meta['singleChoice'],
                                      maxAllowedMistakes=element.meta['maxAllowedMistakes'],
                                      maxAllowedAnswers=element.meta['maxAllowedAnswers'],
                                      roundHints=roundHints,
                                      options=options)

    form.options.append_entry(dict(remove=True, content=dict(displayType='HTML', content='-'), hint=dict(displayType='HTML', content='-'), correctHint=dict(displayType='HTML', content='-'), isCorrect=False))
    form.roundHints.append_entry(dict(displayType='HTML', content='-', remove=True))

    if request.method == 'POST' and form.validate():
        questionElement.meta['displayType'] = form.display.displayType.data
        questionElement.save(form.display.content.data)

        secret['credits'] = form.credits.data

        index = 0
        for entry in form.options.entries:
            if entry.remove.data is True:
                continue

            entryContentElement = create_element(course, branch, path + '-Option-' + str(index), {'type': 'Display', 'displayType': entry.content.displayType.data})
            entryContentElement.save(entry.content.content.data)

            entryHintElement = create_element(course, branch, path + '-Option-Hint-' + str(index), {'type': 'Display', 'displayType': entry.hint.displayType.data})
            entryHintElement.save(entry.hint.content.data)

            entryCorrectElement = create_element(course, branch, path + '-Option-Correct-' + str(index), {'type': 'Display', 'displayType': entry.correctHint.displayType.data})
            entryCorrectElement.save(entry.correctHint.content.data)

            if index >= len(secret['options']):
                secret['options'].append(entry.isCorrect.data)
            else:
                secret['options'][index] = entry.isCorrect.data

            index = index + 1

        roundHintIndex = 0
        for entry in form.roundHints.entries:
            if entry.remove.data is True:
                continue

            roundHintElement = create_element(course, branch, path + '-RoundHint-' + str(roundHintIndex), {'type': 'Display', 'displayType': entry.displayType.data})
            roundHintElement.save(entry.content.data)

            roundHintIndex = roundHintIndex + 1

        secret['roundHintCount'] = roundHintIndex
        element.saveSecret(secret)

        element.meta['shuffle'] = form.shuffle.data
        element.meta['shuffleHints'] = form.shuffleHints.data
        element.meta['singleChoice'] = form.singleChoice.data
        element.meta['maxAllowedMistakes'] = form.maxAllowedMistakes.data
        element.meta['maxAllowedAnswers'] = form.maxAllowedAnswers.data

        element.meta['optionCount'] = index
        element.save()

        return redirect(url_for('question_element_view', course=course, branch=branch, path=path))
    else:
        return render_template('elements/question/mc_edit.html', form=form, element=element)


@app.route("/courses/<course>/branches/<branch>/element/question/<type>/create", methods=["GET", "POST"])
def question_element_create(course, branch, type):
    if 'path' in request.form:
        create_element(course, branch, request.form['path'], {'type': 'Question', 'questionType': type}, addToParent=True)

        return redirect(url_for('element_edit', course=course, branch=branch, path=request.form['path']))
    else:
        return render_template('elements/question/create.html', path=request.args['path'])


@app.route("/courses/<course>/branches/<branch>/element/question/delete/<path:path>")
def question_element_delete(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'edit', element)
    element.delete()

    return redirect(url_for('collection_element_view', course=course, branch=branch, path=element.getParentPath()))


@app.route("/courses/<course>/branches/<branch>/element/question/answer_text/<path:path>", methods=["POST"])
def text_question_element_answer(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'answer', element)
    answer = element.getAnswer(g.user)

    if not answer.isLocked():
        raise LockError('Answering without a lock')

    file = None

    if element.meta['hasFileUpload']:
        fileData = request.files['file']
        file = File(name=fileData.filename, uploader=g.user)
        file.save()
        fileData.save(file.getPath())

    answercontent = AnswerContent(answer=answer, content=request.form["answer"], uploadedFile=file)
    answercontent.save()

    answer.unlock(g.user)

    return redirect(request.args['back'])


@app.route("/files/<int:file_id>")
def uploaded_file_get(file_id):
    file = get_object_or_404(File.select(), File.id == file_id)

    response = make_response(file.loadData())

    response.headers["Content-Disposition"] = "attachment; filename=" + file.name

    return response


@app.route("/courses/<course>/branches/<branch>/element/question/answer_mc/<path:path>", methods=["POST"])
def mc_question_element_answer(course, branch, path):
    controller = MultipleChoiceQuestionController.fromParams(course, branch, path)
    checkPermissionForElement(g.user, 'answer', controller.element)

    if controller.element.meta['singleChoice']:
        answers = [str(i) == request.form['answer'] for i in range(0, controller.element.meta['optionCount'])]
    else:
        answers = ["checkbox_" + str(i) in request.form for i in range(0, controller.element.meta['optionCount'])]

    controller.processAnswer(answers)

    return redirect(request.args['back'])


@app.route("/courses/<course>/branches/<branch>/element/question/lock/<path:path>", methods=["POST"])
def question_element_lock(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'answer', element)
    answer = element.getAnswer(g.user)

    answer.lock(g.user)

    return 'Locked'


@app.route("/courses/<course>/branches/<branch>/element/question/unlock/<path:path>", methods=["POST"])
def question_element_unlock(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'answer', element)
    answer = element.getAnswer(g.user)

    answer.unlock(g.user)

    return 'Unlocked'


@app.route("/courses/<course>/branches/<branch>/element/question/correct/<path:path>", methods=["POST"])
def question_element_correct(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'correct', element)
    answer = element.getAnswer(g.user)
    secret = element.getSecret()
    answercontent = answer.getLatestContent()

    data = {
        'text': request.form['text'],
        'comment': request.form['comment'],
        'credits': int(request.form['credits']),
        'freeCredits': int(request.form['credits'])
    }

    for id, credits in enumerate(secret['sections']):
        data['text-' + str(id)] = request.form['text-' + str(id)]
        data['check-' + str(id)] = 'check-' + str(id) in request.form

        if data['check-' + str(id)]:
            data['credits'] += credits

    answercontent.correction = yaml.dump(data)
    answercontent.corrector = g.user

    answercontent.save()

    return redirect(request.args['back'])
