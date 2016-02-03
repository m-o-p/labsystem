from flask import Flask, session, request, g
from flask_babel import Babel

from playhouse.flask_utils import FlaskDB, get_object_or_404

app = Flask(__name__)
app.config.from_object(__name__)
app.config.from_pyfile('../settings.cfg')
app.config.from_envvar('APP_SETTINGS', silent=True)

database = FlaskDB(app)

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

from entities import User


@app.before_request
def getUser():
    if 'user' in session:
        print('logged in' + str(session['user']))
        g.user = get_object_or_404(User, User.id == session['user'])
