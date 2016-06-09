from flask import render_template, g, request, redirect, url_for

from app import app

from entities import load_element, checkPermissionForElement, AssignmentForm, create_element


@app.route("/courses/<course>/branches/<branch>/element/assignment/stats/<path:path>")
def assignment_element_stats(course, branch, path):
    """Show stats for an assignemnt"""
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'correct', element)

    return render_template('elements/assignment/stats.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/assignment/create", methods=["GET", "POST"])
def assignment_element_create(course, branch):
    form = AssignmentForm(request.form, path=request.args['path'])

    if request.method == 'POST' and form.validate():
        meta = {
            'children': [],
            'type': 'Assignment'
        }

        element = create_element(course, branch, form['path'].data, meta, addToParent=True)

        element.meta['showOnlyHeaders'] = form['showOnlyHeaders'].data
        element.meta['teamwork'] = form['teamwork'].data

        return redirect(url_for('element_view', course=course, branch=branch, path=form['path'].data))
    else:
        return render_template('elements/collection/create.html', form=form)


@app.route("/courses/<course>/branches/<branch>/element/assignment/edit/<path:path>", methods=["GET", "POST"])
def assignment_element_edit(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'edit', element)
    form = AssignmentForm(request.form, element, showOnlyHeaders=element.meta['showOnlyHeaders'], teamwork=element.meta['teamwork'])

    if request.method == 'POST' and form.validate():
        element.meta['showOnlyHeaders'] = form['showOnlyHeaders'].data
        element.meta['teamwork'] = form['teamwork'].data

        element.save()

        if form['path'].data != path:
            element.move(form['path'].data)

        return redirect(url_for('element_view', course=course, branch=branch, path=form['path'].data))
    else:
        return render_template('elements/collection/edit.html', form=form, element=element)
