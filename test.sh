#!/bin/bash -e

source ./env.sh
cd $APP_DIR
python3 tests/test.py
