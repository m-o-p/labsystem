from flask import redirect, url_for

from app import app

from entities import load_element


@app.route("/courses/<course>/branches/<branch>/element/view/<path:path>")
def element_view(course, branch, path):
    element = load_element(course, branch, path)

    if element.meta['type'] == 'Display':
        return redirect(url_for('display_element_view', course=course, branch=branch, path=path))
    elif element.meta['type'] == 'Collection':
        return redirect(url_for('collection_element_view', course=course, branch=branch, path=path))
    else:
        return ''


@app.route("/courses/<course>/branches/<branch>/element/edit/<path:path>")
def element_edit(course, branch, path):
    element = load_element(course, branch, path)

    if element.meta['type'] == 'Display':
        return redirect(url_for('display_element_edit', course=course, branch=branch, path=path))
    elif element.meta['type'] == 'Collection':
        return redirect(url_for('collection_element_edit', course=course, branch=branch, path=path))
    else:
        return ''


@app.route("/courses/<course>/branches/<branch>/element/delete/<path:path>")
def element_delete(course, branch, path):
    element = load_element(course, branch, path)

    if element.meta['type'] == 'Display':
        return redirect(url_for('display_element_delete', course=course, branch=branch, path=path))
    elif element.meta['type'] == 'Collection':
        return redirect(url_for('collection_element_delete', course=course, branch=branch, path=path))
    else:
        return ''
