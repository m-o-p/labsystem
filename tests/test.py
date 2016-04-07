import os
import unittest
import tempfile

from peewee import Using, SqliteDatabase

import app

from login import login, logout
from entities import AllDbEntities


class FlaskrTestCase(unittest.TestCase):

    def setUp(self):
        self.db_fd, self.dbname = tempfile.mkstemp(prefix='labsystem_db_', suffix='.db', dir='.')

        app.app.config['DATABASE'] = 'sqlite:///' + self.dbname
        self.db = SqliteDatabase(self.dbname)

        app.app.config['TESTING'] = True

        self.app = app.app.test_client()
        self.using = Using(self.db, AllDbEntities)

        self.using.__enter__()
        app.setup()

    def tearDown(self):
        self.using.__exit__(None, None, None)

        os.unlink(self.dbname)
        os.close(self.db_fd)

    def test_login_admin(self):
        rv = login(self.app, 'admin', 'admin')

        assert 'admin' in rv.data.decode("utf-8")

    def test_invalid_login(self):
        rv = login(self.app, 'admin', 'blah')

        assert 'Invalid login' in rv.data.decode("utf-8")

    def test_invalid_login_then_add_user(self):
        rv = login(self.app, 'user1', 'user1')

        assert 'Invalid login' in rv.data.decode("utf-8")

        login(self.app, 'admin', 'admin')

        rv = self.app.post('/user/create', data=dict(
            username='user1',
            password='user1pass',
            confirm='user1pass',
            name='User 1',
            email='user1@test.com'
        ), follow_redirects=True)

        assert 'User 1' in rv.data.decode("utf-8")

        logout(self.app)

        rv = login(self.app, 'user1', 'user1pass')

        assert 'Invalid login' not in rv.data.decode("utf-8")


if __name__ == '__main__':
    unittest.main()
