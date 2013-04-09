#!/bin/bash
find "$(dirname "$0")" -type d -exec sh -c "folder=\"{}\" && [ -x \"\$folder/install.sh\" ] && cd \"\$folder\" && ./install.sh" ";"
