def login(app, user, password):
    return app.post('/login', data=dict(
        username=user,
        password=password
    ), follow_redirects=True)


def logout(app):
    return app.post('/logout', follow_redirects=True)
