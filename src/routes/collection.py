from flask import render_template, g, redirect, url_for, request

from app import app
import storage

from entities import load_element, checkPermissionForElement, CollectionForm, create_element


@app.route("/courses/<course>/branches/<branch>/element/collection/view/<path:path>")
def collection_element_view(course, branch, path):
    """Show a Collection"""
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'view', element)

    return render_template('elements/collection/view.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/collection/correct/<path:path>")
def collection_element_correct(course, branch, path):
    """Show a Collection"""
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'correct', element)

    return render_template('elements/collection/correct.html', element=element)


@app.route("/courses/<course>/branches/<branch>/element/collection/create", methods=["GET", "POST"])
def collection_element_create(course, branch):
    form = CollectionForm(request.form, path=request.args['path'])

    if request.method == 'POST' and form.validate():
        meta = {
            'children': [],
            'type': 'Collection',
            'showOnlyHeaders': form['showOnlyHeaders'].data
        }

        create_element(course, branch, form['path'].data, meta, addToParent=True)

        return redirect(url_for('element_view', course=course, branch=branch, path=form['path'].data))
    else:
        return render_template('elements/collection/create.html', form=form)


@app.route("/courses/<course>/branches/<branch>/element/collection/edit/<path:path>", methods=["GET", "POST"])
def collection_element_edit(course, branch, path):
    element = load_element(course, branch, path)
    checkPermissionForElement(g.user, 'edit', element)
    form = CollectionForm(request.form, element, showOnlyHeaders=element.meta['showOnlyHeaders'])

    if request.method == 'POST' and form.validate():
        element.meta['showOnlyHeaders'] = form['showOnlyHeaders'].data

        element.save()

        if form['path'].data != path:
            element.move(form['path'].data)

        return redirect(url_for('element_view', course=course, branch=branch, path=form['path'].data))
    else:
        return render_template('elements/collection/edit.html', form=form, element=element)


class MoveException(Exception):
    pass


@app.route("/courses/<course>/branches/<branch>/element/collection/move/<int:index>/<direction>/<path:path>", methods=["POST"])
def collection_move_element(branch, course, path, index, direction):
    element = load_element(course, branch, path)

    if direction == "up":
        if index <= 0:
            raise MoveException("Invalid index")
    elif direction == "down":
        if index >= len(element.meta['children']) - 1:
            raise MoveException("Invalid index")
    else:
        raise MoveException("Invalid direction")

    element.moveChild(index, direction)

    return redirect(url_for('element_edit', course=course, branch=branch, path=path))
