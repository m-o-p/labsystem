#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source $DIR/env/bin/activate
export APP_DIR=$DIR
export PYTHONPATH=$PYTHONPATH:$APP_DIR/src/
