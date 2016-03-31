from flask import render_template, g

from app import app

from entities import load_element, checkPermissionForElement


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
