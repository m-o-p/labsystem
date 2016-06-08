import re
import os
import io
import shutil

from git import Repo

from app import app

shaRegex = re.compile('[a-f0-9]{40}')


def read(course, branch, path):
    if shaRegex.match(branch):
        repo = Repo(os.path.join(app.config['COURSES_DIR'], course, 'master'))
        blob = repo.commit(branch).tree[path]
        return blob.data_stream
    else:
        return io.open(os.path.join(app.config['COURSES_DIR'], course, branch, path), 'rb')


class GitStorageError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


def write(course, branch, path):
    if shaRegex.match(branch):
        raise GitStorageError('Can only write to checked out branches')

    return io.open(os.path.join(app.config['COURSES_DIR'], course, branch, path), 'w')


def delete(course, branch, path):
    if shaRegex.match(branch):
        raise GitStorageError('Can only delete from checked out branches')

    os.remove(os.path.join(app.config['COURSES_DIR'], course, branch, path))


def deleteDirectory(course, branch, path):
    if shaRegex.match(branch):
        raise GitStorageError('Can only delete from checked out branches')

    shutil.rmtree(os.path.join(app.config['COURSES_DIR'], course, branch, path))


def createBranch(course, source, to):
    repo = Repo(os.path.join(app.config['COURSES_DIR'], course, 'master'))
    repo.create_head(to, source)

    repo.git.worktree('add', os.path.abspath(os.path.join(app.config['COURSES_DIR'], course, to)), to)


def deleteBranch(course, branch):
    repo = Repo(os.path.join(app.config['COURSES_DIR'], course, 'master'))
    repo.delete_head(branch)
    shutil.rmtree(os.path.join(app.config['COURSES_DIR'], course, branch))


def listCheckedOutBranches(course):
    return os.listdir(os.path.join(app.config['COURSES_DIR'], course))


def listAvailableBranches(course):
    repo = Repo(os.path.join(app.config['COURSES_DIR'], course, 'master'))
    return map(lambda x: str(x), repo.heads)


def createCourse(course):
    os.makedirs(app.config['COURSES_DIR'], course)
    repo = Repo.init(os.path.join(app.config['COURSES_DIR'], course, 'master'))
    os.makedirs(app.config['COURSES_DIR'], course, 'master', 'content')
    io.open(app.config['COURSES_DIR'], course, 'master', 'README', 'w+').close()
    repo.git.add('README')
    repo.git.commit('-m', 'Initial commit')


def deleteCourse(course):
    shutil.rmtree(os.path.join(app.config['COURSES_DIR'], course))


def listCourses():
    return os.listdir(app.config['COURSES_DIR'])


def getHistory(course, branch, paths, offset, limit):
    repo = Repo(os.path.join(app.config['COURSES_DIR'], course, 'master'))
    return repo.iter_commits(branch, paths, max_count=limit, skip=offset)
