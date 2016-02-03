#!/bin/bash -e

source ./env.sh
cd $APP_DIR/src

pybabel extract -F babel.cfg -k lazy_gettext -o messages.pot .
pybabel update -i messages.pot -d translations
pybabel compile -d translations
