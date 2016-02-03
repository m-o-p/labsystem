#!/bin/bash -e

virtualenv-3.4 env
source ./env.sh
pip install -r requirements.txt

bower install

./translations.sh
