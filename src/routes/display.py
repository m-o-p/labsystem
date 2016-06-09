from flask import render_template, request, redirect, url_for, g

from app import app
from entities import load_element, create_element, DisplayForm, checkPermissionForElement


@app.route("/courses/<course>/branches/<branch>/element/display/view/<path:path>")
def display_element_view(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'view', element)

    return render_template('elements/display/view.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/display/create", methods=["GET", "POST"])
def display_element_create(course, branch):
    form = DisplayForm(request.form, path=request.args['path'])

    if request.method == 'POST' and form.validate():
        meta = {
            'displayType': form['type'].data,
            'type': 'Display'
        }

        create_element(course, branch, form['path'].data, meta, content=form['content'].data, addToParent=True)

        return redirect(url_for('display_element_view', course=course, branch=branch, path=form['path'].data))
    else:
        return render_template('elements/display/create.html', form=form)


@app.route("/courses/<course>/branches/<branch>/element/display/edit/<path:path>", methods=["GET", "POST"])
def display_element_edit(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'edit', element)
    form = DisplayForm(request.form, element, content=element.getRaw(), type=element.meta['displayType'])

    if request.method == 'POST' and form.validate():
        element.meta['displayType'] = form['type'].data

        element.save(form['content'].data)

        if form['path'].data != path:
            element.move(form['path'].data)

        return redirect(url_for('display_element_view', course=course, branch=branch, path=form['path'].data))
    else:
        return render_template('elements/display/edit.html', form=form, element=element)


@app.route("/courses/<course>/branches/<branch>/element/display/delete/<path:path>")
def display_element_delete(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'edit', element)

    element.delete()

    return redirect(url_for('collection_element_view', course=course, branch=branch, path=element.getParentPath()))
