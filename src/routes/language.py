from flask import session, redirect, request
from flask_babel import get_locale, lazy_gettext

from app import app


@app.route("/set_language/<language>")
def set_language(language):
    """Set the application language"""

    session['locale'] = language

    return redirect(request.args['back'])


languages = {'de': {'name': lazy_gettext('German'), 'flag': 'de'}, 'en': {'name': lazy_gettext('English'), 'flag': 'us'}}


@app.context_processor
def register_language_helpers():
    def getAvailableLanguages():
        return languages

    def getLanguage():
        return get_locale()

    return dict(getAvailableLanguages=getAvailableLanguages, getLanguage=getLanguage)
