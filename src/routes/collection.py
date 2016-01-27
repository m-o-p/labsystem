from flask import render_template

from app import app

from entities import load_element


@app.route("/courses/<course>/branches/<branch>/element/collection/view/<path:path>")
def collection_element_view(course, branch, path):
    element = load_element(course, branch, path)

    return render_template('elements/collection/view.html', element=element)
