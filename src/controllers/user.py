from app import app
from entities import User
from flask import session, g
from playhouse.flask_utils import get_object_or_404


@app.before_request
def getUser():
    if 'user' in session:
        g.user = get_object_or_404(User, User.id == session['user'])
