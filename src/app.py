from flask import Flask, session, request
from flask_babel import Babel
import yaml

from playhouse.flask_utils import FlaskDB

app = Flask(__name__)
app.config.from_object(__name__)
app.config.from_pyfile('../settings.cfg')
app.config.from_envvar('APP_SETTINGS', silent=True)

database = FlaskDB(app)


def setup():
    from entities import create_tables

    create_tables()

    import controllers
    import routes


@app.context_processor
def inject_user():
    def loadYAML(string):
        return yaml.load(string)
    return dict(loadYAML=loadYAML)


@app.context_processor
def register_min():
    def min(a, b):
        if a < b:
            return a
        else:
            return b

    return dict(min=min)

babel = Babel(app)


@babel.localeselector
def get_locale():
    if 'locale' in session:
        return session['locale']

    return request.accept_languages.best_match(['en', 'de'])


@babel.timezoneselector
def get_timezone():
    if 'timezone' in session:
        return session['timezone']
