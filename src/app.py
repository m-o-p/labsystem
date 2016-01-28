from flask import Flask

from playhouse.flask_utils import FlaskDB

app = Flask(__name__)
app.config.from_object(__name__)
app.config.from_pyfile('../settings.cfg')
app.config.from_envvar('APP_SETTINGS', silent=True)

database = FlaskDB(app)
