import os
import mimetypes

from flask import redirect, url_for, make_response

from app import app
import storage

from entities import load_element


@app.route("/courses/<course>/branches/<branch>/element/view/<path:path>")
def element_view(course, branch, path):
    element = load_element(course, branch, path)

    if element.meta['type'] == 'Display':
        return redirect(url_for('display_element_view', course=course, branch=branch, path=path))
    elif element.meta['type'] == 'Collection':
        return redirect(url_for('collection_element_view', course=course, branch=branch, path=path))
    elif element.meta['type'] == 'Assignment':
        return redirect(url_for('collection_element_view', course=course, branch=branch, path=path))
    else:
        return ''


@app.route("/courses/<course>/branches/<branch>/files/view/<path:path>")
def file_view(course, branch, path):
    data = storage.read(course, branch, os.path.join('content', path)).read()
    (type, encoding) = mimetypes.guess_type(path)

    response = make_response(data)

    response.mimetype = type

    return response


@app.route("/courses/<course>/branches/<branch>/element/edit/<path:path>")
def element_edit(course, branch, path):
    element = load_element(course, branch, path)

    if element.meta['type'] == 'Display':
        return redirect(url_for('display_element_edit', course=course, branch=branch, path=path))
    elif element.meta['type'] == 'Collection':
        return redirect(url_for('collection_element_edit', course=course, branch=branch, path=path))
    elif element.meta['type'] == 'Question':
        if element.meta['questionType'] == 'Text':
            return redirect(url_for('question_element_edit', course=course, branch=branch, path=path))
        elif element.meta['questionType'] == 'MultipleChoice':
            return redirect(url_for('mc_element_edit', course=course, branch=branch, path=path))
        else:
            return ''
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
